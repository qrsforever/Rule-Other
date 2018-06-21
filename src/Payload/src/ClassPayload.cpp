/***************************************************************************
 *  ClassPayload.cpp - Class Payload Impl
 *
 *  Created: 2018-06-21 13:55:56
 *
 *  Copyright QRS
 ****************************************************************************/

#include "ClassPayload.h"

namespace HB {

Slot::Slot(SlotType type, std::string name)
    : mType(type), mName(name)
{
}

Slot::~Slot()
{
}

std::string Slot::toString(std::string fmt)
{
    std::string str(fmt);
    if (mIsMulti)
        str.append("(multislot ");
    else
        str.append("(slot ");
    str.append(mName);
    switch (mType) {
        case ST_INTEGER:
            str.append(" (type INTEGER) ");
            if (!mAllowList.empty())
                str.append("(allowed-integers ").append(mAllowList).append(") ");
            break;
        case ST_FLOAT:
            str.append(" (type FLOAT) ");
            if (!mAllowList.empty())
                str.append("(allowed-floats ").append(mAllowList).append(") ");
            break;
        case ST_NUMBER:
            str.append(" (type NUMBER) ");
            if (!mAllowList.empty())
                str.append("(allowed-numbers ").append(mAllowList).append(") ");
            break;
        case ST_STRING:
            str.append(" (type STRING) ");
            if (!mAllowList.empty())
                str.append("(allowed-strings ").append(mAllowList).append(") ");
            break;
        case ST_SYMBOL:
        default:
            str.append(" (type SYMBOL) ");
            if (!mAllowList.empty())
                str.append("(allowed-symbols ").append(mAllowList).append(") ");
            break;
    }
    if (!mMin.empty() && !mMax.empty()) /* TODO: mMax > mMin */
        str.append("(range ").append(mMin).append(" ").append(mMax).append(")");
    str.append(")");
    return str;
}

ClassPayload::ClassPayload(std::string clsname, std::string supercls, bool abst, bool react)
    : mClsName(clsname), mSuperCls(supercls)
    , mIsAbstract(abst), mIsReactive(react)
{
}

ClassPayload::~ClassPayload()
{
    mSlots.clear();
}

Slot::pointer ClassPayload::makeSlot(SlotType type, std::string name, bool multi)
{
    Slot::pointer slot(new Slot(type, name));
    slot->mIsMulti = multi;
    mSlots.push_back(slot);
    return slot;
}

Slot::pointer ClassPayload::makeSlot(SlotType type, std::string name, std::string min, std::string max, bool multi)
{
    Slot::pointer slot(new Slot(type, name));
    slot->mIsMulti = multi;
    slot->mMin = min;
    slot->mMax = max;
    mSlots.push_back(slot);
    return slot;
}

Slot::pointer ClassPayload::makeSlot(SlotType type, std::string name, std::string allow, bool multi)
{
    Slot::pointer slot(new Slot(type, name));
    slot->mIsMulti = multi;
    slot->mAllowList = allow;
    mSlots.push_back(slot);
    return slot;
}

std::string ClassPayload::toString(std::string fmt)
{
    std::string str(fmt);
    str.append("(defclass ").append(mClsName).append(" ");
    str.append("(is-a ").append(mSuperCls).append(") ");
    if (mIsAbstract)
        str.append("(role abstract) ");
    else
        str.append("(role concrete) ");
    if (mIsReactive)
        str.append("(pattern-match reactive) ");
    else
        str.append("(pattern-match no-reactive)");

    for (size_t i = 0; i < mSlots.size(); ++i)
        str.append(mSlots[i]->toString());

    str.append("\n)");
    return str;
}

} /* namespace HB */
