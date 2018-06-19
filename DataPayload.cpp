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

SlotNode::SlotNode(std::string name, std::string flag)
    : mSlotName(name)
    , mPointLogic(flag)
{
}

SlotNode::~SlotNode()
{
    mPoints.clear();
}

SlotNode& SlotNode::append(std::string s1, std::string s2)
{
    mPoints.push_back(SlotPoint(s1, s2));
    return *this;
}

SlotNode::SlotPoint* SlotNode::abtain(size_t index)
{
    if (index > mPoints.size())
        return 0;
    return &mPoints[index];
}

Condition::Condition(KnowledgeType type, std::string id, std::string logic)
    : mType(type)
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

SlotNode& Condition::makeSlot(std::string name, std::string logic)
{
    SlotNode *node = new SlotNode(name, logic);
    if (node)
        mSlots.push_back(node);
    return *node;
}

SlotNode* Condition::get(size_t index) const
{
    if (index > mSlots.size())
        return 0;
    return mSlots[index];
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

Condition& LHSNode::makeCond(KnowledgeType type, std::string id)
{
    Condition *cond = new Condition(type, id);
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

LHSNode* LHSNode::get(size_t index)
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

RHSNode::RHSNode()
{

}

RHSNode::~RHSNode()
{
    for (size_t i = 0; i < mActions.size(); ++i)
        delete mActions[i];
    mActions.clear();
}

Action& RHSNode::makeAction(KnowledgeType type, std::string assert)
{
    Action *act = new Action(type, assert);
    mActions.push_back(act);
    return *act;
}

Action& RHSNode::makeAction(KnowledgeType type, std::string id, std::string slotName, std::string slotvalue)
{
    Action *act = new Action(type, id, slotName, slotvalue);
    mActions.push_back(act);
    return *act;
}

Action* RHSNode::getAction(size_t index)
{
    if (index > mActions.size())
        return 0;
    return mActions[index];
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

} /* namespace HB */
