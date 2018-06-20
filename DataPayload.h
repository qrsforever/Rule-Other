/***************************************************************************
 *  DataPayload.h - Data Payload Header
 *
 *  Created: 2018-06-19 12:38:34
 *
 *  Copyright QRS
 ****************************************************************************/

#ifndef __DataPayload_H__
#define __DataPayload_H__

#include <memory>
#include <string>
#include <vector>

#include "Object.h"

#ifdef __cplusplus

namespace HB {

typedef enum {
    PT_INSTANCE_PAYLOAD,
    PT_RULE_PAYLOAD,
} DataPayloadType;

typedef enum {
    KT_INSTANCE,
    KT_FACT,
} KnowledgeType;

class DataPayload : public ::UTILS::Object {
public:
    DataPayload() {}
    ~DataPayload() {}
    virtual DataPayloadType type() = 0;
}; /* class DataPayload */

class InstancePayload : public DataPayload {
public:
    InstancePayload();
    ~InstancePayload();
    DataPayloadType type() { return PT_INSTANCE_PAYLOAD; }

    std::string mInsName;
    std::string mClsName;

    struct SlotInfo {
        SlotInfo(std::string name, std::string value)
            : nName(name), nValue(value) {}
        std::string nName;
        std::string nValue;
    };
    std::vector<struct SlotInfo> mSlots;
}; /* class InstancePayload */

class SlotNode {
public:
    SlotNode(std::string name) : mPointLogic("or") {}
    SlotNode(std::string name, std::string flag);
    virtual ~SlotNode();
    std::string mSlotName;
    std::string mPointLogic; /* connective symbol: &, |, ~ */
    struct SlotPoint {
        SlotPoint(std::string s1, std::string s2)
            : nCmpFlag(s1), nValue(s2) {}
        std::string nCmpFlag; /* compare symbol: >, <, =, >=, <=, <> */
        std::string nValue;
    };
    size_t pointCount() { return mPoints.size(); }
    SlotNode& append(std::string s1, std::string s2);
    SlotPoint* abtain(size_t index);
private:
    std::vector<SlotPoint> mPoints;
};

class Condition {
public:
    Condition(KnowledgeType type, std::string id, std::string logic = "and");
    virtual ~Condition();
    KnowledgeType mType;
    std::string mID;
    std::string mSlotLogic; /* logic symbol between all slots */
    SlotNode& makeSlot(std::string name, std::string logic = "or");
    SlotNode* get(size_t index) const;
private:
    std::vector<SlotNode *> mSlots;
};

class Action {
public:
    Action(KnowledgeType type, std::string assert)
        : mType(type), mAssert(assert) {}
    Action(KnowledgeType type, std::string id, std::string slotName, std::string slotvalue)
        : mType(type), mID(id), mSlotName(slotName), mSlotValue(slotvalue) {}
    virtual ~Action() { }
    KnowledgeType mType;
    std::string mID;
    std::string mSlotName;
    std::string mSlotValue;
    std::string mAssert;
};

class LHSNode {
public:
    LHSNode(std::string logic = "and");
    virtual ~LHSNode();
    std::string mCondLogic;

    Condition& makeCond(KnowledgeType type, std::string id);
    Condition* getCond(size_t index);
    size_t condCount() { return mConditions.size(); }

    LHSNode& makeNode(std::string logic = "and");
    LHSNode* get(size_t index);
    size_t childCount() { return mChildren.size(); }
private:
    std::vector<Condition *> mConditions; /* leaf node make it sense */
    std::vector<LHSNode *> mChildren;
};

class RHSNode {
public:
    RHSNode();
    virtual ~RHSNode();
    Action& makeAction(KnowledgeType type, std::string assert);
    Action& makeAction(KnowledgeType type, std::string id, std::string slotName, std::string slotvalue);
    Action* getAction(size_t index);
    size_t actionCount() { return mActions.size(); }
private:
    std::vector<Action *> mActions; /* leaf node make it sense */
};

class RulePayload : public DataPayload {
public:
    RulePayload() {}
    RulePayload(std::string name, std::string id);
    ~RulePayload();
    DataPayloadType type() { return PT_RULE_PAYLOAD; }
    std::string mRuleName;
    std::string mRuleID;

    // std::string mCondLogic; /* and, or */

    std::shared_ptr<LHSNode> mLHS;
    std::shared_ptr<RHSNode> mRHS;

    /* std::vector<std::shared_ptr<Condition>> mConditions; */
    /* std::vector<std::shared_ptr<Action>> mActions; */
}; /* class RulePayload */

} /* namespace HB */

#endif /* __cplusplus */

#endif /* __DataPayload_H__ */

/* {
 *     "ruleName":"example",
 *         "ruleId":"
 *             -
 *             ",
 *         "description":"this is a example for ru
 *             le definition",
 *         "trigger":{
 *             "triggerType":"auto|manual",
 *             "switch":{
 *                 "enabled":"on",
 *                 "timeCondition":"on",
 *                 "deviceCondition ":"on",
 *                 "notify":"on",
 *                 "manual":"on"
 *             }
 *         },
 *         "conditions":{
 *             "conditionLogic":"and",
 *             "timeCondition":[
 *             {
 *                 "year":"2018",
 *                 "mouth":"10",
 *                 "day":"10|13|17",
 *                 "hour":"every",
 *                 "minute":"0",
 *                 "second":"0"
 *             }
 *             ],
 *             "deviceCondition":{
 *                 "deviceLogic":"or",
 *                 "deviceStatus":[
 *                 {
 *                     "deviceId":"0007A895C8A7",
 *                     "propName":"CurrentTemperature",
 *                     "propValue":"v>50"
 *                 },
 *                 {
 *                     "deviceId":"DC330D799327",
 *                     "propName":"onOffLight",
 *                     "propValue":"v==true"
 *                 }
 *                 ]
 *             }
 *         },
 *         "actions":{
 *             "notify":{
 *                 "tellYou":"Girlfriend Birthday!"
 *             },
 *             "deviceControl":[
 *             {
 *                 "deviceId":"0007A895C7C7",
 *                 "propName":"CurrentTemperature",
 *                 "propValue":"50"
 *             },
 *             {
 *                 "deviceId":"DC330D79932A",
 *                 "propName":"onOffLight",
 *                 "propValue":"true"
 *             }
 *             ],
 *             "manualRuleId":[
 *                 "100",
 *                 "101"
 *             ]
 *         }
 * } */
