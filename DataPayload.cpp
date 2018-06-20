/***************************************************************************
 *  DataPayload.cpp - Data Payload Impl
 *
 *  Created: 2018-06-19 12:40:54
 *
 *  Copyright QRS
 ****************************************************************************/

#include "DataPayload.h"

namespace HB {


InstancePayload::InstancePayload()
{
}

InstancePayload::~InstancePayload()
{
    mSlots.clear();
}

SlotPoint::SlotPoint(Condition &cond, std::string name, std::string flag)
    : mSlotName(name)
    , mCellLogic(flag)
    , mCond(cond)
{
}

SlotPoint::~SlotPoint()
{
    mCells.clear();
}

SlotPoint& SlotPoint::append(std::string s1, std::string s2)
{
    mCells.push_back(_Cell_(s1, s2));
    return *this;
}

std::string SlotPoint::getSymbol(size_t index)
{
    if (index > mCells.size())
        return std::string();
    return mCells[index].nSymbol;
}

std::string SlotPoint::getValue(size_t index)
{
    if (index > mCells.size())
        return std::string();
    return mCells[index].nValue;
}

std::string SlotPoint::toString(std::string fmt)
{
    /* check valid */
    if (!cellCount())
        return std::string();

    std::string log(mCellLogic);
    std::string str(fmt);
    if (mCond.mType == CT_FACT) {
        /* convert logic symbol */
        if (mCellLogic == "&")
            log = "and";
        else if (mCellLogic == "|")
            log = "or";
        else if (mCellLogic == "~")
            log = "not";
        else
            log = "none";

        if (log == "none") { /* no constraint */
            if (getSymbol(0) != "none") {
                str.append("(test ");
                str.append("(").append(getSymbol(0));
                str.append(" ?").append(mSlotName).append(" ");
                str.append(getValue(0)).append(")");
                str.append(")");
            }
        } else {
            str.append("(test (").append(log).append(" ");
            for (size_t i = 0; i < cellCount(); ++i) {
                str.append("(").append(getSymbol(i));
                str.append(" ?").append(mSlotName).append(" ");
                str.append(getValue(i)).append(") ");
            }
            str.append("))");
        }
    } else if (mCond.mType == CT_INSTANCE) {
        if (log == "none") {
            if (getSymbol(0) == "none")
                return std::string();
        }
        str.append("(").append(mSlotName);
        str.append(" ?").append(mSlotName).append(" &:");
        for (size_t i = 0; i < cellCount(); ++i) {
            if (i != 0)
                str.append(log).append(":");
            str.append("(").append(getSymbol(i)).append(" ?").append(mSlotName);
            str.append(" ").append(getValue(i)).append(")");
        }
        str.append(")");
    }
    return str;
}

Condition::Condition(ConditionType type, std::string cls, std::string id, std::string logic)
    : mType(type)
    , mCls(cls)
    , mID(id)
    , mSlotLogic(logic)
{
}

Condition::~Condition()
{
    for (size_t i = 0; i < mSlots.size(); ++i)
        delete mSlots[i];
    mSlots.clear();
}

SlotPoint& Condition::makeSlot(std::string name, std::string logic)
{
    SlotPoint *node = new SlotPoint(*this, name, logic);
    if (node)
        mSlots.push_back(node);
    return *node;
}

SlotPoint* Condition::get(size_t index) const
{
    if (index > mSlots.size())
        return 0;
    return mSlots[index];
}

std::string Condition::toString(std::string fmt)
{
    /* check valid */
    if (!slotCount())
        return std::string();

    std::string str(fmt);

    if (mType == CT_FACT) {
        std::string tmp("");
        str.append("(and").append(fmt);
        str.append("  ?").append(mID).append(" <- ");
        str.append("(").append(mCls);
        for (size_t i = 0; i < slotCount(); ++i) {
            str.append(" ?").append(get(i)->mSlotName);
            tmp.append(get(i)->toString());
        }
        str.append(")").append(tmp).append(fmt);
        str.append(")");
    } else if (mType == CT_INSTANCE) {
        str.append("?").append(mID).append(" <- ");
        str.append("(object (is-a ").append(mCls).append(")");
        for (size_t i = 0; i < slotCount(); ++i)
            str.append(get(i)->toString(fmt + "  "));
        str.append(fmt).append(")");
    }
    return str;
}

LHSNode::LHSNode(std::string logic) : mCondLogic(logic)
{
}

LHSNode::~LHSNode()
{
    for (size_t i = 0; i < mConditions.size(); ++i)
        delete mConditions[i];
    mConditions.clear();
    for (size_t i = 0; i < mChildren.size(); ++i)
        delete mChildren[i];
    mChildren.clear();
}

Condition& LHSNode::makeCond(ConditionType type, std::string cls, std::string id)
{
    Condition *cond = new Condition(type, cls, id);
    if (cond)
        mConditions.push_back(cond);
    return *cond;
}

Condition* LHSNode::getCond(size_t index)
{
    if (index > mConditions.size())
        return 0;
    return mConditions[index];
}

LHSNode* LHSNode::getChild(size_t index)
{
    if (index > mChildren.size())
        return 0;
    return mChildren[index];
}

LHSNode& LHSNode::makeNode(std::string logic)
{
    LHSNode *node = new LHSNode(logic);
    if (node)
        mChildren.push_back(node);
    return *node;
}

std::string LHSNode::toString(std::string fmt)
{
    /* check valid */
    if (!childCount() && !condCount())
        return std::string();

    std::string str(fmt);

    str.append("(").append(mCondLogic);

    for (size_t i = 0; i < condCount(); ++i)
        str.append(getCond(i)->toString(fmt+"  "));

    for (size_t i = 0; i < childCount(); ++i)
        str.append(getChild(i)->toString(fmt+"  "));

    str.append(fmt).append(")");

    return str;
}

Action::Action(ActionType type, std::string call, std::string name, std::string val)
    : mType(type)
    , mCall(call)
    , mSlotName(name), mSlotValue(val)
{
}

Action::Action(ActionType type, std::string call, std::string id, std::string name, std::string value)
    : mType(type)
    , mCall(call), mID(id)
    , mSlotName(name), mSlotValue(value)
{
}

std::string Action::toString(std::string fmt)
{
    /* check valid */
    if (mCall.empty())
        return std::string();

    std::string str(fmt);
    str.append("(").append(mCall);
    if (mType == AT_CONTROL) {
        str.append(" ").append(mID);
        str.append(" ").append(mSlotName);
        str.append(" ").append(mSlotValue);
    } else if (mType == AT_NOTIFY) {
        str.append(" ").append(mSlotName);
        str.append(" \"").append(mSlotValue).append("\"");
    } else if (mType == AT_SCENE) {
        str.append(" ").append(mSlotName);
        str.append(" ").append(mSlotValue);
    }
    str.append(")");
    return str;
}

RHSNode::RHSNode()
{

}

RHSNode::~RHSNode()
{
    for (size_t i = 0; i < mActions.size(); ++i)
        delete mActions[i];
    mActions.clear();
}

Action& RHSNode::makeAction(ActionType type, std::string name, std::string value)
{
    Action *act = 0;
    if (type == AT_NOTIFY)
        act = new Action(type, "act_notify", name, value);
    else if (type == AT_SCENE)
        act = new Action(type, "act_scene", name, value);
    else
        act = new Action(AT_ASSERT, "act_assert", name, value);
    mActions.push_back(act);
    return *act;
}

Action& RHSNode::makeAction(ActionType type, std::string id, std::string name, std::string value)
{
    Action *act = 0;
    if (type == AT_CONTROL)
        act = new Action(type, "act_control", id, name, value);
    else
        act = new Action(AT_ASSERT, "act_assert", id, name, value);
    mActions.push_back(act);
    return *act;
}

Action* RHSNode::getAction(size_t index)
{
    if (index > mActions.size())
        return 0;
    return mActions[index];
}

std::string RHSNode::toString(std::string fmt)
{
    /* check valid */
    if (!actionCount())
        return std::string();

    std::string str("");
    for (size_t i = 0; i < actionCount(); ++i)
        str.append(getAction(i)->toString());
    return str;
}

RulePayload::RulePayload(std::string name, std::string id)
    : mRuleName(name)
    , mRuleID(id)
{
    mLHS = std::make_shared<LHSNode>();
    mRHS = std::make_shared<RHSNode>();
}

RulePayload::~RulePayload()
{
    mLHS.reset();
    mRHS.reset();
}

std::string RulePayload::toString(std::string fmt)
{
    std::string str(fmt);
    str.append("(defrule ").append(mRuleName);
    str.append(mLHS->toString());
    str.append(fmt).append("=>");
    str.append(mRHS->toString());
    str.append(fmt).append(")");
    return str.append(fmt);
}

} /* namespace HB */
