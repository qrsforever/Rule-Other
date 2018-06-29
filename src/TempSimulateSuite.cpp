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
#include "Log.h"

#include <fstream>
#include <sstream>

namespace HB {

DeviceManager& deviceManager()
{
    static DeviceManager sDM;
    return sDM;
}

void DeviceManager::setProperty(std::string did, std::string pro, std::string val)
{
    LOGD("(%s, %s, %s)\n", did.c_str(), pro.c_str(), val.c_str());
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
    if (deviceId == "04FA8309822A")
        return std::string("SweepingRobot");
    if (deviceId == "0007A895C8A7")
        return std::string("AirClean1");
    if (deviceId == "0007A895C7C7")
        return std::string("AirClean2");
    return std::string("Light");
}

void tempSimulateTest(Message *msg)
{
    LOGD("msg: [%d] [%d] [%d]\n", msg->what, msg->arg1, msg->arg2);

#define TEST_INIT 0
#define TEST_PROFILE_SYNC 1
#define TEST_RULE_SYNC 2
#define TEST_INSTANCE 3
#define TEST_RULE_TRIGGER 4

    switch (msg->arg1) {
        case TEST_INIT:
            msg->arg1 = TEST_INSTANCE; //TEST_PROFILE_SYNC;  /* specify test item */
            msg->arg2 = 0;
            break;
        case TEST_PROFILE_SYNC: /* test device profile to clp defclass */
            switch (msg->arg2) {
                case 1:
/* PASS */          test_profile(getClassByDeviceId("0007A895C8A7"), "test/profiles/0007A895C8A7.json");
                    break;
                case 2:
/* PASS */          test_profile(getClassByDeviceId("0007A895C7C7"), "test/profiles/0007A895C7C7.json");
                    break;
                case 3:
/* PASS */          test_profile(getClassByDeviceId("04FA8309822A"), "test/profiles/04FA8309822A.json");
                    break;
                default:
                    ruleEngine().core()->debug(DEBUG_SHOW_CLASSES);
                    msg->arg1 = TEST_RULE_SYNC; /* enter next test: rule */
                    msg->arg2 = 0;
                    break;
            }
            break;
        case TEST_RULE_SYNC: /* test rule profile to clp defrule */
            switch (msg->arg2) {
                case 1:
/* PASS */          test_rule("test/rules/manualtest1.json");
                    break;
                case 2:
/* PASS */          test_rule("test/rules/manualtest3.json");
                    break;
                case 3:
/* PASS */          test_rule("test/rules/autotest1.json");
                    break;
                case 4:
/* PASS */          test_rule("test/rules/autotest3.json");
                    break;
                case 5:
/* PASS */          test_rule("test/rules/autotest4.json");
                    break;
                default:
                    ruleEngine().core()->debug(DEBUG_SHOW_RULES);
                    msg->arg1 = TEST_INSTANCE; /* enter next test: instance */
                    msg->arg2 = 0;
                    break;
            }
            break;
        case TEST_INSTANCE: /* test device */
            switch (msg->arg2) {
                case 1:
/* PASS */          deviceManager().mStateCB("0007A895C8A7", getClassByDeviceId("0007A895C8A7"), 1); /* ADD */
                    break;
                case 2:
/* PASS */          deviceManager().mStateCB("04FA8309822A", getClassByDeviceId("04FA8309822A"), 1);
                    break;
                case 3:
/* PASS */          deviceManager().mStateCB("0007A895C7C7", getClassByDeviceId("0007A895C7C7"), 1);
                    break;
                case 4:
/* PASS */          deviceManager().mStateCB("0007A895C7C7", getClassByDeviceId("0007A895C7C7"), 2); /* DEL */
                    break;
                case 5:
/* PASS */          deviceManager().mPropertyCB("0007A895C8A7", "WorkMode", "2");
                    break;
                default:
                    ruleEngine().core()->debug(DEBUG_SHOW_INSTANCES);
                    msg->arg1 = TEST_RULE_TRIGGER; /* enter next test: rule trigger */
                    msg->arg2 = 0;
                    break;
            }
            break;
        case TEST_RULE_TRIGGER: /* test rule trigger */
            switch (msg->arg2) {
                case 1:  /* trigger autotest1: device control rule */
/* PASS */          deviceManager().mPropertyCB("0007A895C8A7", "WorkMode", "4");
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
                case 2:  /* trigger autotest3: scene manual rule */
/* PASS */          deviceManager().mPropertyCB("0007A895C8A7", "WorkMode", "5");
                    /* "actions": {
                     *     "manualRuleId": [
                     *         "1529578676.958.69587"
                     *     ]
                     * } */
                    break;
                case 3:  /* trigger autotest4: notify rule */
/* PASS */          deviceManager().mPropertyCB("0007A895C8A7", "WorkMode", "1");
                    /* "actions": {
                     *     "notify": {
                     *         "message": "",
                     *         "title": ""
                     *     }
                     * } */
                    break;
                case 4: /* trigger manualtest1 */
/* PASS */          ruleEngine().triggerRule("1529578676.958.69587");
                    break;
                case 5: /* trigger manualtest3 */
/* PASS */          ruleEngine().triggerRule("1529574021.272.65916");
                    break;
                default:
                    ruleEngine().core()->debug(DEBUG_SHOW_AGENDA);
                    return;
            }
            break;
        default:
            ruleEngine().core()->debug(DEBUG_SHOW_ALL);
            return;
    }

    /* loop test */
    mainHandler().sendMessageDelayed(
        mainHandler().obtainMessage(msg->what, msg->arg1, msg->arg2 + 1), 1000);
}

} /* namespace HB */
