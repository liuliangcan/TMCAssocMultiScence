/* 
 * File:   FilesMemoryManager.h
 * Author: LIU
 *
 * Created on 2015年12月10日, 上午10:08
 */

#ifndef FILESMEMORYMANAGER_H
#define	FILESMEMORYMANAGER_H
#include <stddef.h>
#include <list>
class FilesMemoryManager {
public:
    
    
    
    int Apply(void **p, unsigned long long size);
    int Collect();
    FilesMemoryManager();
    FilesMemoryManager(const FilesMemoryManager& orig);
    virtual ~FilesMemoryManager();
private:
class st
    {
    public:
        st()
        {
            p = NULL;
            size = 0;
            inUse = false;
        }
        void *p;
        unsigned long long size;
        bool inUse;        
    };
    //list允许的最大长度，如果检测到超过这个长度则剪掉首尾元素
    int m_iBoundSize;
    std::list<st> m_list;
    int FindInList(void **p, unsigned long long size);
    int Insert2List(const st& tmp);
    
};

#endif	/* FILESMEMORYMANAGER_H */

