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


#ifdef	__cplusplus
}
#endif

#endif	/* ASSOCFILESSTRUCT_H */

