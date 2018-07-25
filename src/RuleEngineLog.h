/***************************************************************************
 *  RuleEngineLog.h - Rule Engine Log
 *
 *  Created: 2018-07-18 15:36:53
 *
 *  Copyright QRS
 ****************************************************************************/

#ifndef __RuleEngineLog_H__
#define __RuleEngineLog_H__

#include "Log.h"

#ifdef __cplusplus

extern int gRuleEngineModuleLevel;

#define RE_LOGE(args...)  _LOGE(gRuleEngineModuleLevel, args)
#define RE_LOGW(args...)  _LOGW(gRuleEngineModuleLevel, args)
#define RE_LOGD(args...)  _LOGD(gRuleEngineModuleLevel, args)
#define RE_LOGI(args...)  _LOGI(gRuleEngineModuleLevel, args)
#define RE_LOGT(args...)  _LOGT(gRuleEngineModuleLevel, args)
#define RE_LOGTT()  _LOGT(gRuleEngineModuleLevel, "run here!\n")

#endif /* __cplusplus */

#endif /* __RuleEngineLog_H__ */
