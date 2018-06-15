/***************************************************************************
 *  RuleDataChannel.h - Rule Data Channel
 *
 *  Created: 2018-06-14 15:02:39
 *
 *  Copyright QRS
 ****************************************************************************/

#ifndef __RuleDataChannel_H__
#define __RuleDataChannel_H__

#include "DataChannel.h"

#ifdef __cplusplus

namespace HB {

class RuleDataChannel : public DataChannel {
public:
    RuleDataChannel();
    virtual ~RuleDataChannel();

    virtual bool send(std::string key, int action, std::shared_ptr<DataPayload> payload);

private:

}; /* class RuleDataChannel */

class ELinkRuleDataChannel : public RuleDataChannel {
public:
    ELinkRuleDataChannel() {}
    ~ELinkRuleDataChannel() {}

}; /* class ELinkRuleDataChannel */

} /* namespace HB */

#endif /* __cplusplus */

#endif /* __RuleDataChannel_H__ */
