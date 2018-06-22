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
#include "RulePayload.h"
#include "InstancePayload.h"
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

bool RuleDataChannel::send(int action, std::shared_ptr<Payload> payload)
{
    return false;
}

ELinkRuleDataChannel::ELinkRuleDataChannel()
{

}

ELinkRuleDataChannel::~ELinkRuleDataChannel()
{

}

int ELinkRuleDataChannel::init()
{
    LOGTT();

    /* regist rule sync json doc from cloud */
    mCloudMgr.registRuleSyncCallback(
        std::bind(
            &ELinkRuleDataChannel::onRuleSync,
            this,
            std::placeholders::_1)
        );

    return 0;
}

void ELinkRuleDataChannel::onRuleSync(std::string doc)
{
    LOGTT();
    /* TODO need json parse doc */

    std::shared_ptr<RulePayload> payload = std::make_shared<RulePayload>(doc, innerOfRulename("0001"), "1.0.0");

    /* Condition */
    payload->mLHS->mCondLogic = "and"; // TOP

    Condition &timeCond = payload->mLHS->makeCond(CT_FACT, "time", "fct_t1");
    timeCond.makeSlot("year").append("=", "2018");
    timeCond.makeSlot("month").append("=", "06");
    timeCond.makeSlot("day", "|").append("=", "20").append("=", "21").append("=", "22");
    timeCond.makeSlot("hour");
    timeCond.makeSlot("minute");
    timeCond.makeSlot("second", "|").append("=", "15").append("=", "25").append("=", "35").append("=", "45");

    Condition &cond1 = payload->mLHS->makeCond(CT_INSTANCE, "Light2", innerOfInsname("000001"));
    cond1.makeSlot("temprature").append(">", "80");

    /* Action */
    payload->mRHS->makeAction(AT_CONTROL, innerOfInsname("000001"), "switch", "0");
    payload->mRHS->makeAction(AT_NOTIFY, "tellYou", "Light is too hot");

    mH.sendMessage(mH.obtainMessage(RET_RULE_SYNC, payload));
}

bool ELinkRuleDataChannel::send(int action, std::shared_ptr<Payload> payload)
{
    return false;
}

} /* namespace HB */
