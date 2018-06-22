/***************************************************************************
 *  HomeBrainMain.cpp - HomeBrainMain
 *
 *  Created: 2018-06-13 15:25:27
 *
 *  Copyright
 ****************************************************************************/

#include "Log.h"
#include "LogThread.h"
#include "Message.h"
#include "MessageTypes.h"
#include "RuleEngineService.h"
#include "RuleEventHandler.h"
#include "DeviceDataChannel.h"
#include "RuleDataChannel.h"

#include "MainPublicHandler.h" /* temp use, not rule module */

using namespace HB;

extern "C" int initMainThread();
extern "C" int mainThreadRun();

class InitThread : public Thread {
public:
    InitThread(){}
    ~InitThread(){}
    void run();
};

void InitThread::run()
{
    /*************************
     *  Network module init  *
     *************************/


    /*******************************
     *  Device Manger module init  *
     *******************************/


    /*****************************
     *  Rule Engine module init  *
     *****************************/
    ruleEngine().setServerRoot("./RuleDriver/clips");
    ruleEngine().setDeviceChannel(std::make_shared<DeviceDataChannel>());
    ruleEngine().setRuleChannel(std::make_shared<ELinkRuleDataChannel>());
    ruleEngine().init();


    /* Simulate Test */
    mainHandler().sendEmptyMessage(MT_SIMULATE);
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;
    printf("\n-----------Test---------\n");

    /***************************
     *  First init log module  *
     ***************************/
    initLogThread();
    setLogLevel(LOG_LEVEL_INFO);

    /*****************************
     *  Second init main module  *
     *****************************/
    initMainThread();

    /*******************************
     *  Third init others modules  *
     *******************************/
    InitThread init;
    init.start();


    /****************************
    *  Main message queue run  *
    ****************************/
    return mainThreadRun();
}
