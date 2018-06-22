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

void DeviceManager::setProperty(std::string did, std::string pro, std::string val)
{
    LOGD("(%s, %s, %s)\n", did.c_str(), pro.c_str(), val.c_str());
}

CloudManager& cloudManager()
{
    static CloudManager sCM;
    return sCM;
}

void tempSimulateTest(Message *msg)
{
    static int sCount = 7;
    static int sMagic = 0;

    switch (sMagic++ % sCount) {
        case 1:
            deviceManager().mProfileCB("Light2", "json-doc");
            break;
        case 2:
            cloudManager().mRuleSyncCB("light-hot-to-off");
            break;
        case 3:
            deviceManager().mStateCB("000001", "Light2", 1 /* online */);
            deviceManager().mStateCB("000002", "Light2", 1 /* online */);
            break;
        case 4:
            deviceManager().mStateCB("000002", "Light2", 2 /* offline */);
            break;
        case 5: /* property change */
            deviceManager().mPropertyCB("000001", "temprature", "85.0");
            break;
        case 6:
            return;
    }
    mainHandler().sendEmptyMessageDelayed(msg->what, 1000);
}

} /* namespace HB */
