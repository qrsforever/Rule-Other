/***************************************************************************
 *  HomeBrainMain.cpp - HomeBrainMain
 *
 *  Created: 2018-06-13 15:25:27
 *
 *  Copyright
 ****************************************************************************/

#include "Log.h"
#include "LogThread.h"
#include "MainPublicHandler.h"

using namespace HB;

extern "C" int mainThreadRun();

int main(int argc, char *argv[])
{
    /* TODO see rule-engine/RuleDriver/src/UnitTest.cpp */

    initLogThread();
    setLogLevel(LOG_LEVEL_INFO);

    return mainThreadRun();
}
