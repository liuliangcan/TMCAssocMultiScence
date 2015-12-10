/* 
 * File:   Assoc.cpp
 * Author: LIU
 * 
 * Created on 2015年12月3日, 下午4:03
 */

#include "Assoc.h"
#include "BinFilesReader.h"
#include "DataDefine.h"
#include <algorithm>
#include <string.h>
#include "tools/log.h"
#include "GlobalConfiger.h"
#include <unistd.h>

using namespace std;

#define XDR_MME_TAG 5
#define XDR_HTTP_TAG 11
#define TIME_DELAY (GlobalConfiger::GetInstance()->GetIDeviation())

Assoc::Assoc()
{
    m_p = NULL;
    
}

Assoc::Assoc(const Assoc& orig)
{
}

Assoc::~Assoc()
{    
}

int Assoc::Process()
{
    //读取文件
    int ret = ReadFiles();
    if(ret < 0)
    {
        LOG_ERROR("[Assoc]读取文件集失败%s", m_pFiles->ScenceFile.file);
        return ret;
    }
    //场景关联
    AssocScence();
    //输出文件
    OutPutFiles();
    //备份
    Backup();
    return 0;
}
void Assoc::Backup()
{
    bfr.Backup();
}
//输出文件

int Assoc::OutPutFiles()
{
    bfr.WriteFiles();
    return 0;
}
//场景关联回填

int Assoc::AssocScence()
{
    //初始化排序数组
    InitPofXDR();
    //排序
    SortP();
    //回填
    GetScenceIntoMR();

    return 0;
}
//进行回填

void Assoc::PScence2PMR(st_XDR_INFO* pReferenceScence, st_RC_XDR_INFO* p)
{
    p->rc_additional_part.cCoveringChildProperties = pReferenceScence->additional_part.cCoveringChildProperties;
    p->rc_additional_part.cCoveringProperties = pReferenceScence->additional_part.cCoveringProperties;
    p->rc_additional_part.cScenceProperties = pReferenceScence->additional_part.cScenceProperties;
    p->rc_additional_part.cVendorId = pReferenceScence->additional_part.cVendorId;
    p->rc_additional_part.dLatitude = pReferenceScence->additional_part.dLatitude;
    p->rc_additional_part.dLongitude = pReferenceScence->additional_part.dLongitude;
    memcpy(p->rc_additional_part.sGridID,
           pReferenceScence->additional_part.sGridID,
           sizeof (pReferenceScence->additional_part.sGridID));
}

/*
 * findNextRef：向下找本imsi的基准场景xdr
 * i :当前指针
 * pReferenceScence基准指针
 */
void Assoc::findNextRef(unsigned int& i, st_XDR_INFO* &pReferenceScence)
{
    st_RC_XDR_INFO* p = (st_RC_XDR_INFO*) m_p[i];
    if (i >= m_xdrsize - 1)return ; //当前就是最后一条，直接跳出
    bool ifChangeReferenceScence = false; //用来记录是否找到基准点
    unsigned int j = i + 1; //从当前记录下一条开始找，由于判断过i不是最后一条，所以不会越界
    for (; j < m_xdrsize; ++j)
    {
        if (((st_XDR_INFO*) m_p[j])->public_part.llImsi != p->public_part.llImsi)
        {//找到下一个用户了，证明本用户没有基准点了
            break;
        }
        if (((st_XDR_INFO*) m_p[j])->public_part.cInterface == XDR_MME_TAG
                || ((st_XDR_INFO*) m_p[j])->public_part.cInterface == XDR_HTTP_TAG)
        {//找到了基准点，更新
            pReferenceScence = (st_XDR_INFO*) m_p[j];
            ifChangeReferenceScence = true;
            break;
        }
    }//end for 找下边的基准点
    if (!ifChangeReferenceScence)
    {//没找到基准点，跳过本用户
        i = j - 1;
    }
    else if (pReferenceScence->additional_part.nStartTime - p->rc_additional_part.nStartTime < TIME_DELAY)
    {//若找到了基准点，则判一下本条
        PScence2PMR(pReferenceScence, p);
    }//end if 找到基准点判本条
}

void Assoc::GetScenceIntoMR()
{
    st_XDR_INFO* pReferenceScence = NULL;
    for (unsigned int i = 0; i < m_xdrsize; ++i)
    {//初始化基准点
        if (((st_XDR_INFO*) m_p[i])->public_part.cInterface == XDR_MME_TAG
                || ((st_XDR_INFO*) m_p[i])->public_part.cInterface == XDR_HTTP_TAG)
        {
            pReferenceScence = (st_XDR_INFO*) m_p[i];
            break;
        }
    }
    //如果整个队列遍历结束都没有基准点，则不回填(这里实际上不会发生，因为场景文件不会为空)
    if (pReferenceScence == NULL)return;
    
    for (unsigned int i = 0; i < m_xdrsize; ++i)
    {
        if (((st_XDR_INFO*) m_p[i])->public_part.cInterface == XDR_MME_TAG
                || ((st_XDR_INFO*) m_p[i])->public_part.cInterface == XDR_HTTP_TAG)
        {
            pReferenceScence = (st_XDR_INFO*) m_p[i];
        }
        else
        {
            st_RC_XDR_INFO* p = (st_RC_XDR_INFO*) m_p[i];
            if (p->public_part.llImsi == pReferenceScence->public_part.llImsi)
            {//imsi相同
                if (p->rc_additional_part.nStartTime >= pReferenceScence->additional_part.nStartTime)
                {//当前mr数据在基准点后边：符合要求就填，不符合就向下找基准点
                    if (p->rc_additional_part.nStartTime - pReferenceScence->additional_part.nStartTime < TIME_DELAY)
                    {
                        PScence2PMR(pReferenceScence, p);
                    }
                    else
                    {//不符合要求：与前边基准点的时间差过大      ,则向下找基准点                  
                        findNextRef(i, pReferenceScence);
                    }//end if ：与前边基准点过远
                }//end if:基准点在前边
                else
                {//当前mr数据在基准点前边：符合要求就填，不符合就填不上，循环继续，处理下一跳
                    if (pReferenceScence->additional_part.nStartTime - p->rc_additional_part.nStartTime < TIME_DELAY)
                    {
                        PScence2PMR(pReferenceScence, p);
                    }
                }//end if:基准点在后边
            }//本mr数据imsi与基准点相同
            else
            {
                findNextRef(i, pReferenceScence);
            }//end if imsi不同
        }//endif 本条是软采
    }//end for
}


inline bool mycmp(void* a, void * b)
{
    st_XDR_INFO* x = (st_XDR_INFO*) a;
    st_XDR_INFO* y = (st_XDR_INFO*) b;
    unsigned long long llpImsi;
    unsigned long long llpStartTime;
    unsigned long long llqImsi;
    unsigned long long llqStartTime;
    if (x->public_part.cInterface == XDR_MME_TAG || x->public_part.cInterface == XDR_HTTP_TAG)
    {
        llpImsi = x->public_part.llImsi;
        llpStartTime = x->additional_part.nStartTime;
    }
    else
    {
        st_RC_XDR_INFO* p = (st_RC_XDR_INFO*) a;
        llpImsi = p->public_part.llImsi;
        llpStartTime = p->rc_additional_part.nStartTime;
    }
    if (y->public_part.cInterface == XDR_MME_TAG || y->public_part.cInterface == XDR_HTTP_TAG)
    {
        llqImsi = y->public_part.llImsi;
        llqStartTime = y->additional_part.nStartTime;
    }
    else
    {
        st_RC_XDR_INFO* q = (st_RC_XDR_INFO*) b;
        llqImsi = q->public_part.llImsi;
        llqStartTime = q->rc_additional_part.nStartTime;
    }
    if (llpImsi < llqImsi)return true;
    else if (llpImsi == llqImsi
            && llpStartTime < llqStartTime)return true;
    return false;

}

//排序

void Assoc::SortP()
{
    std::sort(m_p, m_p + m_xdrsize, mycmp);
}
//排序指针初始化

int Assoc::InitPofXDR()
{
    m_xdrsize = m_pFiles->ScenceFile.num;
    list<FAndP>::iterator it_lst = m_pFiles->MRFiles.begin();
    list<FAndP>::iterator it_lst_end = m_pFiles->MRFiles.end();
    for (; it_lst != it_lst_end; ++it_lst)
    {
        m_xdrsize += it_lst->num;
    }
    m_p = new(std::nothrow) void*[m_xdrsize];
    short retry = 3;
    while (m_p == NULL && retry--)
    {
        sleep(1);
        m_p = new(std::nothrow) void*[m_xdrsize];
    }
    if (m_p == NULL && retry == 0)
    {
        LOG_ERROR("[Assoc]排序指针内存申请错误,大小%u[日期%s,组号%s]", m_xdrsize , m_pFiles->day, m_pFiles->imsiGroup);
        return -1;
    }
    unsigned int pIndex = 0;
    for (unsigned int i = 0; i < m_pFiles->ScenceFile.num; ++i, ++pIndex)
    {
        m_p[pIndex] = (st_XDR_INFO*) (m_pFiles->ScenceFile.p) + i;
    }
    for (it_lst = m_pFiles->MRFiles.begin(); it_lst != it_lst_end; ++it_lst)
    {
        for (unsigned int i = 0; i < it_lst->num; ++i, ++pIndex)
        {
            m_p[pIndex] = ((st_RC_XDR_INFO*) (m_pFiles->ScenceFile.p) + i);
        }
    }

    return 0;
}
//将文件集读入内存

int Assoc::ReadFiles()
{
    int ret = bfr.ReadFiles();
    if (ret < 0)
    {        
        LOG_ERROR("[Assoc]文件集读取失败[日期%s,组号%s]", m_pFiles->day, m_pFiles->imsiGroup);
        return -1;
    }
    return 0;
}

int Assoc::SetFiles(AssocFiles& files)
{
    this->m_pFiles = &files;

    if (m_p != NULL)delete[] m_p;
    m_p = NULL;

    bfr.Init(this->m_pFiles);
    return 0;
}