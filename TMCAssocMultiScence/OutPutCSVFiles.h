/* 
 * File:   OutPutCSVFiles.h
 * Author: LIU
 * describe:输出csv文件
 * Created on 2015年12月9日, 下午5:35
 */

#ifndef OUTPUTCSVFILES_H
#define	OUTPUTCSVFILES_H

#include <string>
#include "DataDefine.h"
#include <queue>
#include "AssocFiles.h"

class OutPutCSVFiles {
public:


    int OutputCsv(AssocFiles* p);



    OutPutCSVFiles();
    OutPutCSVFiles(const OutPutCSVFiles& orig);
    virtual ~OutPutCSVFiles();
    void SetSOutputDir(std::string sOutputDir);
    std::string GetSOutputDir() const;
private:

    std::string m_sOutputDir;
};

#endif	/* OUTPUTCSVFILES_H */

