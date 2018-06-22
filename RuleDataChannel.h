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

class CloudManager;
class RuleEventHandler;

class RuleDataChannel : public DataChannel {
public:
    RuleDataChannel();
    virtual ~RuleDataChannel();

    virtual int init() = 0;
    virtual bool send(int action, std::shared_ptr<Payload> payload);

protected:
    CloudManager &mCloudMgr;
    RuleEventHandler &mH;
}; /* class RuleDataChannel */

class ELinkRuleDataChannel : public RuleDataChannel {
public:
    ELinkRuleDataChannel();
    ~ELinkRuleDataChannel();

    int init();

    void onRuleSync(std::string doc);

    bool send(int action, std::shared_ptr<Payload> payload);

}; /* class ELinkRuleDataChannel */

} /* namespace HB */

#endif /* __cplusplus */

#endif /* __RuleDataChannel_H__ */
