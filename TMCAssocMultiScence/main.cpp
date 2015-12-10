/* 
 * File:   main.cpp
 * Author: LIU
 *
 * Created on 2015年12月1日, 下午12:23
 */
#include <cstdlib>
#include "ThreadManager.h"
#include "tools/log.h"
#include "GlobalConfiger.h"

using namespace std;

int f_main(int argc, char** argv)
{
    char configXML[512] = "./AssocMultiScenceConf.xml";
    if (argc > 1)
    {
        strcpy(configXML, argv[1]);
    }
    GlobalConfiger *gc = GlobalConfiger::GetInstance();
    if (gc->Init(configXML) < 0)
        return -1;

    log_init(LL_NOTICE, "AssocMultiScence", gc->GetLogDir().c_str());

    ThreadManager *tm = ThreadManager::GetInstance();
    tm->Init();
    tm->Start();

    bool flagRun = true;
    while (flagRun)
    {
        sleep(100000);
    }


    //    log_init(LL_TRACE, "mysql", "./log/");
    //    LOG_NOTICE("%s [time:%d]", "test calling log", time(NULL));
    //    LOG_DEBUG("debug msg,only write to log when the loglevel bigger than or equal to debug [time:%d]", time(NULL));
    //    LOG_WARN("warnning msg will be writing to the error files [time:%d]", time(NULL));
    //    LOG_ERROR("you also can change  number of output files by rewrite the macro_define.h");
    return 0;
}

/*
 * 
 */
int main(int argc, char** argv)
{
    f_main(argc, argv);
    return 0;
}


