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
#include "InstancePayload.h"
#include "ClassPayload.h"
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
            &DeviceDataChannel::onStateChanged,
            this,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3)
        );

    /* regist device property changed callback */
    mDeviceMgr.registDevicePropertyChangedCallback(
        std::bind(
            &DeviceDataChannel::onPropertyChanged,
            this,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3)
        );

    /* regist device profile sync callback */
    mDeviceMgr.registDeviceProfileSyncCallback(
        std::bind(
            &DeviceDataChannel::onProfileSync,
            this,
            std::placeholders::_1,
            std::placeholders::_2)
        );
    return 0;
}

void DeviceDataChannel::onStateChanged(std::string did, std::string devName, int state)
{
    LOGTT();

    switch (state) {
        case 1:
            { /* online */
                std::shared_ptr<InstancePayload> payload = std::make_shared<InstancePayload>();
                payload->mInsName = innerOfInsname(did);
                payload->mClsName = devName;
                mH.sendMessage(mH.obtainMessage(RET_INSTANCE_ADD, payload));
            }
            break;
        case 2: /* offline */
            {
                std::shared_ptr<InstancePayload> payload = std::make_shared<InstancePayload>();
                payload->mInsName = innerOfInsname(did);
                mH.sendMessage(mH.obtainMessage(RET_INSTANCE_DEL, payload));
            }
            break;
    }
}

void DeviceDataChannel::onPropertyChanged(std::string did, std::string proKey, std::string proVal)
{
    LOGTT();

    std::shared_ptr<InstancePayload> payload = std::make_shared<InstancePayload>();
    payload->mInsName = innerOfInsname(did);
    payload->mSlots.push_back(InstancePayload::SlotInfo(proKey, proVal));
    mH.sendMessage(mH.obtainMessage(RET_INSTANCE_PUT, payload));
}

void DeviceDataChannel::onProfileSync(std::string devName, std::string doc)
{
    LOGTT();
    std::shared_ptr<ClassPayload> payload = std::make_shared<ClassPayload>(devName, "DEVICE", "1.0.0");
    payload->makeSlot(ST_INTEGER, "switch", "0 1", false);
    payload->makeSlot(ST_FLOAT, "temprature", "-15.0", "95.0", false);
    payload->makeSlot(ST_STRING, "color", "\"red\" \"green\" \"blue\"", false);

    mH.sendMessage(mH.obtainMessage(RET_CLASS_SYNC, payload));
}

bool DeviceDataChannel::send(int action, std::shared_ptr<Payload> data)
{
    LOGTT();
    if (action == PT_INSTANCE_PAYLOAD) {
        std::shared_ptr<InstancePayload> payload(std::dynamic_pointer_cast<InstancePayload>(data));
        mDeviceMgr.setProperty(
            outerOfInsname(payload->mInsName),
            payload->mSlots[0].nName,
            payload->mSlots[0].nValue);
    }
    return true;
}

} /* namespace HB */
