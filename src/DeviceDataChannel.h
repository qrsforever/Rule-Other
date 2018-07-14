/***************************************************************************
 *  DeviceDataChannel.h - Device Data Channel
 *
 *  Created: 2018-06-14 15:04:25
 *
 *  Copyright QRS
 ****************************************************************************/

#ifndef __DeviceDataChannel_H__
#define __DeviceDataChannel_H__

#include "DataChannel.h"
#include "ClassPayload.h"
#include "rapidjson/document.h"

#include "TempSimulateSuite.h" /* TODO only test */

#ifdef __cplusplus

namespace HB {

class HBDeviceManager;
class RuleEventHandler;

class DeviceDataChannel : public DataChannel, public HBDeviceCallBackHandler {
public:
    DeviceDataChannel();
    ~DeviceDataChannel();

    int init();

    virtual void onDeviceStatusChanged(const std::string deviceId, const std::string deviceName, HBDeviceStatus status);
    virtual void onDevicePropertyChanged(const std::string deviceId, const std::string propertyKey, std::string value);

    virtual bool send(int action, std::shared_ptr<Payload> payload);

protected:
    HBDeviceManager &mDeviceMgr;
    HBCloudManager &mCloudMgr;
    RuleEventHandler &mH;

}; /* class DeviceDataChannel */

class ElinkDeviceDataChannel : public DeviceDataChannel {
public:
    ElinkDeviceDataChannel();
    ~ElinkDeviceDataChannel();

    int init();
    void onSyncDeviceProfile(const std::string deviceName, const std::string jsonDoc);

private:
    bool _ParsePropValue(const char *propval, Slot &slot);

}; /* class ElinkDeviceDataChannel */

} /* namespace HB */

#endif /* __cplusplus */

#endif /* __DeviceDataChannel_H__ */
