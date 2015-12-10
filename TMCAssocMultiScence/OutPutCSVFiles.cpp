/* 
 * File:   OutPutCSVFiles.cpp
 * Author: LIU
 * 
 * Created on 2015年12月9日, 下午5:35
 */

#include "OutPutCSVFiles.h"

OutPutCSVFiles::OutPutCSVFiles()
{
}

OutPutCSVFiles::OutPutCSVFiles(const OutPutCSVFiles& orig)
{
}

OutPutCSVFiles::~OutPutCSVFiles()
{
}

void OutPutCSVFiles::SetSOutputDir(std::string sOutputDir)
{
    m_sOutputDir = sOutputDir;
}

std::string OutPutCSVFiles::GetSOutputDir() const
{
    return m_sOutputDir;
}

int OutPutCSVFiles::OutputCsv(AssocFiles* p)
{
    return 0;
}

