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

    virtual void init() = 0;
    virtual bool send(std::string key, int action, std::shared_ptr<DataPayload> payload);

protected:
    CloudManager &mCloudMgr;
    RuleEventHandler &mH;
}; /* class RuleDataChannel */

class ELinkRuleDataChannel : public RuleDataChannel {
public:
    ELinkRuleDataChannel();
    ~ELinkRuleDataChannel();

    void init();

    void onRuleSync(std::string doc);

    bool send(std::string key, int action, std::shared_ptr<DataPayload> payload);

}; /* class ELinkRuleDataChannel */

} /* namespace HB */

#endif /* __cplusplus */

#endif /* __RuleDataChannel_H__ */
