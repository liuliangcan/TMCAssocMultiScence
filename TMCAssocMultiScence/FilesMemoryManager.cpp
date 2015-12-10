/* 
 * File:   FilesMemoryManager.cpp
 * Author: LIU
 * 
 * Created on 2015年12月10日, 上午10:08
 */

#include "FilesMemoryManager.h"
#include "tools/log.h"
#include "GlobalConfiger.h"
#include <unistd.h>
using namespace std;
FilesMemoryManager::FilesMemoryManager()
{    
    list<std::string> strList;
    GlobalConfiger::GetInstance()->GetListMRInputDir(strList);
    m_iBoundSize = strList.size() * 24 + 3;
}

FilesMemoryManager::FilesMemoryManager(const FilesMemoryManager& orig)
{
}

FilesMemoryManager::~FilesMemoryManager()
{
    while(m_list.size() != 0)
        {
            if(m_list.front().p != NULL) delete[]m_list.front().p;
            m_list.pop_front();
        }
}
int FilesMemoryManager::FindInList(void **p, unsigned long long size)
{
    list<st>::iterator it_list = m_list.begin();
    list<st>::iterator it_list_end = m_list.end();
    for(;it_list != it_list_end;++it_list)
    {
        if(!it_list->inUse && it_list->size >= size)
        {
            *p = it_list->p;
            it_list->inUse = true;
            return 0;
        }
    }    
    return -1;
}
int FilesMemoryManager::Apply(void **p, unsigned long long size)
{
    int ret = FindInList(p, size);
    //从现有中申请到可用的可直接返回成功
    if(ret >= 0) return 0;
    //如果没申请到 则new内存
    st tmp;
    tmp.p = new(std::nothrow) unsigned char[size];
    short retry = 3;
    while(tmp.p == NULL && retry--)
    {        
        sleep(1);
        tmp.p = new(std::nothrow) unsigned char[size];
    }
    if(tmp.p == NULL)
    {
        LOG_ERROR("[FilesMemoryManager]内存申请错误[%lld byte]", size);
        return -1;
    }
    tmp.size = size;
    tmp.inUse = true;
    *p = tmp.p;
    Insert2List(tmp);
    return 0;
}
int FilesMemoryManager::Insert2List(const st& tmp)
{
    list<st>::iterator it_list = m_list.begin();
    list<st>::iterator it_list_end = m_list.end();
    for(;it_list != it_list_end;++it_list)
    {
        if(tmp.size <= it_list->size)
        {
            m_list.insert(it_list, tmp);
            return 0;
        }
    }
    m_list.push_back(tmp);
    return 0;
}
//内存部分回收操作：最小和最大的内存delete
int FilesMemoryManager::Collect()
{           
    if(m_list.size() > m_iBoundSize * 1.5)
    {
        while(m_list.size() != 0)
        {
            if(m_list.front().p != NULL) delete[]m_list.front().p;
            m_list.pop_front();
        }
        return 0;
    }
    if(m_list.size() > m_iBoundSize)
    {
        if(m_list.front().p != NULL) delete[]m_list.front().p;
        m_list.pop_front();
        unsigned long long backSize = m_list.back().size;
        while(m_list.size() != 0)
        {
            if(m_list.size() < (m_iBoundSize>>1))break;
            if(m_list.back().size < (backSize>>1))break;
            if(m_list.back().p != NULL) delete[]m_list.back().p;
            m_list.pop_back();
        }
    }
    list<st>::iterator it_list = m_list.begin();
    list<st>::iterator it_list_end = m_list.end();
    for(;it_list != it_list_end;++it_list)
    {
        it_list->inUse = false;
    }
    return 0;
}