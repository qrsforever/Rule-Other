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

class DeviceDataChannel : public DataChannel {
public:
    DeviceDataChannel();
    ~DeviceDataChannel();

    bool send(std::string key, int action, std::shared_ptr<DataPayload> payload);

private:

}; /* class DeviceDataChannel */

} /* namespace HB */

#endif /* __cplusplus */

#endif /* __DeviceDataChannel_H__ */
