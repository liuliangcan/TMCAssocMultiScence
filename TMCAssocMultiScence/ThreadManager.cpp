/* 
 * File:   ThreadManager.cpp
 * Author: LIU
 * 
 * Created on 2015年12月1日, 下午2:07
 */

#include "ThreadManager.h"
#include "GlobalConfiger.h"
#include "tools/log.h"
#include "tools/toolkit.h"
#include "Assoc.h"


using namespace std;

ThreadManager* ThreadManager::m_instance_tm = NULL;

ThreadManager::ThreadManager()
{
}

ThreadManager::ThreadManager(const ThreadManager& orig)
{
}

ThreadManager::~ThreadManager()
{
    CPO_Dele(m_cpo_qFileslist);
}

int ThreadManager::Init()
{
    CPO_Init(m_cpo_qFileslist);

    GlobalConfiger *gc = GlobalConfiger::GetInstance();
    m_ithreadNum = gc->GetIThreadNum();
    gc->GetListMRInputDir(m_listMRInputDir);
    m_ScenceInputDir = gc->GetScenceInputDir();
    m_logDir = gc->GetLogDir();

    return 0;
}

THREAD_RETURN THREAD_PROC FileHandleProc(THREAD_PARAM param);

int ThreadManager::Start()
{
    for (int i = 0; i < m_ithreadNum; ++i)
    {
        THREAD_ID handle_FileHandleProc = NULL;
        handle_FileHandleProc = bcreate_thread(FileHandleProc, this);

        if (FileHandleProc == NULL)
        {
            LOG_ERROR("[FileHandleProc]创建线程失败");
            return -1;
        }
    }
    LOG_NOTICE("[FileHandleProc]启动%d个线程", m_ithreadNum);
    return 0;
}
unsigned iDealing = 0;

THREAD_RETURN THREAD_PROC FileHandleProc(THREAD_PARAM param)
{
    ThreadManager* pParam = (ThreadManager*) param;
    bool threadflag = true;
    AssocFiles files;
    Assoc assoc;
    assoc.Init();
    while (threadflag)
    {
        CPO_Enter(pParam->m_cpo_qFileslist);
        if (pParam->m_qFileslist.empty())
        {//检查m_qFilelist队列，如果为空，休息五秒，否则取出来一个文件进行处理
            pParam->GetFileList();
            if (pParam->m_qFileslist.empty())
            {
                //如果存在结束标志文件，认为处理完了。
                //等到全部线程结束，处理剩余mr文件
                if (iDealing == 0 && F_IsPathExist((char*) GlobalConfiger::GetInstance()->GetUMultiScenceEndBatchFile().c_str()))
                {
                    //pParam->BackupMRFiles();
                    pParam->MvMRFiles();                    
                    pParam->m_setDealing.clear();
                    //删除结束标志文件
                    remove((char*) GlobalConfiger::GetInstance()->GetUMultiScenceEndBatchFile().c_str());
                    //分割日志增加日期+时间戳    
                    pParam->SpliteLogFile();
                }
            }
            CPO_Leave(pParam->m_cpo_qFileslist);

            sleep(5);
        }
        else
        {
            ++iDealing;
            files = pParam->m_qFileslist.front();
            pParam->m_qFileslist.pop();
            string tmps = "";
            string tmpname = tmps + files.ScenceFile.file + DEALING_TMP_STRING;
            rename(files.ScenceFile.file, tmpname.c_str());

            CPO_Leave(pParam->m_cpo_qFileslist);
            sleep(0);

            LOG_NOTICE("[Assoc]开始处理文件:%s", files.ScenceFile.file);
            assoc.SetFiles(files);
            assoc.Process();

            CPO_Enter(pParam->m_cpo_qFileslist);
            --iDealing;
            CPO_Leave(pParam->m_cpo_qFileslist);
            sleep(0);
        }
    }

    return (THREAD_RETURN) 0;
}

int ThreadManager::SpliteLogFile()
{
    char logfile[PATH_MAX] = "";
    char dstfile[PATH_MAX] = "";
    sprintf(logfile, "%s/AssocMultiScence.acess", GlobalConfiger::GetInstance()->GetLogDir().c_str());
    time_t now = time(0);
    int dnow = now;
    tm tm1 = {0};
    localtime_r(&now, &tm1);
    char day[16];
    strftime(day, sizeof(day) ,"%Y%m%d", &tm1);
    sprintf(dstfile, "%s/AssocMultiScence_%s_%d.acess", GlobalConfiger::GetInstance()->GetLogDir().c_str(), day, dnow);
    rename(logfile, dstfile);    
    
    sprintf(logfile, "%s/AssocMultiScence.error", GlobalConfiger::GetInstance()->GetLogDir().c_str());
    sprintf(dstfile, "%s/AssocMultiScence_%s_%d.error", GlobalConfiger::GetInstance()->GetLogDir().c_str(), day, dnow);
    rename(logfile, dstfile);  
    return 0;
}
/*
int ThreadManager::BackupMRFiles()
{
    list<string>::iterator it_m_listInputDir = m_listMRInputDir.begin();
    list<string>::iterator it_m_listInputDirt_end = m_listMRInputDir.end();
    for (; it_m_listInputDir != it_m_listInputDirt_end; ++it_m_listInputDir)
    {
        char srcDir[512] = "";
        sprintf(srcDir, "%s", it_m_listInputDir->c_str());
        char *ptr = strchr(srcDir, '/');
        char dirname[64] = "";
        strcpy(dirname, ptr + 1);
        char dstDir[512] = "";
        sprintf(dstDir, "%s/%s", GlobalConfiger::GetInstance()->GetBackupDir().c_str(), dirname);
        F_CopyFiles(srcDir, dstDir);
    }
    return 0;
}*/
//通过场景文件找MR文件

int ThreadManager::FindMRFiles(AssocFiles& files)
{
    string fullPath = files.ScenceFile.file;
    string filename;
    string day;
    string dir;
    size_t backslashIndex = fullPath.find_last_of('/');
    if (backslashIndex == string::npos)
    {
        filename = fullPath;
    }
    else
    {
        filename = fullPath.substr(backslashIndex + 1);
        dir = fullPath.substr(0, backslashIndex);
        std::size_t backslashIndex2 = dir.find_last_of('/');
        day = dir.substr(backslashIndex2 + 1);
    }    
    strcpy(files.day, day.c_str());
    
    char mrfilename[16] = "";
    strncpy(mrfilename, filename.c_str() + filename.length() - 10, 6);   
    
    strcpy(files.imsiGroup, day.c_str());
    
    strcpy(mrfilename + 6, ".bin");
    list<string>::iterator it_m_listInputDir = m_listMRInputDir.begin();
    list<string>::iterator it_m_listInputDirt_end = m_listMRInputDir.end();
    for (; it_m_listInputDir != it_m_listInputDirt_end; ++it_m_listInputDir)
    {
        string basedir = *it_m_listInputDir;
        for (int i = 0; i < 24; ++i)
        {
            char ihour[4];
            sprintf(ihour, "%02d", i);
            string mrfullfile = "";
            mrfullfile.append(basedir).append("/").append(day).append("/").append(ihour).append("/").append(mrfilename);
            if (F_DirIsExist((char*) mrfullfile.c_str()))
            {
                files.MRFiles.push_back(FAndP((char*) mrfullfile.c_str()));
            }
        }
    }
    return 0;
}
//遍历20150606这种目录下的文件
int ThreadManager::GetChildDir(char * dir, bool bFirstTimeFlag)
{
    string basedir = dir;

    struct DIR_INFO *hDirInfo;
    struct FILE_INFO hFileInfo;

    char sSrcFileFullPath[1024];
    hDirInfo = F_DirOpen((char*) basedir.c_str());
    if (hDirInfo != NULL)
    {

        while (F_DirNextItem(hDirInfo, &hFileInfo) == 0)
        {
            if (hFileInfo.sName[0] == '.' || hFileInfo.sName[0] == ' ')
                continue;

            if (strcasecmp(hFileInfo.sName + (strlen(hFileInfo.sName) - 4), ".bin") == 0)
            {
                sprintf(sSrcFileFullPath, "%s/%s", basedir.c_str(), hFileInfo.sName);
                AssocFiles files;
                strcpy(files.ScenceFile.file, sSrcFileFullPath);
                //根据文件名组号去找MR和UU文件,并且拼在files里
                FindMRFiles(files);
                m_qFileslist.push(files);
                continue;
            }

            if (bFirstTimeFlag && strcasecmp(hFileInfo.sName + (strlen(hFileInfo.sName) - 12), DEALING_TMP_STRING) == 0)
            {
                sprintf(sSrcFileFullPath, "%s/%s", basedir.c_str(), hFileInfo.sName);
                char sSrcFileFullPathNew[1024];
                strcpy(sSrcFileFullPathNew, sSrcFileFullPath);
                sSrcFileFullPathNew[(strlen(sSrcFileFullPath) - 12)] = 0;
                rename(sSrcFileFullPath, sSrcFileFullPathNew);
                AssocFiles files;
                strcpy(files.ScenceFile.file, sSrcFileFullPathNew);
                //根据文件名组号去找MR和UU文件,并且拼在files里
                FindMRFiles(files);
                m_qFileslist.push(files);
                continue;
            }
        }//end while
    }//end if
    else
    {
        LOG_ERROR("[ThreadManager]打开目录失败！%s", basedir.c_str());
    }
    F_DirClose(hDirInfo);
    return 0;
}
//获取文件列表
//定义的所有目录中的文件全部读入m_qFilelist

int ThreadManager::GetFileList()
{
    static bool bFirstTime = true;

    string basedir = m_ScenceInputDir;

    struct DIR_INFO *hDirInfo;
    struct FILE_INFO hFileInfo;

    char sSrcFileFullPath[1024];
    hDirInfo = F_DirOpen((char*) basedir.c_str());
    if (hDirInfo != NULL)
    {
        while (F_DirNextItem(hDirInfo, &hFileInfo) == 0)
        {
            if (hFileInfo.sName[0] == '.' || hFileInfo.sName[0] == ' ')
                continue;

            if (hFileInfo.bDir == 1)
            {
                int day = atoi(hFileInfo.sName);
                if (day > 20000000)
                {
                    m_setDealing.insert(day);
                    sprintf(sSrcFileFullPath, "%s/%s", basedir.c_str(), hFileInfo.sName);
                    GetChildDir(sSrcFileFullPath, bFirstTime);
                }
            }
        }//end while
    }//end if
    else
    {
        LOG_ERROR("[ThreadManager]打开目录失败！%s", basedir.c_str());
    }
    F_DirClose(hDirInfo);

    bFirstTime = false;
    return 0;
}

int ThreadManager::GetMRDayChildDir(char *sSrcFileFullDir)
{
    string basedir = sSrcFileFullDir;

    struct DIR_INFO *hDirInfo;
    struct FILE_INFO hFileInfo;

    char sSrcFileFullPath[1024];
    hDirInfo = F_DirOpen((char*) basedir.c_str());
    if (hDirInfo != NULL)
    {

        while (F_DirNextItem(hDirInfo, &hFileInfo) == 0)
        {
            if (hFileInfo.sName[0] == '.' || hFileInfo.sName[0] == ' ')
                continue;

            if (hFileInfo.bDir == 1)
            {
                int hour = atoi(hFileInfo.sName);
                if (hour >= 0 && hour < 24)
                {
                    sprintf(sSrcFileFullPath, "%s/%s", basedir.c_str(), hFileInfo.sName);
                    GetMRHourChildDir(sSrcFileFullPath);
                }
            }
        }//end while
    }//end if
    else
    {
        LOG_ERROR("[ThreadManager]打开目录失败！%s", basedir.c_str());
    }
    F_DirClose(hDirInfo);
    return 0;
}

int ThreadManager::GetMRHourChildDir(char *sSrcFileFullDir)
{
    string basedir = sSrcFileFullDir;

    struct DIR_INFO *hDirInfo;
    struct FILE_INFO hFileInfo;

    char sSrcFileFullPath[1024];
    hDirInfo = F_DirOpen((char*) basedir.c_str());
    if (hDirInfo != NULL)
    {

        while (F_DirNextItem(hDirInfo, &hFileInfo) == 0)
        {
            if (hFileInfo.sName[0] == '.' || hFileInfo.sName[0] == ' ')
                continue;

            if (strcasecmp(hFileInfo.sName + (strlen(hFileInfo.sName) - 4), ".bin") == 0)
            {
                sprintf(sSrcFileFullPath, "%s/%s", basedir.c_str(), hFileInfo.sName);
                char dstDir[512] = "";
                char dayAndHour[32] = "";
                char * ptr = sSrcFileFullDir + strlen(sSrcFileFullDir) - 1;
                int count = 0;                
                AssocFiles afs;      
                while (ptr != sSrcFileFullDir)
                {
                    --ptr;
                    if (*ptr == '/')
                    {
                        ++count;
                    }
                    if(count == 3)
                    {
                        strncpy(afs.day, ptr + 1, 8);
                        afs.day[8] = 0;
                    }
                }    
                afs.MRFiles.push_back(FAndP(sSrcFileFullPath));
                strncpy(afs.imsiGroup, hFileInfo.sName + strlen(hFileInfo.sName) - 10, 6);
                afs.imsiGroup[6] = 0;
                
                BinFilesReader bfr;
                bfr.Init(&afs);
                bfr.ReadFiles();
                bfr.WriteFiles();
                bfr.Backup();
                
                continue;
            }

        }//end while
    }//end if
    else
    {
        LOG_ERROR("[ThreadManager]打开目录失败！%s", basedir.c_str());
    }
    F_DirClose(hDirInfo);
    return 0;
}

int ThreadManager::MvMRFiles()
{
    list<string>::iterator it_m_listInputDir = m_listMRInputDir.begin();
    list<string>::iterator it_m_listInputDirt_end = m_listMRInputDir.end();
    for (; it_m_listInputDir != it_m_listInputDirt_end; ++it_m_listInputDir)
    {
        string basedir = *it_m_listInputDir;

        struct DIR_INFO *hDirInfo;
        struct FILE_INFO hFileInfo;

        char sSrcFileFullPath[1024];
        hDirInfo = F_DirOpen((char*) basedir.c_str());
        if (hDirInfo != NULL)
        {

            while (F_DirNextItem(hDirInfo, &hFileInfo) == 0)
            {
                if (hFileInfo.sName[0] == '.' || hFileInfo.sName[0] == ' ')
                    continue;

                if (hFileInfo.bDir == 1)
                {
                    int day = atoi(hFileInfo.sName);
                    std::SET<int>::iterator it_set = m_setDealing.find(day);
                    if (it_set != m_setDealing.end())
                    {
                        sprintf(sSrcFileFullPath, "%s/%s", basedir.c_str(), hFileInfo.sName);
                        GetMRDayChildDir(sSrcFileFullPath);
                    }
                }
            }//end while
        }//end if
        else
        {
            LOG_ERROR("[ThreadManager]打开目录失败！%s", basedir.c_str());
        }
        F_DirClose(hDirInfo);
    }
    return 0;
}