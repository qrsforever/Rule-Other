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

    std::shared_ptr<RulePayload> payload = std::make_shared<RulePayload>("example", "0001");

    /* Condition */
    payload->mLHS->mCondLogic = "and"; // TOP

    Condition &timeCond = payload->mLHS->makeCond(CT_FACT, "time", "fct_t1");
    timeCond.makeSlot("year").append("=", "2018");
    timeCond.makeSlot("month").append("=", "06");
    timeCond.makeSlot("day", "|").append("=", "20").append("=", "21").append("=", "22");
    timeCond.makeSlot("hour", "none"); /* hour: every hour */
    timeCond.makeSlot("minute"); /* default: none */
    timeCond.makeSlot("second");
    /* timeCond.makeSlot("hour", "&").append(">=", "0").append("<", "24"); */
    /* timeCond.makeSlot("minute").append("=", "0"); */
    /* timeCond.makeSlot("second").append("=", "0"); */

    LHSNode &or_node = payload->mLHS->makeNode("or");

    Condition &cond1 = or_node.makeCond(CT_INSTANCE, "TempSensor", "ins_0007A895C8A7");
    cond1.makeSlot("CurrentTemperature").append(">", "50");

    Condition &cond2 = or_node.makeCond(CT_INSTANCE, "Light", "ins_DC330D799327");
    cond2.makeSlot("onOffLight").append("=", "1");

    /* Action */
    payload->mRHS->makeAction(AT_CONTROL, "ins_0007A895C7C7", "CurrentTemperature", "50");
    payload->mRHS->makeAction(AT_CONTROL, "ins_DC330D79932A", "onOffLight", "1");
    payload->mRHS->makeAction(AT_NOTIFY, "tellYou", "Girlfriend Birthday");
    payload->mRHS->makeAction(AT_SCENE, "array", "100 101");

    mH.sendMessage(mH.obtainMessage(RET_RULE_SYNC, payload));

   /* after parse:
    *
    * (defrule example
    *   (and
    *     ?fct_t1 <- (time ?year ?month ?day ?hour ?minute ?second)
    *     (and
    *       (test (= ?year 2018))
    *       (test (= ?month 6))
    *       (test (or (= ?day 20) (= ?day 21) (= ?day 22) ))
    *       (test (= ?minute 0))
    *       (test (= ?second 0))
    *     )
    *     (or
    *       ?ins_0007A895C8A7 <- (object (is-a TempSensor)
    *         (CurrentTemperature ?CurrentTemperature &:(> ?CurrentTemperature 50))
    *       )
    *       ?ins_DC330D799327 <- (object (is-a Light)
    *         (onOffLight ?onOffLight &:(= ?onOffLight 1))
    *       )
    *     )
    *   )
    * =>
    *   (act_control ins_0007A895C7C7 CurrentTemperature 50)
    *   (act_control ins_DC330D79932A onOffLight 1)
    *   (act_notify tellYou "Girlfriend Birthday")
    *   (act_scene array 100 101)
    * )
    *      */
}

bool ELinkRuleDataChannel::send(std::string key, int action, std::shared_ptr<DataPayload> payload)
{
    return false;
}

} /* namespace HB */
