/***************************************************************************
 *  TempSimulateSuit.cpp - Only for test
 *
 *  Created: 2018-06-15 19:52:41
 *
 *  Copyright QRS
 ****************************************************************************/

#include "TempSimulateSuite.h"
#include "MainPublicHandler.h"
#include "Log.h"

namespace HB {

DeviceManager& deviceManager()
{
    static DeviceManager sDM;
    return sDM;
}

CloudManager& cloudManager()
{
    static CloudManager sCM;
    return sCM;
}

void tempSimulateTest(Message *msg)
{
    static int sCount = 5;
    static int sMagic = 0;

    switch (sMagic++ % sCount) {
        case 1:
            cloudManager().mSyncCB("test");
            return;
            // deviceManager().mStateCB("000001", "Light", 1 [> online <]);
            break;
        case 2:
            deviceManager().mStateCB("000001", "Light", 2 /* offline */);
            break;
        case 3: /* property change */
            deviceManager().mPropertyCB("000001", "switch", "1");
            break;
        case 4:
            cloudManager().mSyncCB("test");
            break;
    }
    mainHandler().sendEmptyMessageDelayed(msg->what, 1000);
}

} /* namespace HB */
