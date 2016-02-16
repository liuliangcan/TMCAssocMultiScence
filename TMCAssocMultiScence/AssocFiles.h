/* 
 * File:   assocFilesStruct.h
 * Author: LIU
 *
 * Created on 2015年12月3日, 下午3:41
 */

#ifndef ASSOCFILESSTRUCT_H
#define	ASSOCFILESSTRUCT_H
#include <list>
#include <string.h>
#include <stdio.h>


#define DEALING_TMP_STRING "__ASSOCTMP__"
#define OUTPUT_MR_TMP_STRING "__ASSOCMROUTTNP__"

#ifdef	__cplusplus
extern "C" {
#endif

//文件名+结构体信息    
class FAndP
{
public:
    FAndP()
    {
        file[0] = 0;
        this->p = NULL;
        num = 0;
        filesize = 0;
    }
    FAndP(char* ffff)
    {
        strcpy(file, ffff);
        this->p = NULL;
        num = 0;
        filesize = 0;
    };
    char file[512];//原始文件名
    void* p;//指针
    unsigned num;//结构体size
    unsigned long long filesize; //二进制文件大小
};
//关联文件结构体    
class AssocFiles
{
public:
    FAndP ScenceFile;
    std::list<FAndP> MRFiles;
    char day[32];
    char imsiGroup[16];
private:
    
};

class OutStatistics
{
public :
    unsigned long m_iHttp;
    unsigned long m_iMme;
    unsigned long m_iUemr;
    unsigned long m_iUu;
    unsigned long m_iTotal;
    
    OutStatistics()
    {
        m_iHttp = 0;
        m_iMme = 0;
        m_iUemr = 0;
        m_iUu = 0;
        m_iTotal = 0;
    }
    OutStatistics(unsigned long  http, unsigned long  mme, unsigned long uemr, unsigned long  uu, unsigned long  total) 
    {        
        m_iHttp = http;
        m_iMme = mme;
        m_iUemr = uemr;
        m_iUu = uu;
        m_iTotal = total;
    }
    
    unsigned long Add(const OutStatistics& addO)
    {
        m_iHttp += addO.m_iHttp;
        m_iMme += addO.m_iMme;
        m_iUemr += addO.m_iUemr;
        m_iUu += addO.m_iUu;
        m_iTotal += addO.m_iTotal;
    }
    /*
     * GetString
     * 输出顺序http,mme,uemr,uu,total
     */
    void GetString(char* dst) const
    {        
        sprintf(dst, "%lu,%lu,%lu,%lu,%lu", m_iHttp, m_iMme, m_iUemr, m_iUu, m_iTotal);
    }
    
    void Reset()
    {
        m_iHttp = 0;
        m_iMme = 0;
        m_iUemr = 0;
        m_iUu = 0;
        m_iTotal = 0;
    }
};


#ifdef	__cplusplus
}
#endif

#endif	/* ASSOCFILESSTRUCT_H */

