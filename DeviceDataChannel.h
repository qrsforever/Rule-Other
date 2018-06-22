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

#ifdef __cplusplus

namespace HB {

class DeviceManager;
class RuleEventHandler;

class DeviceDataChannel : public DataChannel {
public:
    DeviceDataChannel();
    ~DeviceDataChannel();

    int init();

    void onStateChanged(std::string did, std::string devName, int state);
    void onPropertyChanged(std::string did, std::string proKey, std::string proVal);
    void onProfileSync(std::string devName, std::string doc);

    bool send(int action, std::shared_ptr<Payload> payload);

private:
    DeviceManager &mDeviceMgr;
    RuleEventHandler &mH;

}; /* class DeviceDataChannel */

} /* namespace HB */

#endif /* __cplusplus */

#endif /* __DeviceDataChannel_H__ */
