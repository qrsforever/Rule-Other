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
    typedef std::function<void(std::string, std::string)> DeviceProfileSyncCallback;

    void registDeviceStateChangedCallback(DeviceStateChangedCallback cb) {
        mStateCB = cb;
    }

    void registDevicePropertyChangedCallback(DevicePropertyChangedCallback cb) {
        mPropertyCB = cb;
    }

    void registDeviceProfileSyncCallback(DeviceProfileSyncCallback cb) {
        mProfileCB = cb;
    }

    void setProperty(std::string did, std::string pro, std::string val);

public:
    DeviceStateChangedCallback mStateCB;
    DevicePropertyChangedCallback mPropertyCB;
    DeviceProfileSyncCallback mProfileCB;
};

DeviceManager& deviceManager();

class CloudManager {
public:
    CloudManager() {}
    ~CloudManager() {}

    typedef std::function<void(std::string)> RuleSyncCallback;

    void registRuleSyncCallback(RuleSyncCallback cb) {
        mRuleSyncCB = cb;
    }
public:
    RuleSyncCallback mRuleSyncCB;
};

CloudManager& cloudManager();

std::string getClassByDeviceId(const std::string &deviceId);

void tempSimulateTest(Message *msg);

} /* namespace HB */

#endif /* __cplusplus */

#endif /* __TempSimulateSuite_H__ */
