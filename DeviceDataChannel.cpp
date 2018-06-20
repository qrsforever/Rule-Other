/***************************************************************************
 *  DeviceDataChannel.cpp - Device Data Channel Impl
 *
 *  Created: 2018-06-14 15:05:46
 *
 *  Copyright QRS
 ****************************************************************************/

#include "DeviceDataChannel.h"
#include "RuleEventHandler.h"
#include "RuleEventTypes.h"
#include "StringArray.h"
#include "Log.h"

#include "TempSimulateSuite.h" /* TODO only test */

using namespace UTILS;

namespace HB {

DeviceDataChannel::DeviceDataChannel()
    : mDeviceMgr(deviceManager())
    , mH(ruleHandler())
{

}

DeviceDataChannel::~DeviceDataChannel()
{

}

int DeviceDataChannel::init()
{
    LOGTT();

    /* regist device state changed callback */
    mDeviceMgr.registDeviceStateChangedCallback(
        std::bind(
            &DeviceDataChannel::onDeviceStateChanged,
            this,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3)
        );

    /* regist device property changed callback */
    mDeviceMgr.registDevicePropertyChangedCallback(
        std::bind(
            &DeviceDataChannel::onDevicePropertyChanged,
            this,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3)
        );
    return 0;
}

void DeviceDataChannel::onDeviceStateChanged(std::string did, std::string devName, int state)
{
    LOGTT();

    switch (state) {
        case 1:
            { /* online */
                std::string insName = did; /* std::string(ID_PREFIX) + did; */
                /* TODO dangerous using, let's try */
                std::shared_ptr<InstancePayload> payload = std::make_shared<InstancePayload>();
                payload->mInsName = insName;
                payload->mClsName = devName;
                mH.sendMessage(mH.obtainMessage(RET_INSTANCE_ADD, payload));
            }
            break;
        case 2: /* offline */
            {
                std::string insName = did; /* std::string(ID_PREFIX) + did; */
                std::shared_ptr<InstancePayload> payload = std::make_shared<InstancePayload>();
                payload->mInsName = insName;
                mH.sendMessage(mH.obtainMessage(RET_INSTANCE_DEL, payload));
            }
            break;
    }
}

void DeviceDataChannel::onDevicePropertyChanged(std::string did, std::string proKey, std::string proVal)
{
    LOGTT();

    std::string insName = did; /* std::string(ID_PREFIX) + did; */
    std::shared_ptr<InstancePayload> payload = std::make_shared<InstancePayload>();
    payload->mInsName = insName;
    payload->mSlots.push_back(InstancePayload::SlotInfo(proKey, proVal));
    mH.sendMessage(mH.obtainMessage(RET_INSTANCE_PUT, payload));
}

bool DeviceDataChannel::send(std::string key, int action, std::shared_ptr<DataPayload> payload)
{
    return false;
}

} /* namespace HB */
