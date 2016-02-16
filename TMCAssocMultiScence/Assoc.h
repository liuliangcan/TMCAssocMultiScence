/* 
 * File:   Assoc.h
 * Author: LIU
 *
 * Created on 2015年12月3日, 下午4:03
 */

#ifndef ASSOC_H
#define	ASSOC_H

#include "AssocFiles.h"
#include "BinFilesReader.h"
#include "DataDefine.h"
#include "OutPutCSVFiles.h"
#include "GlobalConfiger.h"

class Assoc {
public:

    int Init()
    {        
        return 0;
    }

    //处理
    int Process(OutStatistics& ossDay);

    //设置文件集
    int SetFiles(AssocFiles& files);


    Assoc();
    Assoc(const Assoc& orig);
    virtual ~Assoc();
private:
    //将文件集读入内存
    int ReadFiles();
    //场景关联回填
    int AssocScence();
    void PScence2PMR(st_XDR_INFO* pReferenceScence, st_RC_XDR_INFO* p);
    void findNextRef(unsigned int& i, st_XDR_INFO* &pReferenceScence);
    //排序指针初始化
    int InitPofXDR();
    //排序
    void SortP();
    //进行回填
    void GetScenceIntoMR();
    //输出文件
    int OutPutFiles(OutStatistics& ossDay);

    //备份文件
    void Backup();
    AssocFiles* m_pFiles;
    //二进制文件读取类
    BinFilesReader bfr;
    //csv文件输出类
    
    //排序指针
    void ** m_p;
    unsigned int m_xdrsize;
};

#endif	/* ASSOC_H */

