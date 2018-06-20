/***************************************************************************
 *  DataChannel.h - Data Channel Header
 *
 *  Created: 2018-06-14 14:33:48
 *
 *  Copyright QRS
 ****************************************************************************/

#ifndef __DataChannel_H__
#define __DataChannel_H__

#include "DataPayload.h"

#include <memory>

#ifdef __cplusplus

namespace HB {

class DataChannel {
public:
    DataChannel() {}
    virtual ~DataChannel() {}

    virtual int init() = 0;
    virtual bool send(std::string key, int action, std::shared_ptr<DataPayload> payload) = 0;
}; /* class DataChannel */

} /* namespace HB */

#endif /* __cplusplus */

#endif /* __DataChannel_H__ */
