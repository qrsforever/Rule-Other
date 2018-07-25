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
#include "Common.h"
#include "Log.h"
#include <map>

#ifndef SIM_SUITE
extern "C" HBDeviceManager& deviceManager();
#endif

using namespace UTILS;

namespace HB {

DeviceDataChannel::DeviceDataChannel()
    : mH(ruleHandler())
{
}

DeviceDataChannel::~DeviceDataChannel()
{

}

int DeviceDataChannel::init()
{
    return 0;
}

void DeviceDataChannel::onDeviceStatusChanged(const std::string deviceId, const std::string deviceName, HBDeviceStatus status)
{
    switch (status) {
        case HB_DEVICE_STATUS_ONLINE:
            {
                std::shared_ptr<InstancePayload> payload = std::make_shared<InstancePayload>();
                payload->mInsName = innerOfInsname(deviceId);
                payload->mClsName = deviceName;
                mH.sendMessage(mH.obtainMessage(RET_INSTANCE_ADD, payload));
            }
            break;
        case HB_DEVICE_STATUS_OFFLINE:
            {
                std::shared_ptr<InstancePayload> payload = std::make_shared<InstancePayload>();
                payload->mInsName = innerOfInsname(deviceId);
                mH.sendMessage(mH.obtainMessage(RET_INSTANCE_DEL, payload));
            }
            break;
        case HB_DEVICE_STATUS_UNINITIALIZED:
        case HB_DEVICE_STATUS_INITIALIZING:
        default:
            break;
    }
}

void DeviceDataChannel::onDevicePropertyChanged(const std::string deviceId, const std::string proKey, std::string proVal)
{
    if (proKey == "n")
        return;
    std::shared_ptr<InstancePayload> payload = std::make_shared<InstancePayload>();
    payload->mInsName = innerOfInsname(deviceId);
    payload->mSlots.push_back(InstancePayload::SlotInfo(proKey, proVal));
    mH.sendMessage(mH.obtainMessage(RET_INSTANCE_PUT, payload));
}

bool DeviceDataChannel::send(int action, std::shared_ptr<Payload> data)
{
    LOGTT();
    if (action == PT_INSTANCE_PAYLOAD) {
        std::shared_ptr<InstancePayload> payload(std::dynamic_pointer_cast<InstancePayload>(data));
        deviceManager().SetDevicePropertyValue(
            outerOfInsname(payload->mInsName),
            payload->mSlots[0].nName,
            payload->mSlots[0].nValue, true);
    }
    return true;
}

ElinkDeviceDataChannel::ElinkDeviceDataChannel()
{
}

ElinkDeviceDataChannel::~ElinkDeviceDataChannel()
{
}

int ElinkDeviceDataChannel::init()
{
    DeviceDataChannel::init();

    /* regist device profile sync callback */
#ifdef SIM_SUITE
    cloudManager().registSyncDeviceProfileCallback(
        std::bind(
            &ElinkDeviceDataChannel::onSyncDeviceProfile,
            this,
            std::placeholders::_1,
            std::placeholders::_2)
        );
#else
    std::map<std::string, OCFDevice::Ptr> deviceList;
    deviceManager().GetAllDevices(deviceList);
    LOGD("GetAllDevices size = [%d]\n", deviceList.size());
    std::map<std::string, OCFDevice::Ptr>::iterator device;
    for (device = deviceList.begin(); device != deviceList.end(); device++) {
        std::string deviceId = device->first;
        LOGD("get device : %s %s\n", deviceId.c_str(), device->second->m_deviceType.c_str());
        onDeviceStatusChanged(deviceId, device->second->m_deviceType, HB_DEVICE_STATUS_ONLINE);
        std::map<std::string, HBPropertyType> propertyList;
        for (int i = 0; i < 3; ++i) {
            if (-1 == deviceManager().GetDevicePropertyList(deviceId, propertyList))
                continue;
            break;
        }
        std::map<std::string, HBPropertyType>::iterator property;
        for (property = propertyList.begin(); property != propertyList.end(); property++) {
            std::string propertyKey = property->first;
            if (propertyKey == "n")
                continue;
            std::string propertyValue;
            for (int i = 0; i < 3; ++i) {
                if (-1 == deviceManager().GetDevicePropertyValue(deviceId, propertyKey, propertyValue, false))
                    continue;

                LOGD("getProperty[%s]: %s = %s\n", deviceId.c_str(), propertyKey.c_str(), propertyValue.c_str());
                onDevicePropertyChanged(deviceId, propertyKey, propertyValue);
                break;
            }
        }
    }
#endif
    LOGD("init end\n");
    return 0;
}

bool ElinkDeviceDataChannel::_ParsePropValue(const char *cpropval, Slot &slot)
{
    char *propval = (char*)cpropval;
    char *t_and = strstr(propval, "and");
    if (!t_and)
        return true;
    char lside[64] = { 0 };
    char rside[64] = { 0 };
    strncpy(lside, propval, t_and - propval);
    strcpy(rside, t_and+3);

    if (lside[0]) {
        char *egt = strstr(lside, ">=");
        if (!egt)
            return false;
        slot.mMin = std::string(egt + 2);
        stringTrim(slot.mMin);
    }

    if (rside[0]) {
        char *elt = strstr(propval, "<=");
        if (!elt)
            return false;
        slot.mMax = std::string(elt + 2);
        stringTrim(slot.mMax);
    }
    return true;
}

void ElinkDeviceDataChannel::onSyncDeviceProfile(const std::string deviceName, const std::string jsonDoc)
{
    // LOGD("jsondoc:\n%s\n", jsondoc.c_str());
    rapidjson::Document doc;
    doc.Parse<0>(jsonDoc.c_str());
    if (doc.HasParseError()) {
        rapidjson::ParseErrorCode code = doc.GetParseError();
        LOGE("rapidjson parse error[%d]\n", code);
        return;
    }

    if (!doc.HasMember("status") && !doc.HasMember("result")) {
        LOGE("rapidjson parse error!\n");
        return;
    }

    rapidjson::Value &result = doc["result"];
    if (!result.HasMember("profile")) {
        LOGE("rapidjson parse error, no profile key!\n");
        return;
    }

    rapidjson::Value &profile = result["profile"];

    /* elink v0.0.7 */
    std::string profileVersion("0.0.7");

    std::shared_ptr<ClassPayload> payload = std::make_shared<ClassPayload>(deviceName, "DEVICE", profileVersion);

    if (!profile.IsObject()) {
        LOGE("parse profile error, not object!\n");
        return;
    }
    rapidjson::Value::ConstMemberIterator itprofile;
    for (itprofile = profile.MemberBegin(); itprofile != profile.MemberEnd(); ++itprofile) {
        LOGI("%s is %d\n", itprofile->name.GetString(), itprofile->value.GetType());
        Slot &slot = payload->makeSlot(itprofile->name.GetString());
        if (!itprofile->value.IsObject()) {
            LOGE("parse profile %s.%s error, value is not object!\n", deviceName.c_str(), itprofile->name.GetString());
            return;
        }
        if (!itprofile->value.HasMember("type") && !itprofile->value.HasMember("range")) {
            LOGE("parse profile %s.%s.type|range error, not found type!\n", deviceName.c_str(), itprofile->name.GetString());
            return;
        }
        const rapidjson::Value &type = itprofile->value["type"];
        const rapidjson::Value &range = itprofile->value["range"];
        const char *typestr = type.GetString();
        if (!strncmp(typestr, "enum", 4)) {
            if (!range.IsObject()) {
                LOGE("parse profile %s.%s.range error, not object!\n", deviceName.c_str(), itprofile->name.GetString());
                return;
            }
            std::string allowlist;
            rapidjson::Value::ConstMemberIterator itrange;
            bool symb2int = true;
            for (itrange = range.MemberBegin(); itrange != range.MemberEnd(); ++itrange) {
                LOGI("range[%s]: %s is %d\n", itprofile->name.GetString(), itrange->name.GetString(), itrange->value.GetType());
                const char *enumrange = itrange->name.GetString();
                /* TODO */
                if (!isdigit(enumrange[0]))
                    symb2int = false;
                allowlist.append(" ").append(itrange->name.GetString());
            }
            if (symb2int)
                slot.mType = ST_NUMBER;
            else
                slot.mType = ST_SYMBOL;
            slot.mAllowList = stringTrim(allowlist);
        } else if (!strncmp(typestr, "int", 3)) {
            if (!range.IsString()) {
                LOGE("parse profile %s.%s.range error, not String!\n", deviceName.c_str(), itprofile->name.GetString());
                return;
            }
            if (!_ParsePropValue(range.GetString(), slot)) {
                LOGE("parse profile %s.%s.range error!\n", deviceName.c_str(), itprofile->name.GetString());
                return;
            }
            slot.mType = ST_NUMBER;
        } else if (!strncmp(typestr, "float", 5)) {
            if (!range.IsString()) {
                LOGE("parse profile %s.%s.range error, not String!\n", deviceName.c_str(), itprofile->name.GetString());
                return;
            }
            if (!_ParsePropValue(range.GetString(), slot)) {
                LOGE("parse profile %s.%s.range error!\n", deviceName.c_str(), itprofile->name.GetString());
                return;
            }
            slot.mType = ST_NUMBER;
        } else {
            LOGE("parse profile error, not support type!\n");
            return;
        }
    }
    mH.sendMessage(mH.obtainMessage(RET_CLASS_SYNC, payload));
}

} /* namespace HB */
