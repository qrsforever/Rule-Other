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

void tempSimulateTest(Message *msg);

} /* namespace HB */

#endif /* __cplusplus */

#endif /* __TempSimulateSuite_H__ */

/* {
 *     "ruleName":"example",
 *         "ruleId":"
 *             -
 *             ",
 *         "description":"this is a example for ru
 *             le definition",
 *         "trigger":{
 *             "triggerType":"auto|manual",
 *             "switch":{
 *                 "enabled":"on",
 *                 "timeCondition":"on",
 *                 "deviceCondition ":"on",
 *                 "notify":"on",
 *                 "manual":"on"
 *             }
 *         },
 *         "conditions":{
 *             "conditionLogic":"and",
 *             "timeCondition":[
 *             {
 *                 "year":"2018",
 *                 "mouth":"10",
 *                 "day":"10|13|17",
 *                 "hour":"every",
 *                 "minute":"0",
 *                 "second":"0"
 *             }
 *             ],
 *             "deviceCondition":{
 *                 "deviceLogic":"or",
 *                 "deviceStatus":[
 *                 {
 *                     "deviceId":"0007A895C8A7",
 *                     "propName":"CurrentTemperature",
 *                     "propValue":"v>50"
 *                 },
 *                 {
 *                     "deviceId":"DC330D799327",
 *                     "propName":"onOffLight",
 *                     "propValue":"v==true"
 *                 }
 *                 ]
 *             }
 *         },
 *         "actions":{
 *             "notify":{
 *                 "tellYou":"Girlfriend Birthday!"
 *             },
 *             "deviceControl":[
 *             {
 *                 "deviceId":"0007A895C7C7",
 *                 "propName":"CurrentTemperature",
 *                 "propValue":"50"
 *             },
 *             {
 *                 "deviceId":"DC330D79932A",
 *                 "propName":"onOffLight",
 *                 "propValue":"true"
 *             }
 *             ],
 *             "manualRuleId":[
 *                 "100",
 *                 "101"
 *             ]
 *         }
 * } */

