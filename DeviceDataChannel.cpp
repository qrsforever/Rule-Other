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
    : mDeviceManger(deviceManager())
    , mH(ruleHandler())
{

}

DeviceDataChannel::~DeviceDataChannel()
{

}

void DeviceDataChannel::init()
{
    LOGTT();

    /* regist device state changed callback */
    mDeviceManger.registDeviceStateChangedCallback(
        std::bind(
            &DeviceDataChannel::onDeviceStateChanged,
            this,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3)
        );

    /* regist device property changed callback */
    mDeviceManger.registDevicePropertyChangedCallback(
        std::bind(
            &DeviceDataChannel::onDevicePropertyChanged,
            this,
            std::placeholders::_1,
            std::placeholders::_2,
            std::placeholders::_3)
        );
}

void DeviceDataChannel::onDeviceStateChanged(std::string did, std::string devName, int state)
{
    LOGTT();

    switch (state) {
        case 1:
            { /* online */
                std::string insName = std::string(ID_PREFIX) + did;
                std::shared_ptr<StringArray> data = std::make_shared<StringArray>();
                data->put(0, insName.c_str());
                data->put(1, devName.c_str());
                mH.sendMessage(mH.obtainMessage(RET_INSTANCE_ADD, data));
            }
            break;
        case 2: /* offline */
            {
                std::string insName = std::string(ID_PREFIX) + did;
                std::shared_ptr<StringArray> data = std::make_shared<StringArray>();
                data->put(0, insName.c_str());
                mH.sendMessage(mH.obtainMessage(RET_INSTANCE_DEL, data));
            }
            break;
    }
}

void DeviceDataChannel::onDevicePropertyChanged(std::string did, std::string proKey, std::string proVal)
{
    LOGTT();

    std::string insName = std::string(ID_PREFIX) + did;
    std::shared_ptr<StringArray> data = std::make_shared<StringArray>();
    data->put(0, insName.c_str());
    data->put(1, proKey.c_str());
    data->put(2, proVal.c_str());
    mH.sendMessage(mH.obtainMessage(RET_INSTANCE_PUT, data));
}

bool DeviceDataChannel::send(std::string key, int action, std::shared_ptr<DataPayload> payload)
{
    return false;
}

} /* namespace HB */
