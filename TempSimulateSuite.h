/***************************************************************************
 *  TempSimulateSuite.h - Only Test
 *
 *  Created: 2018-06-15 19:52:12
 *
 *  Copyright QRS
 ****************************************************************************/

#ifndef __TempSimulateSuite_H__
#define __TempSimulateSuite_H__

#include <string>
#include <functional>
#include "Message.h"


#ifdef __cplusplus

using namespace UTILS;

namespace HB {

class DeviceManager {
public:
    DeviceManager() {}
    ~DeviceManager(){}

    typedef std::function<void(std::string, std::string, int)> DeviceStateChangedCallback;
    typedef std::function<void(std::string, std::string, std::string)> DevicePropertyChangedCallback;

    void registDeviceStateChangedCallback(DeviceStateChangedCallback cb) {
        mStateCB = cb;
    }

    void registDevicePropertyChangedCallback(DevicePropertyChangedCallback cb) {
        mPropertyCB = cb;
    }

public:
    DeviceStateChangedCallback mStateCB;
    DevicePropertyChangedCallback mPropertyCB;
};

DeviceManager& deviceManager();

class CloudManager {
public:
    CloudManager() {}
    ~CloudManager() {}

    typedef std::function<void(std::string)> RuleSyncCallback;

    void registRuleSyncCallback(RuleSyncCallback cb) {
        mSyncCB = cb;
    }
public:
    RuleSyncCallback mSyncCB;
};

CloudManager& cloudManager();

void tempSimulateTest(Message *msg);

} /* namespace HB */

#endif /* __cplusplus */

#endif /* __TempSimulateSuite_H__ */
