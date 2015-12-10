/* 
 * File:   GlobalConfiger.h
 * Author: LIU
 *
 * Created on 2015年12月3日, 下午1:45
 */

#ifndef GLOBALCONFIGER_H
#define	GLOBALCONFIGER_H

#include <string>
#include <list>

class GlobalConfiger {
public:
    GlobalConfiger();
    GlobalConfiger(const GlobalConfiger& orig);
    virtual ~GlobalConfiger();
    
    int Init(char* configXML);
    
    //单例
    static GlobalConfiger *GetInstance()
    {
            if(m_instance_gc == NULL)
            {
                    m_instance_gc = new GlobalConfiger();
            }
            return m_instance_gc;
    }
    std::string GetUMultiScenceEndBatchFile() const;
    int GetIThreadNum() const;
    std::string GetBackupDir() const;
    std::string GetLogDir() const;
    std::string GetOutputDir() const;
    std::string GetScenceInputDir() const;
    void GetListMRInputDir(std::list<std::string>&_list) const;
    int GetIDeviation() const;
private:
    //输入目录
    std::list<std::string> m_listMRInputDir;
    //输入目录
    std::string m_ScenceInputDir;
    //输出目录
    std::string m_outputDir;
    //日志输出目录
    std::string m_logDir;
    //备份目录
    std::string m_backupDir;
    //线程数
    int m_iThreadNum;
    //关联时间误差范围，单位毫秒
    int  m_iDeviation;
    
    
    //标记文件名
    std::string m_uMultiScenceEndBatchFile;
    
    int check();
    
    
    //单例
    static GlobalConfiger *m_instance_gc;
};

#endif	/* GLOBALCONFIGER_H */

