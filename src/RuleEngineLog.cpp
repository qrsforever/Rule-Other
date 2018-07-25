/***************************************************************************
 *  RuleEngineLog.cpp - Rule Engine Log impl
 *
 *  Created: 2018-07-18 15:39:46
 *
 *  Copyright QRS
 ****************************************************************************/

#include "RuleEngineLog.h"

int gRuleEngineModuleLevel = LOG_LEVEL_DEBUG;
int gRuleClipsModuleLevel = LOG_LEVEL_DEBUG;

namespace HB {

static LogModule RuleEngineModule("rule-engine", gRuleEngineModuleLevel);

static LogModule RuleClipsModule("clips", gRuleClipsModuleLevel);

} /* namespace HB */
