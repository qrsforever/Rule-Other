/***************************************************************************
 *  DeviceDataChannel.cpp - Device Data Channel Impl
 *
 *  Created: 2018-06-14 15:05:46
 *
 *  Copyright QRS
 ****************************************************************************/

#include "DeviceDataChannel.h"

namespace HB {

DeviceDataChannel::DeviceDataChannel()
{
}

DeviceDataChannel::~DeviceDataChannel()
{
}

bool DeviceDataChannel::send(std::string key, int action, std::shared_ptr<DataPayload> payload)
{
    return false;
}

} /* namespace HB */
