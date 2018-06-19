/***************************************************************************
 *  RuleDataChannel.cpp - Rule Data Channel
 *
 *  Created: 2018-06-14 15:08:33
 *
 *  Copyright QRS
 ****************************************************************************/

#include "RuleDataChannel.h"
#include "RuleEventHandler.h"
#include "RuleEventTypes.h"
#include "Log.h"

#include "TempSimulateSuite.h" /* TODO only test */

namespace HB {

RuleDataChannel::RuleDataChannel()
    : mCloudMgr(cloudManager())
    , mH(ruleHandler())
{
}

RuleDataChannel::~RuleDataChannel()
{
}

bool RuleDataChannel::send(std::string key, int action, std::shared_ptr<DataPayload> payload)
{
    return false;
}

ELinkRuleDataChannel::ELinkRuleDataChannel()
{

}

ELinkRuleDataChannel::~ELinkRuleDataChannel()
{

}

void ELinkRuleDataChannel::init()
{
    LOGTT();

    /* regist rule sync json doc from cloud */
    mCloudMgr.registRuleSyncCallback(
        std::bind(
            &ELinkRuleDataChannel::onRuleSync,
            this,
            std::placeholders::_1)
        );
}

void ELinkRuleDataChannel::onRuleSync(std::string doc)
{
    LOGTT();
    /* TODO need json parse doc */

    std::shared_ptr<RulePayload> payload = std::make_shared<RulePayload>("example", "0001");

    /* Condition */
    payload->mLHS->mCondLogic = "and"; // TOP

    LHSNode &factnodes = payload->mLHS->makeNode();

    Condition &timeCond = factnodes.makeCond(KT_FACT, "time");
    timeCond.makeSlot("year").append("==", "2018");
    timeCond.makeSlot("month").append("==", "10");
    timeCond.makeSlot("day", "or").append("==", "10").append("==", "13").append("==", "17");
    timeCond.makeSlot("hour", "and").append(">=", "0").append("<", "24");
    timeCond.makeSlot("minute").append("==", "0");
    timeCond.makeSlot("second").append("==", "0");

    LHSNode &insnodes = payload->mLHS->makeNode("or");

    LHSNode &node1 = insnodes.makeNode();
    Condition &cond1 = node1.makeCond(KT_INSTANCE, "0007A895C8A7");
    cond1.makeSlot("CurrentTemperature").append(">", "50");

    LHSNode &node2 = insnodes.makeNode();
    Condition &cond2 = node2.makeCond(KT_INSTANCE, "DC330D799327");
    cond2.makeSlot("onOffLight").append("==", "1");

    /* Action */
    payload->mRHS->makeAction(KT_INSTANCE, "0007A895C7C7", "CurrentTemperature", "50");
    payload->mRHS->makeAction(KT_INSTANCE, "DC330D79932A", "onOffLight", "1");
    payload->mRHS->makeAction(KT_FACT, "notify tellYou \"Girlfriend Birthday\"");

    mH.sendMessage(mH.obtainMessage(RET_RULE_SYNC, payload));
}

bool ELinkRuleDataChannel::send(std::string key, int action, std::shared_ptr<DataPayload> payload)
{
    return false;
}

} /* namespace HB */
