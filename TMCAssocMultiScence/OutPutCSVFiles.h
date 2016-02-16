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
#include "AssocFiles.h"
#include <queue>

class OutPutCSVFiles {
public:
    OutPutCSVFiles();
    virtual ~OutPutCSVFiles();
    int OutputCsv(AssocFiles* p, OutStatistics& ossDay);
    void SetSOutputDir(std::string sOutputDir);
    std::string GetSOutputDir();
protected:
    OutPutCSVFiles(const OutPutCSVFiles& orig);
private:
    std::string m_sOutputDir;
};

#endif	/* OUTPUTCSVFILES_H */

