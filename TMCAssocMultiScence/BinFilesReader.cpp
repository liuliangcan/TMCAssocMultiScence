/* 
 * File:   BinFilesReader.cpp
 * Author: LIU
 * 
 * Created on 2015年12月7日, 上午10:39
 */

#include "BinFilesReader.h"
#include "AssocFiles.h"
#include <fstream>
#include "tools/log.h"
#include "GlobalConfiger.h"
#include "tools/toolkit.h"
#include "DataDefine.h"
#include "FilesMemoryManager.h"
#include "OutPutCSVFiles.h"
#include <limits.h>

using namespace std;

BinFilesReader::BinFilesReader()
{
    opcsv.SetSOutputDir(GlobalConfiger::GetInstance()->GetOutputDir());
}

BinFilesReader::BinFilesReader(const BinFilesReader& orig)
{
}

BinFilesReader::~BinFilesReader()
{
}

 
//读取一个文件，flag0为场景文件，否则为mr文件
int BinFilesReader::ReadOneFile(FAndP* fap, int flag)
{    
    unsigned int struct_size = sizeof(st_XDR_INFO);
    char filename[512] = "";
    if(flag == 0)
    {
        sprintf(filename, "%s%s", fap->file, DEALING_TMP_STRING);
    }
    else
    {
        struct_size = sizeof(st_RC_XDR_INFO);
        strcpy(filename, fap->file);
    }
    ifstream infile(filename,  ios::in|ios::binary); 
    if(!infile.is_open())
    {
        LOG_ERROR("[BinFilesReader]文件打开失败[%s]", filename);
        return -1;
    }        
    
    //获取文件大小
    unsigned long long start, end;
    start = infile.tellg();
    infile.seekg(0, ios::end);
    end = infile.tellg();
    //本文件中数据个数为文件大小除以结构体大小
    unsigned long long file_size = end - start;
    //如果文件大小错误
    if(file_size < struct_size  || file_size % struct_size != 0)
    {
        LOG_ERROR("[BinFilesReader]文件格式错误%s[%lld byte]", filename, file_size);
        //将源文件转移        
        if(flag == 0)MoveScenceFile(fap);
        if(infile.is_open())
        {
            infile.close();
        }
        return -1;
    }
    fap->num = file_size / struct_size;
    fap->filesize = file_size;
    int ret = fmm.Apply(&(fap->p), fap->filesize);
    if(ret < 0)
    {
        LOG_ERROR("[BinFilesReader]内存申请错误%s[%lld byte]", filename, file_size);
        if(infile.is_open())
        {
            infile.close();
        }
        return ret;
    }
    
   infile.seekg(ios::beg);         
   infile.read( (char *)(fap->p), file_size);       
    
    if(infile.is_open())
    {
        infile.close();
    }
    return 0;
}
//读文件
int BinFilesReader::ReadFiles()
{
    fmm.Collect();
    if(m_pFiles->ScenceFile.file[0] != 0)
    {
        int ret = ReadOneFile(&m_pFiles->ScenceFile, 0);
        if(ret < 0)
        {
            LOG_ERROR("[BinFilesReader]场景文件读取失败%s", m_pFiles->ScenceFile.file);
            return ret;
        }
     }
    list<FAndP>::iterator it_lst = m_pFiles->MRFiles.begin();
    list<FAndP>::iterator it_lst_end = m_pFiles->MRFiles.end();
    for(;it_lst != it_lst_end;++it_lst)
    {
        ReadOneFile(&(*it_lst), 1);
    }
    
    return 0;
}
int BinFilesReader::BackupScenceFile()
{
    if(m_pFiles->ScenceFile.file[0] == 0)return -1;
    char srcName[PATH_MAX] = "";
    sprintf(srcName, "%s%s", m_pFiles->ScenceFile.file, DEALING_TMP_STRING);

    char dstName[PATH_MAX] = "";
    char name[NAME_MAX] = "";

   char *ptr = strrchr(m_pFiles->ScenceFile.file, '/');
   if(ptr != NULL) 
   {
       *ptr = 0;
       strcpy(name, ptr + 1);
   }

    char dstDir[PATH_MAX] = "";
    sprintf(dstDir, "%s/S1Scence/%s", GlobalConfiger::GetInstance()->GetBackupDir().c_str(), m_pFiles->day);

    if(!F_DirIsExist(dstDir))
    {
        int ret = F_MakeDirs(dstDir, 1);
        if(ret != 0)
        {
            LOG_ERROR("[BinFilesReader]备份目录: %s创建失败， 请检查！errno = %d", dstDir,ret);
            return -1;
        }
    }
     sprintf(dstName, "%s/%s", dstDir ,name);
     rename(srcName, dstName);
    return 0;
}
int BinFilesReader::BackupMRFile(FAndP* fap)
{
    if(fap->file[0] == 0)return -1;
    char backupDir[PATH_MAX];
    char name[NAME_MAX];
    char dstFile[PATH_MAX];
    char dayAndHourDir[PATH_MAX];
    
    char *ptr = strrchr(fap->file, '/');
    char *ptr2 = ptr;
   //拆分文件路径特征
    int count = 0;    
    for(ptr = fap->file + strlen(fap->file) - 1;ptr != fap->file; --ptr)
    {
        if(*ptr == '/')
        {
            ++count;
        }
        if(count == 1)
        {
            ptr2 = ptr;
            *ptr = 0;               
            strcpy(name, ptr + 1);
        }
        else if(count == 4)
        {
            strcpy(dayAndHourDir, ptr + 1);
            *ptr2 = '/';            
            break;
        }
    }
    //备份MR
    sprintf(backupDir, "%s/%s",GlobalConfiger::GetInstance()->GetBackupDir().c_str(),dayAndHourDir);
    if(!F_DirIsExist(backupDir))
    {
        int ret = F_MakeDirs(backupDir, 1);
        if(ret != 0)
        {
            LOG_ERROR("[BinFilesReader]输出目录: %s创建失败， 请检查！errno = %d\n", backupDir, ret);
            return -1;
        }
    }
    sprintf(dstFile, "%s/%s", backupDir, name);
    rename(fap->file, dstFile); 
    return 0;
}
int BinFilesReader::Backup()
{
    BackupScenceFile();
    list<FAndP>::iterator it_lst = m_pFiles->MRFiles.begin();
    list<FAndP>::iterator it_lst_end = m_pFiles->MRFiles.end();
    for(;it_lst != it_lst_end;++it_lst)
    {
        BackupMRFile(&(*it_lst));
    }
    return 0;
}
//场景文件只做rename
int BinFilesReader::MoveScenceFile(FAndP* fap)
{
    if(fap->p != NULL)delete[]fap->p;
    
    char srcName[512] = "";
    sprintf(srcName, "%s%s", fap->file, DEALING_TMP_STRING);

    char dstName[512] = "";
    char day[16] = "";
    char name[32] = "";

   char *ptr = strrchr(fap->file, '/');
   if(ptr != NULL) 
   {
       *ptr = 0;
       strcpy(name, ptr + 1);
   }
    ptr = strrchr(fap->file, '/');        
   if(ptr != NULL)
   {
       strcpy(day, ptr + 1);
   }

    char dstDir[512] = "";
    sprintf(dstDir, "%s/S1Scence/%s", GlobalConfiger::GetInstance()->GetBackupDir().c_str(), day);

    if(!F_DirIsExist(dstDir))
    {
        int ret = F_MakeDirs(dstDir, 1);
        if(ret != 0)
        {
            LOG_ERROR("[BinFilesReader]备份目录: %s创建失败， 请检查！errno = %d", dstDir,ret);
            return -1;
        }
    }
     sprintf(dstName, "%s/%s", dstDir ,name);
     rename(srcName, dstName);
    return 0;
}
//写MR文件
int BinFilesReader::WriteMRFile(FAndP* fap)
{
    char outFile[512] = "";
    char outDir[512] = "";
    char dayAndHourDir[64] = "";
    char name[32] = "";
    char *ptr = strrchr(fap->file, '/');
    char *ptr2 = ptr;
   //拆分文件路径特征
    int count = 0;    
    for(ptr = fap->file + strlen(fap->file) - 1;ptr != fap->file; --ptr)
    {
        if(*ptr == '/')
        {
            ++count;
        }
        if(count == 1)
        {
            ptr2 = ptr;
            *ptr = 0;               
            strcpy(name, ptr + 1);
        }
        else if(count == 4)
        {
            strcpy(dayAndHourDir, ptr + 1);
            break;
        }
    }
    //写mr文件
    sprintf(outDir, "%s/%s",GlobalConfiger::GetInstance()->GetOutputDir().c_str(),dayAndHourDir);
    if(!F_DirIsExist(outDir))
    {
        int ret = F_MakeDirs(outDir, 1);
        if(ret != 0)
        {
            LOG_ERROR("[BinFilesReader]输出目录: %s创建失败， 请检查！errno = %d\n", outDir,ret);
            if(fap->p!=NULL)delete[]fap->p;
            return -1;
        }
    }
    sprintf(outFile, "%s/%s", outDir, name);
    char tarFilenameTmp[512] = "";
    sprintf(tarFilenameTmp, "%s%s", outFile, OUTPUT_MR_TMP_STRING);
    ofstream outfile(tarFilenameTmp, ios::out|ios::binary);
    if(!outfile.is_open())
    {
        LOG_ERROR("[BinFilesReader]文件打开失败[%s]",tarFilenameTmp);
        if(fap->p!=NULL)delete[]fap->p;
        return -1;
    }
    outfile.write((char*)fap->p, fap->filesize);    
    if(outfile.is_open())
    {
        outfile.close();
    }
    if(fap->p!=NULL)delete[]fap->p;
    rename(tarFilenameTmp, outFile);
    
    //备份MR
    sprintf(outDir, "%s/%s",GlobalConfiger::GetInstance()->GetBackupDir().c_str(),dayAndHourDir);
    if(!F_DirIsExist(outDir))
    {
        int ret = F_MakeDirs(outDir, 1);
        if(ret != 0)
        {
            LOG_ERROR("[BinFilesReader]输出目录: %s创建失败， 请检查！errno = %d\n", outDir, ret);
            return -1;
        }
    }
    sprintf(outFile, "%s/%s", outDir, name);
    *ptr2 = '/';
    rename(fap->file, outFile);    
    
    return 0;
}
//写一个文件，flag0为场景文件，否则为mr文件
int BinFilesReader::WriteOneFile(FAndP* fap, int flag)
{
    //场景文件只做备份
    if(flag == 0)
    {
       MoveScenceFile(fap);
    }
    //写mr文件，并备份
    else
    {
        WriteMRFile(fap);
    }
    return 0;
}
//写文件
int BinFilesReader::WriteFiles()
{
    int ret = opcsv.OutputCsv(this->m_pFiles);
    
    if(ret <= 0)
    {
        LOG_ERROR("[Assoc][日期%s,组号%s]CSV文件输出结束,输出成功条数[%d]", m_pFiles->day, m_pFiles->imsiGroup, ret);
        return ret;
    }
    LOG_NOTICE("[Assoc][日期%s,组号%s]CSV文件输出结束,输出成功条数[%d]", m_pFiles->day, m_pFiles->imsiGroup, ret);
    /**
    WriteOneFile(&m_pFiles->ScenceFile, 0);
    
    list<FAndP>::iterator it_lst = m_pFiles->MRFiles.begin();
    list<FAndP>::iterator it_lst_end = m_pFiles->MRFiles.end();
    for(;it_lst != it_lst_end;++it_lst)
    {
        WriteOneFile(&(*it_lst), 1);
    }*/
    return 0;
}