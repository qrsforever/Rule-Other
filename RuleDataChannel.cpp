/***************************************************************************
 *  RuleDataChannel.cpp - Rule Data Channel
 *
 *  Created: 2018-06-14 15:08:33
 *
 *  Copyright QRS
 ****************************************************************************/

#include "RuleDataChannel.h"

namespace HB {

RuleDataChannel::RuleDataChannel()
{
}

RuleDataChannel::~RuleDataChannel()
{
}

bool RuleDataChannel::send(std::string key, int action, std::shared_ptr<DataPayload> payload)
{
    return false;
}

} /* namespace HB */
