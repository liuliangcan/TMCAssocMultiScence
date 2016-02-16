/* 
 * File:   BinFilesReader.h
 * Author: LIU
 * des:二进制文件处理文件
 * Created on 2015年12月7日, 上午10:38
 */

#ifndef BINFILESREADER_H
#define	BINFILESREADER_H

#include "AssocFiles.h"
#include <string>
#include "FilesMemoryManager.h"
#include "OutPutCSVFiles.h"

class BinFilesReader {
public:
    
    int Init(AssocFiles* pFiles)
    {
        if(pFiles == NULL)return -1;
        this->m_pFiles = pFiles;
        return 0;
    }
    //读文件
    int ReadFiles();    
    //写文件
    int WriteFiles(OutStatistics& ossDay);
    //备份
    int Backup();
    
    BinFilesReader();
    BinFilesReader(const BinFilesReader& orig);
    virtual ~BinFilesReader();
private:
    //读取单个文件
    int ReadOneFile(FAndP* fap, int flag);
    //写单个文件
    int WriteOneFile(FAndP* fap, int flag);
    //写MR文件
    int WriteMRFile(FAndP* fap);
    //场景文件只做rename
    int MoveScenceFile(FAndP* fap);
    
    int BackupScenceFile();
    int BackupMRFile(FAndP* fap);
    //文件集指针
    AssocFiles* m_pFiles;
    OutPutCSVFiles opcsv;
    FilesMemoryManager fmm;
};

#endif	/* BINFILESREADER_H */

