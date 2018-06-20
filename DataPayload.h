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
    CT_INSTANCE,
    CT_FACT,
} ConditionType;

typedef enum {
    AT_NOTIFY,
    AT_SCENE,
    AT_CONTROL,
    AT_ASSERT,
} ActionType;

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

class Condition;
class SlotPoint {
public:
    SlotPoint(Condition &cond, std::string name) : mCellLogic("none"), mCond(cond) {}
    SlotPoint(Condition &cond, std::string name, std::string flag);
    virtual ~SlotPoint();
    std::string mSlotName;
    std::string mCellLogic; /* connective symbol: &, |, ~, none*/

    std::string toString(std::string fmt = "\n      ");

    struct _Cell_ {
        _Cell_(std::string s1, std::string s2)
            : nSymbol(s1), nValue(s2) {}
        std::string nSymbol; /* compare symbol: >, <, =, >=, <=, <>, none */
        std::string nValue;
    };
    size_t cellCount() { return mCells.size(); }
    SlotPoint& append(std::string s1, std::string s2);
    std::string getSymbol(size_t index);
    std::string getValue(size_t index);
private:
    Condition& mCond;
    std::vector<_Cell_> mCells;
}; /* class SlotPoint */

class Condition {
public:
    Condition(ConditionType type, std::string cls, std::string id, std::string logic = "none");
    virtual ~Condition();
    ConditionType mType;
    std::string mCls;
    std::string mID;
    std::string mSlotLogic; /* not used (only support "and"): logic symbol between all slots */

    std::string toString(std::string fmt = "\n    ");

    SlotPoint& makeSlot(std::string name, std::string logic = "none");
    SlotPoint* get(size_t index) const;
    size_t slotCount() { return mSlots.size(); }
private:
    std::vector<SlotPoint *> mSlots;
}; /* class Condition */

class LHSNode {
public:
    LHSNode(std::string logic = "and");
    virtual ~LHSNode();
    std::string mCondLogic; /* conditions logic: and, or */

    std::string toString(std::string fmt = "\n  ");

    Condition& makeCond(ConditionType type, std::string cls, std::string id);
    Condition* getCond(size_t index);
    size_t condCount() { return mConditions.size(); }

    LHSNode& makeNode(std::string logic = "and");
    LHSNode* getChild(size_t index);
    size_t childCount() { return mChildren.size(); }
private:
    std::vector<Condition *> mConditions;
    std::vector<LHSNode *> mChildren;
}; /* class LHSNode */

class Action {
public:
    Action(ActionType type, std::string call, std::string name, std::string value);
    Action(ActionType type, std::string call, std::string id, std::string name, std::string value);
    virtual ~Action() { }

    ActionType mType;
    std::string mCall;
    std::string mID;
    std::string mSlotName;
    std::string mSlotValue;

    std::string toString(std::string fmt="\n  ");
}; /* class Action */

class RHSNode {
public:
    RHSNode();
    virtual ~RHSNode();

    std::string toString(std::string fmt="\n  ");

    Action& makeAction(ActionType type, std::string name, std::string value);
    Action& makeAction(ActionType type, std::string id, std::string name, std::string value);
    Action* getAction(size_t index);
    size_t actionCount() { return mActions.size(); }
private:
    std::vector<Action *> mActions;
}; /* class RHSNode */

class RulePayload : public DataPayload {
public:
    RulePayload() {}
    RulePayload(std::string name, std::string id);
    ~RulePayload();
    DataPayloadType type() { return PT_RULE_PAYLOAD; }
    std::string mRuleName;
    std::string mRuleID;

    std::string toString(std::string fmt = "\n");

    std::shared_ptr<LHSNode> mLHS;
    std::shared_ptr<RHSNode> mRHS;

}; /* class RulePayload */

} /* namespace HB */

#endif /* __cplusplus */

#endif /* __DataPayload_H__ */
