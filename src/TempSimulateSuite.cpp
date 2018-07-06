/***************************************************************************
 *  TempSimulateSuit.cpp - Only for test
 *
 *  Created: 2018-06-15 19:52:41
 *
 *  Copyright QRS
 ****************************************************************************/

#include "TempSimulateSuite.h"
#include "MainPublicHandler.h"
#include "RuleEngineService.h"
#include "RuleEventTypes.h"
#include "RulePayload.h"
#include "Log.h"

#include <fstream>
#include <sstream>

#define TEST_CASE_BLOCK \
"\n/*-----------------------------------------------------------------" \
"\n *   Test Case: %s" \
"\n *-----------------------------------------------------------------*/\n"

namespace HB {

DeviceManager& deviceManager()
{
    static DeviceManager sDM;
    return sDM;
}

void DeviceManager::setProperty(std::string did, std::string pro, std::string val)
{
    LOGD("(%s, %s, %s)\n", did.c_str(), pro.c_str(), val.c_str());
    /* do real work */

    deviceManager().mPropertyCB(did.c_str(), pro.c_str(), val.c_str());
}

CloudManager& cloudManager()
{
    static CloudManager sCM;
    return sCM;
}

static void test_rule(std::string filepath)
{
    std::stringstream jsondoc;
    std::ifstream in;
    in.open(filepath, std::ifstream::in);
    if (in.is_open()) {
        jsondoc << in.rdbuf();
        in.close();

        cloudManager().mRuleSyncCB(jsondoc.str());
    }
}

static void test_profile(std::string clsname, std::string filepath)
{
    std::stringstream jsondoc;
    std::ifstream in;
    in.open(filepath, std::ifstream::in);
    if (in.is_open()) {
        jsondoc << in.rdbuf();
        in.close();
        deviceManager().mProfileCB(clsname, jsondoc.str());
    }
}

std::string getClassByDeviceId(const std::string &deviceId)
{
    if ((deviceId == "04FA8309822A") ||
        (deviceId == "04FA0000000A"))
        return std::string("SweepingRobot");
    if (deviceId == "0007A895C8A7")
        return std::string("AirClean1");
    if (deviceId == "0007A895C7C7")
        return std::string("AirClean2");
    if (deviceId == "00124B00146D743D00")
        return std::string("SmogAlarm");
    if (deviceId == "38D269B0EA1801010311")
        return std::string("Light");
    if (deviceId == "00000000000000000001")
        return std::string("LightSensor");
    if (deviceId == "00000000000000000002")
        return std::string("Letv");
    if (deviceId == "38D269B0EA1886D3E200")
        return std::string("EmergencyButton");

    CRASH();
    return std::string("");
}

void tempSimulateTest(Message *msg)
{
    LOGD("msg: [%d] [%d] [%d]\n", msg->what, msg->arg1, msg->arg2);

#define TEST_END -1
#define TEST_INIT 0
#define TEST_PROFILE_SYNC 1
#define TEST_RULE_SYNC 2
#define TEST_INSTANCE 3
#define TEST_RULE_TRIGGER 4
#define TEST_RULE_AND 5
#define TEST_RULE_OR 6
#define TEST_RULE_INS_ONLINE_LATER 7
#define TEST_RULE_STEP_CONTROL 8

    int delayms = 1000;
    switch (msg->arg1) {
        case TEST_INIT:
            msg->arg1 = TEST_PROFILE_SYNC;
            /* msg->arg1 = TEST_RULE_SYNC; */
            /* msg->arg1 = TEST_INSTANCE; */
            /* msg->arg1 = TEST_RULE_AND; */
            /* msg->arg1 = TEST_RULE_OR; */
            /* msg->arg1 = TEST_RULE_INS_ONLINE_LATER; */
            /* msg->arg1 = TEST_RULE_STEP_CONTROL; */
            msg->arg2 = 0;
            LOGD(TEST_CASE_BLOCK, "<<BEGIN>>");
            break;
        case TEST_PROFILE_SYNC:
            switch (msg->arg2) {/*{{{ test device profile to clp defclass */
/* PASS */      case 1:
                    LOGD(TEST_CASE_BLOCK, "profile sync (0007A895C8A7.json)");
                    test_profile(getClassByDeviceId("0007A895C8A7"), "test/profiles/0007A895C8A7.json");
                    break;
/* PASS */      case 2:
                    LOGD(TEST_CASE_BLOCK, "profile sync (0007A895C7C7.json)");
                    test_profile(getClassByDeviceId("0007A895C7C7"), "test/profiles/0007A895C7C7.json");
                    break;
/* PASS */      case 3:
                    LOGD(TEST_CASE_BLOCK, "profile sync (04FA8309822A.json)");
                    test_profile(getClassByDeviceId("04FA8309822A"), "test/profiles/04FA8309822A.json");
                    break;
/* PASS */      case 4:
                    LOGD(TEST_CASE_BLOCK, "profile sync (00124B00146D743D00.json)");
                    test_profile(getClassByDeviceId("00124B00146D743D00"), "test/profiles/00124B00146D743D00.json");
                default:
                    LOGD(TEST_CASE_BLOCK, "show classes");
                    ruleEngine().core()->debug(DEBUG_SHOW_CLASSES);
                    msg->arg1 = TEST_RULE_SYNC; /* enter next test: rule */
                    msg->arg2 = 0;
                    break;
            }/*}}}*/
            break;
        case TEST_RULE_SYNC:
            switch (msg->arg2) {/*{{{ test rule profile to clp defrule */
/* PASS */      case 1:
                    LOGD(TEST_CASE_BLOCK, "rule sync (manualtest1.json)");
                    test_rule("test/rules/manualtest1.json"); /* 1529578676.958.69587 */
                    break;
/* PASS */      case 2:
                    LOGD(TEST_CASE_BLOCK, "rule sync (manualtest3.json)");
                    test_rule("test/rules/manualtest3.json"); /* 1529574021.272.65916 */
                    break;
/* PASS */      case 3:
                    LOGD(TEST_CASE_BLOCK, "rule sync (autotest1.json)");
                    test_rule("test/rules/autotest1.json"); /* 1529578016.389.86822 */
                    break;
/* PASS */      case 4:
                    LOGD(TEST_CASE_BLOCK, "rule sync (autotest3.json)");
                    test_rule("test/rules/autotest3.json"); /* 1529578775.206.24324 */
                    break;
/* PASS */      case 5:
                    LOGD(TEST_CASE_BLOCK, "rule sync (autotest4.json)");
                    test_rule("test/rules/autotest4.json"); /* 1529583875.818.80441 */
                    break;
                default:
                    LOGD(TEST_CASE_BLOCK, "show rules");
                    ruleEngine().core()->debug(DEBUG_SHOW_RULES);
                    msg->arg1 = TEST_INSTANCE; /* enter next test: instance */
                    msg->arg2 = 0;
                    break;
            }/*}}}*/
            break;
        case TEST_INSTANCE:
            switch (msg->arg2) {/*{{{ test device online offline and property changed */
/* PASS */      case 1:
                    LOGD(TEST_CASE_BLOCK, "instance online (0007A895C8A7)");
                    deviceManager().mStateCB("0007A895C8A7", getClassByDeviceId("0007A895C8A7"), 1); /* ADD */
                    break;
/* PASS */      case 2:
                    LOGD(TEST_CASE_BLOCK, "instance online (04FA8309822A)");
                    deviceManager().mStateCB("04FA8309822A", getClassByDeviceId("04FA8309822A"), 1);
                    break;
/* PASS */      case 3:
                    LOGD(TEST_CASE_BLOCK, "instance online (0007A895C7C7)");
                    deviceManager().mStateCB("0007A895C7C7", getClassByDeviceId("0007A895C7C7"), 1);
                    break;
/* PASS */      case 4:
                    LOGD(TEST_CASE_BLOCK, "instance offline (0007A895C7C7)");
                    deviceManager().mStateCB("0007A895C7C7", getClassByDeviceId("0007A895C7C7"), 2); /* DEL */
                    break;
/* PASS */      case 5:
                    LOGD(TEST_CASE_BLOCK, "instance property change(0007A895C8A7 WorkMode 2)");
                    deviceManager().mPropertyCB("0007A895C8A7", "WorkMode", "2");
                    break;
                default:
                    LOGD(TEST_CASE_BLOCK, "show instances");
                    ruleEngine().core()->debug(DEBUG_SHOW_INSTANCES);
                    msg->arg1 = TEST_RULE_TRIGGER; /* enter next test: rule trigger */
                    msg->arg2 = 0;
                    break;
            }/*}}}*/
            break;
        case TEST_RULE_TRIGGER:
            switch (msg->arg2) {/*{{{ test rule trigger */
/* PASS */      case 1:  /* trigger autotest1: device control rule */
                    LOGD(TEST_CASE_BLOCK, "rule tigger autotest1(0007A895C8A7 WorkMode 4)");
                    deviceManager().mPropertyCB("0007A895C8A7", "WorkMode", "4");
                    /* "actions": {
                     *     "deviceControl": [
                     *         {
                     *             "propValue": "2",
                     *             "deviceId": "04FA8309822A",
                     *             "propName": "CleaningMode"
                     *         }
                     *     ]
                     * } */
                    break;
/* PASS */      case 2:  /* trigger autotest3: scene manual rule */
                    LOGD(TEST_CASE_BLOCK, "rule tigger autotest3(0007A895C8A7 WorkMode 5)");
                    deviceManager().mPropertyCB("0007A895C8A7", "WorkMode", "5");
                    /* "actions": {
                     *     "manualRuleId": [
                     *         "1529578676.958.69587"
                     *     ]
                     * } */
                    break;
/* PASS */      case 3:  /* trigger autotest4: notify rule */
                    LOGD(TEST_CASE_BLOCK, "rule tigger autotest4(0007A895C8A7 WorkMode 1)");
                    deviceManager().mPropertyCB("0007A895C8A7", "WorkMode", "1");
                    /* "actions": {
                     *     "notify": {
                     *         "message": "",
                     *         "title": ""
                     *     }
                     * } */
                    break;
/* PASS */      case 4: /* trigger manualtest1 */
                    LOGD(TEST_CASE_BLOCK, "rule tigger manualtest1(1529578676.958.69587)");
                    ruleEngine().triggerRule(innerOfRulename("1529578676.958.69587"));
                    break;
/* PASS */      case 5: /* trigger manualtest3 */
                    LOGD(TEST_CASE_BLOCK, "rule tigger manualtest3(1529574021.272.65916)");
                    ruleEngine().triggerRule(innerOfRulename("1529574021.272.65916"));
                    break;
                default:
                    LOGD(TEST_CASE_BLOCK, "show agenda");
                    ruleEngine().core()->debug(DEBUG_SHOW_AGENDA);
                    msg->arg1 = TEST_RULE_AND;  /* enter next test */
                    msg->arg2 = 0;
            }/*}}}*/
            break;
        case TEST_RULE_AND:
            switch (msg->arg2) {/*{{{ test rule logic and */
/* PASS */      case 1:
                    LOGD(TEST_CASE_BLOCK, "rule and (Light / LightSensor / Letv profile)");
                    test_profile(getClassByDeviceId("38D269B0EA1801010311"), "test/profiles/38D269B0EA1801010311.json");
                    test_profile("LightSensor", "test/profiles/Light-Sensor.json");
                    test_profile("Letv", "test/profiles/Letv.json");
                    break;
/* PASS */      case 2:
                    LOGD(TEST_CASE_BLOCK, "rule and (rule Light / LightSensor / Letv)");
                    test_rule("test/rules/tv-light-rule.json"); /* 0000000000.000.00001 */
                    break;
/* PASS */      case 3:
                    LOGD(TEST_CASE_BLOCK, "rule and (online Light / LightSensor / Letv)");
                    deviceManager().mStateCB("38D269B0EA1801010311", getClassByDeviceId("38D269B0EA1801010311"), 1);
                    deviceManager().mStateCB("00000000000000000001", "LightSensor", 1);
                    deviceManager().mStateCB("00000000000000000002", "Letv", 1);
                    break;
/* PASS */      case 4:
                    LOGD(TEST_CASE_BLOCK, "rule and [(Light.PowerOnOff,1), (Letv.PowerOnOff,1), (LightSensor.Quantity,15)]");
                    deviceManager().mPropertyCB("38D269B0EA1801010311", "PowerOnOff", "1");
                    deviceManager().mPropertyCB("00000000000000000002", "PowerOnOff", "1");
                    deviceManager().mPropertyCB("00000000000000000001", "Quantity", "15");
                    break;
                default:
                    LOGD(TEST_CASE_BLOCK, "show agenda");
                    ruleEngine().core()->debug(DEBUG_SHOW_AGENDA);
                    msg->arg1 = TEST_RULE_OR; /* enter next test */
                    msg->arg2 = 0;
            }/*}}}*/
            break;
        case TEST_RULE_OR:
            switch (msg->arg2) {/*{{{ test rule logic or */
/* PASS */      case 1:
                    LOGD(TEST_CASE_BLOCK, "rule or (EmergencyButton / SmogAlarm profile)");
                    test_profile(getClassByDeviceId("38D269B0EA1886D3E200"), "test/profiles/38D269B0EA1886D3E200.json");
                    test_profile(getClassByDeviceId("00124B00146D743D00"), "test/profiles/00124B00146D743D00.json");
                    break;
/* PASS */      case 2:
                    LOGD(TEST_CASE_BLOCK, "rule or (rule Emergency-Alarm)");
                    test_rule("test/rules/emergency-alarm.json"); /* 0000000000.000.00002 */
                    break;
/* PASS */      case 3:
                    LOGD(TEST_CASE_BLOCK, "rule or (online EmergencyButton / SmogAlarm)");
                    deviceManager().mStateCB("38D269B0EA1886D3E200", getClassByDeviceId("38D269B0EA1886D3E200"), 1);
                    deviceManager().mStateCB("00124B00146D743D00", getClassByDeviceId("00124B00146D743D00"), 1);
                    break;
/* PASS */      case 4:
                    LOGD(TEST_CASE_BLOCK, "rule or [(EmergencyButton.PowerOnOff,1), (SmogAlarm.PowerOnOff,1)]");
                    /* deviceManager().mPropertyCB("38D269B0EA1886D3E200", "PowerOnOff", "1"); */
                    deviceManager().mPropertyCB("00124B00146D743D00", "PowerOnOff", "1");
                    break;
                default:
                    LOGD(TEST_CASE_BLOCK, "show agenda");
                    ruleEngine().core()->debug(DEBUG_SHOW_AGENDA);
                    msg->arg1 = TEST_RULE_INS_ONLINE_LATER; /* enter next test */
                    msg->arg2 = 0;
            }/*}}}*/
            break;
        case TEST_RULE_INS_ONLINE_LATER:
            switch (msg->arg2) {/*{{{ test device online to later, if rhs depend it, refresh this rule */
/* PASS */      case 1:
                    LOGD(TEST_CASE_BLOCK, "device online to later (offline 04FA8309822A instance)");
                    deviceManager().mStateCB("04FA8309822A", getClassByDeviceId("04FA8309822A"), 2);
                    break;
/* PASS */      case 2:
                    LOGD(TEST_CASE_BLOCK, "device online to later (0007A895C8A7 WorkMode 4)");
                    deviceManager().mStateCB("0007A895C8A7", getClassByDeviceId("0007A895C8A7"), 1);
                    deviceManager().mPropertyCB("0007A895C8A7", "WorkMode", "4");
                    /* modify delay message timer */
                    delayms = 3000;
                    break;
/* PASS */      case 3:
                    LOGD(TEST_CASE_BLOCK, "device online to later (online 04FA8309822A instance)");
                    /* deviceManager().mPropertyCB("0007A895C8A7", "WorkMode", "3"); */
                    deviceManager().mStateCB("04FA8309822A", getClassByDeviceId("04FA8309822A"), 1);
                    /* restore delay message timer */
                    delayms = 1000;
                    break;
                default:
                    LOGD(TEST_CASE_BLOCK, "show agenda");
                    ruleEngine().core()->debug(DEBUG_SHOW_AGENDA);
                    msg->arg1 = TEST_RULE_STEP_CONTROL; /* enter next test */
                    msg->arg2 = 0;
            }/*}}}*/
            break;
        case TEST_RULE_STEP_CONTROL:
            switch (msg->arg2) {/*{{{ test step control */
/* PASS */      case 1:
                    test_profile(getClassByDeviceId("0007A895C8A7"), "test/profiles/0007A895C8A7.json");
                    test_profile(getClassByDeviceId("04FA8309822A"), "test/profiles/04FA8309822A.json");
                    // test_rule("test/rules/step-control.json"); /* 0000000000.000.00004 */
/* PASS */      case 2:
                    deviceManager().mStateCB("0007A895C8A7", getClassByDeviceId("0007A895C8A7"), 1);
                    deviceManager().mStateCB("04FA0000000A", getClassByDeviceId("04FA0000000A"), 1);
                    deviceManager().mPropertyCB("04FA0000000A", "DumpEnergy", "100");
                    break;
/* PASS */      case 3:
                    deviceManager().mPropertyCB("0007A895C8A7", "WorkMode", "5");
                    break;
                default:
                    LOGD(TEST_CASE_BLOCK, "show agenda");
                    ruleEngine().core()->debug(DEBUG_SHOW_AGENDA);
                    msg->arg1 = TEST_END; /* enter next test */
                    msg->arg2 = 0;
            }/*}}}*/
            break;
        default:
            LOGD(TEST_CASE_BLOCK, "<<loop show all per 5s>>");
            ruleEngine().core()->debug(DEBUG_SHOW_ALL);
            delayms = 5000;
    }

    /* loop test */
    mainHandler().sendMessageDelayed(
        mainHandler().obtainMessage(msg->what, msg->arg1, msg->arg2 + 1), delayms);
}

} /* namespace HB */
