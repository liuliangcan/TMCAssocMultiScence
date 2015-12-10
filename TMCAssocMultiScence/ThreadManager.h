/* 
 * File:   ThreadManager.h
 * Author: LIU
 *
 * Created on 2015年12月1日, 下午2:07
 */

#ifndef THREADMANAGER_H
#define	THREADMANAGER_H

#include "tools/macro.h"
#include "AssocFiles.h" 
#include <queue>
#include <string>

#if __cplusplus < 201103L
#include <tr1/unordered_map>
#include <tr1/unordered_set>
#define MAP tr1::unordered_map
#define SET tr1::unordered_set
#else

#include <unordered_map>
#include <unordered_set>
#define MAP unordered_map
#define SET unordered_set
#endif
class ThreadManager {
public:

    int Init();

    int Start();



    //单例

    static ThreadManager *GetInstance() {
        if (m_instance_tm == NULL) {
            m_instance_tm = new ThreadManager();
        }
        return m_instance_tm;
    }
    ThreadManager();
    ThreadManager(const ThreadManager& orig);
    virtual ~ThreadManager();
private:
    friend THREAD_RETURN THREAD_PROC FileHandleProc(THREAD_PARAM param);
    //启动的处理线程数
    int m_ithreadNum;

    //输入目录
    std::list<std::string> m_listMRInputDir;
    //输入目录
    std::string m_ScenceInputDir;

    //读取到的文件列表
    std::queue<AssocFiles> m_qFileslist;
    
    //当前正在处理的日期
    std::SET<int> m_setDealing;

    //日志输出目录
    std::string m_logDir;

    //文件列表锁
    CPO m_cpo_qFileslist;

    //读取文件列表
    int GetFileList();
    //遍历20150606这种目录下的文件
    int GetChildDir(char * dir, bool bFirstTimeFlag);
    //通过场景文件找MR文件
    int FindMRFiles(AssocFiles& files);

    //批次处理完毕后，将未处理的MR文件移走
    int MvMRFiles();
    //批次处理完毕后，将未处理的MR文件备份
   // int BackupMRFiles();
    //遍历MR目录
    int GetMRDayChildDir(char *sSrcFileFullPath);
    int GetMRHourChildDir(char *sSrcFileFullPath);
    
    int SpliteLogFile();
    //单例
    static ThreadManager *m_instance_tm;
};

#endif	/* THREADMANAGER_H */

