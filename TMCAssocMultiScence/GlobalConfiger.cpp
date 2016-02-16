/* 
 * File:   GlobalConfiger.cpp
 * Author: LIU
 * 
 * Created on 2015年12月3日, 下午1:45
 */

#include "GlobalConfiger.h"
#include "tools/Markup.h"
#include <stdio.h>
#include "tools/toolkit.h"

GlobalConfiger* GlobalConfiger::m_instance_gc = NULL;
GlobalConfiger::GlobalConfiger()
{
}

std::string GlobalConfiger::GetUMultiScenceEndBatchFile() const
{
    return m_uMultiScenceEndBatchFile;
}

int GlobalConfiger::GetIThreadNum() const
{
    return m_iThreadNum;
}

std::string GlobalConfiger::GetBackupDir() const
{
    return m_backupDir;
}

std::string GlobalConfiger::GetLogDir() const
{
    return m_logDir;
}

std::string GlobalConfiger::GetOutputDir() const
{
    return m_outputDir;
}

std::string GlobalConfiger::GetScenceInputDir() const
{
    return m_ScenceInputDir;
}

void GlobalConfiger::GetListMRInputDir(std::list<std::string>&_list) const
{
    //_list.insert(m_listMRInputDir.begin(), m_listMRInputDir.end());
    std::copy(m_listMRInputDir.begin(), m_listMRInputDir.end(), std::back_inserter(_list));
}

int GlobalConfiger::GetIDeviation() const
{
    return m_iDeviation;
}

GlobalConfiger::GlobalConfiger(const GlobalConfiger& orig)
{
}

GlobalConfiger::~GlobalConfiger()
{
}

int GlobalConfiger::Init(char* configXML)
{
    m_listMRInputDir.clear();
    m_ScenceInputDir = "MRINPUTDIR";
    m_outputDir = "OUTDIR";
    m_logDir = "LOG";
    m_backupDir = "BACKDIR";
    m_iThreadNum = 0;
    m_iDeviation = 3600000;
    CMarkup cmxml;
    bool b = cmxml.Load(configXML);  
    if(!b)
    {
        fprintf(stderr, "[GlobalConfiger]配置文件读取失败！%s\n",configXML);
        return -1;
    }
     while (cmxml.FindChildElem("AssocMRInputDir")) {  
        m_listMRInputDir.push_back(cmxml.GetChildAttrib("name")); 
    }
    cmxml.ResetMainPos();
    if (cmxml.FindChildElem("AssocScenceInputDir"))
    {
         m_ScenceInputDir = cmxml.GetChildAttrib("name");
    }
    cmxml.ResetMainPos();
    if (cmxml.FindChildElem("AssocOutputDir"))
    {
         m_outputDir = cmxml.GetChildAttrib("name");
    }
    cmxml.ResetMainPos();
    if (cmxml.FindChildElem("AssocLogDir"))
    {
         m_logDir = cmxml.GetChildAttrib("name");
    }
    cmxml.ResetMainPos();
    if (cmxml.FindChildElem("AssocBackupDir"))
    {
         m_backupDir = cmxml.GetChildAttrib("name");
    }
    cmxml.ResetMainPos();
    if (cmxml.FindChildElem("AssocThreadNum"))
    {
         m_iThreadNum = atoi(cmxml.GetChildAttrib("name").c_str());
    }    
    cmxml.ResetMainPos();
    if (cmxml.FindChildElem("MultiScenceEndBatchFile"))
    {
        m_uMultiScenceEndBatchFile = cmxml.GetChildAttrib("name");        
    }
    cmxml.ResetMainPos();
    if (cmxml.FindChildElem("AssocDeviation"))
    {
        m_iDeviation = atoi(cmxml.GetChildAttrib("name").c_str());       
    }
    
    int ret = check();
    if(ret < 0)return -1;
    return 0;
}
int GlobalConfiger::check()
{
    std::list<std::string>::iterator it_m_listInputDir=  m_listMRInputDir.begin();
    std::list<std::string>::iterator it_m_listInputDir_end=  m_listMRInputDir.end();
    unsigned int inputNum = 0;
    for(;it_m_listInputDir != it_m_listInputDir_end;)
    {
        if(F_DirIsExist((char*)(*it_m_listInputDir).c_str()))
        {
            fprintf(stdout,"输入目录: %s\n",(*it_m_listInputDir).c_str());
            ++inputNum;
            ++it_m_listInputDir;
        }
        else
        {
            fprintf(stderr,"输入目录: %s,检查不存在， 忽略此目录\n",(*it_m_listInputDir).c_str());
            it_m_listInputDir = m_listMRInputDir.erase(it_m_listInputDir);
        }
    }
    if(!F_DirIsExist((char*)m_ScenceInputDir.c_str()))
    {
        fprintf(stderr,"输入目录: %s,检查不存在， 退出\n",m_ScenceInputDir.c_str());
        return -1;
    }
    if(inputNum == 0)
    {
        fprintf(stderr,"输入目录不可用， 程序退出！\n");
        return -1;
    }
    if(!F_DirIsExist((char*)m_outputDir.c_str()))
    {
        fprintf(stdout,"输出目录: %s不存在，",m_outputDir.c_str());
        int ret = F_MakeDirs((char*)m_outputDir.c_str(), 1);
        if(ret != 0)
        {
            fprintf(stderr,"创建失败， 请检查！errno = %d\n",ret);
            return -1;
        }
        else
        {
            fprintf(stdout,"创建成功。\n");
        }
    }
    else
    {
        fprintf(stdout,"输出目录: %s检测到存在。\n",m_outputDir.c_str());
    }
    if(!F_DirIsExist((char*)m_logDir.c_str())) 
    {
        fprintf(stdout,"日志目录: %s不存在，",m_logDir.c_str());
        int ret = F_MakeDirs((char*)m_logDir.c_str(), 1); 
        if(ret != 0)
        {
            fprintf(stderr,"创建失败， 请检查！errno = %d\n",ret);
            return -1;
        }
        else
        {
            fprintf(stdout,"创建成功。\n");
        }
    }
    else
    {
        fprintf(stdout,"日志目录: %s检测到存在。\n",m_logDir.c_str());
    }
    if(!F_DirIsExist((char*)m_backupDir.c_str()))
    {
        fprintf(stdout,"备份目录: %s不存在，",m_backupDir.c_str());
        int ret = F_MakeDirs((char*)m_backupDir.c_str(), 1); 
        if(ret != 0)
        {
            fprintf(stderr,"创建失败， 请检查！errno = %d\n",ret);
            return -1;
        }
        else
        {
            fprintf(stdout,"创建成功。\n");
        }
    }
    else
    {
        fprintf(stdout,"备份目录: %s检测到存在。\n", m_backupDir.c_str());
    }
    if(m_iThreadNum <= 0)
    {
        fprintf(stderr,"线程数目: %d, 设置错误， 请检查!\n", m_iThreadNum);
        return -1;
    }
    else
    {
        fprintf(stdout,"线程数目: %d, 成功。\n", m_iThreadNum);
    }
    if(m_iDeviation <= 0 || m_iDeviation > 2100000000)
    {
        fprintf(stderr,"关联误差: %d, 设置错误， 请检查!\n", m_iDeviation);
        return -1;
    }
    else
    {
        fprintf(stdout,"关联误差: %dms, 成功。\n", m_iDeviation);
    }
    return 0;
}