---

title: Rule Engine Intro
date: 2018-06-14 15:48:14
tags: [ IOT, DrawIt ]
categories: [ Local ]

---

Rule Model Design
=================

LHS Tree
--------

```
LHSNode Tree

                                              *********          "and": condition logic
                                           *** LHSNode  **
                                           *             *
                                           ***  "and"  ***
                                         /    *********    \
                                        /         |         \
                             child     /          |          \   child
                                      /      cond |           \
                                     /            |            \
                                    /             |             \
                                   /              |              \
                                  /               |               \
                                 /                |                \
                      *********            +-------------+            *********
                   *** LHSNode ***         |  Condition  |         *** LHSNode ***
                   *             *         |             |         *             *
                   ***  "or"   ***         |    "and"    |         ***   "or"  ***
                   /  *********  \         +-------------+            *********
                  /               \
                 /                 \            "and": slot logic, only support "and"
                / cond         cond \
               /                     \                                                +---------------------------+
              /                       \                                               |          +------+         |
      +-------------+           +-------------+                                       |          |      |         |
      |  Condition  |           |  Condition  |                                     Fact    Instance    |         |
      |             |           |             |                                       \        /        |         |
      |   "and"     |           |    "and"    |                                        \ type /         |         |
      +-------------+           +-------------+                                         \    /          v         v
                                       |                                               Condition <---> Device | TimeEvent
                                       |                                               SlotPoint <---> Property
                                       |
                -----------+-----------+-----------+---------------
                           |                       |
                           |                       |
                           v                       v
                    +-------------+         +-------------+     "&": cell logic
                    |  SlotPoint  |         |  SlotPoint  |     "|": cell logic
                    |             |         |             |     "~"
                    |    "&"      |         |     "|"     |
                    +-------------+         +-------------+
                           |                       |
                           |                       |
                           v                       v
                 +-------------------+    +-------------------+
                 |       Cell        |    |       Cell        |    Compare Symbol: "=, >, <, >=, <=, <>"
                 |                   |    |                   |    Connective Symbol: "&, |, ~"
                 |(v > 10) & (v < 20)|    |(v = 10) | (v > 20)|
                 +-------------------+    +-------------------+

```

Fig Sample
----------

```
                            ruleID
                              ^                   -----> comment, here we want is rule name.
                              |                  /
                              |                 /                  -----> MultiSlot
                    (defrule rul-0001 "this is an example"        /
          +-------    (and                                       /
          |             (and                -----------------------------------------------
          |               ?fct_t1 <- (time ?year ?month ?day ?hour ?minute ?second $?others)      -----+
          |    1          (test (= ?year 2018))                                                        |
          |  layer        (test (= ?month 06))                                                         | Condition: Fact
          |               (test (or (= ?day 20) (= ?day 21) (= ?day 22) (= ?day 23) ))                 |
          |             )                                               -----------\              -----+
  LHSNode |             (or                                                         \------> Cell
          |    2          ?ins-0007A895C8A7 <- (object (is-a TempSensor)                          -----+
          |  layer          (CurrentTemperature ?CurrentTemperature &:(> ?CurrentTemperature 50)       |
          |               )                                                                            |
          |               ?ins-DC330D799327 <- (object (is-a Light)                                    | Condition: Instance
          |                 (onOffLight ?onOffLight &:(= ?onOffLight 1))                               |
          |               )       \                   -----------------                                |
          |             )          \                             \                                -----+
          +-------    )             --------> SlotPoint           \ -------> Cell
                     =>
          +-------    (act-control ins-0007A895C7C7 CurrentTemperature 50)   ---> action: device control
  RHSNode |           (act-control ins-DC330D79932A onOffLight 1)
          |           (act-notify tellYou "Girlfriend Birthday")             ---> action: message notify
          +-------    (act-scene array rul-100 rul-101)                      ---> action: active scene
                    )
```

Rule Class UML
==============

```
                                          +---------------+
 +--------------+                         |   Condition   |                 +------------+               +----------+
 |    Action    |                         |---------------|                 | SlotPoint  |               |  _Cell_  |
 |--------------|                         |   mSlotLogic  |          -----> |------------|        ------>|----------|
 |  mType       |                         |    mType      |         /       | mCellLogic |       /       | nSymbol  |
 |  mCall       | ^                       |    mCls       |        /        | mSlotName  |      /        | nValue   |
 |  mID         |  \                      |    mID        |       /         | mCells     |c2---/mCells   +----------+
 |  mSlotName   |   \                     |   mSlots      |c2----/mSlots    +------------+
 |  mSlotValue  |    -------+             +---------------+
 +--------------+    1:n    | mActions       ^                  +-----------+     mCondLogic: and/or/not
                            |                |                  |           |     mSlotLogic: and               +------------+
                          +------------+     |          +--------------+    |     mCellLogic: &,|,~             |  SlotInfo  |
                          |  RHSNode   |     |          |   LHSNode    |    |     nSymbol: =,>,<,>=,<=,<>       |------------|
                          | -----------|     |          |--------------|    |                                   |   nName    |
                          |  mActions  |     |          |  mCondLogic  |   /mChildren                           |   nValue   |
                          +------------+     +--------c1|  mConditions |  /                                     +------------+
                                   ^         mConditions|  mChildren   |-/  +---------------+                           ^
                                   |                    +--------------+    |  RulePayload  |                    mSlots |  1:n
                                   +----------\                ^            |---------------|                           |
                                               \               |            |  mRuleName    |                           c2
                              +-------------+   \              |            |  mRuleID      |              +-------------------+
                              | DataChannel |    \             |     mLHS   |  mVersion     |              |  InstancePayload  |
                              |-------------|     \            +----------c1|  mLHS         |          /---|-------------------|
                              |             |      -----------------------c1|  mRHS         |         /    |      mInsName     |
                              |send(payload)|                        mLHS   |---------------|        /     |      mClsName     |
                              +-------------+                               |  toString()   |       /      |      mSlots       |
                                  e1^ ^ e1                                  +---------------+      /       +-------------------+
                                  | | | |                                             |           /
                                  | | | |                                             |          |
                  +---------------+ | | +----------------+                            |          |
                  |                 | |                  |                            |          |
                  |                 | |                  |                            e3         |
         +------------------+       | |     +--------------------------+        +----------+ e3--+             +-----------------+
         | RuleDataChannel  |       | |     |    DeviceDataChannel     |        | Payload  |                   |  ClassPayload   |
mCloudMgr|------------------|       | |     | ------------------------ |        |----------| e3----------------|-----------------|
  +----c2|    mCloudMgr     |       | |     |       mDeviceMgr         |        |  type()  |                   |   classname     |
  |      |    mHandler      |       | |     |       mHandler           |        +----------+                   |   superclass    |
  |      +------------------+       | |     |--------------------------|             |                mSlots   |   version       |
  |               e1                | |     |    onStateChanged()      |             |                   +---c1|    mSlots       |
  |               |                 | |     |    onPropertyChanged()   |-------------+               1:n |     |-----------------|
  |               |                 | |     |    onProfileSync()       |                                 |     |   toString()    |
  |    +------------------------+   | |     |         send()           |c2---------------------+         |     +-----------------+
  |    |  ELinkRuleDataChannel  |   | |     +--------------------------+   mHandler            |         |
  |    |------------------------|   | |                  c2                                    |         |
  |    |                        |   | |                  | mDeviceMgr                          |         |
  |    |      onRuleSync()      |   | |                  |                                     |         |          +---------------+
  |    |        send()          |   | |                  |                                     |         |          |     Slot      |
  |    +------------------------+   | |                  |                                     |         +--------> |---------------|
  |                                 | |                  |                                     |                    |    mType      |
  |                                 | |                  |                                     |                    |    mName      |
  |                                 | |  +----------------------------------------+            |                    |   mMin/mMax   |
  |    +-------------------------+  | |  |             DeviceManger               |            |                    |   mAllowList  |
  |    |     CloudManager        |  | |  | -------------------------------------- |            |                    |---------------|
  |    |-------------------------|  | |  |                                        |            |                    |   toString()  |
  +--->|                         |  | |  |   registDeviceStateChangedCallback     |            |                    +---------------+
       | registRuleSyncCallback  |  | |  |   registDevicePropertyChangedCallback  |            |
       |                         |  | |  |   registDeviceProfileSyncCallback      |            |
       +-------------------------+  | |  |        setProperty                     |            |
                                    | |  +----------------------------------------+            |
                                    | |                                                        |
+-----------------------------------+ |                                                        |
| +-----------------------------------+                      +----------------+                |
| |                                                          | MessageHandler |                |
| |                    +----------------------------+        |----------------|                |
| |             +----e4|  MessageHandler::Callback  | <----c1|   mCallback    | e3---------+   |
| |             |      +----------------------------+        +----------------+            |   |
| |             |                                                                          |   |
| |             |                                                                          |   |
| |             |                                                                          |   |
| |             |                                                                          |   v
| |  +----------------------+                                                     +--------------------+
| |  |  RuleEngineService   |                                                     |  RuleEventHandler  |
| |  |----------------------|    mCore         +----------------------+    ------>|--------------------|
| |  |     mCore            | c2-------------> |  RuleEngineCore      |   /       |    handleMessage   |
| |  |     mServerRoot      |                  |----------------------|  /        +--------------------+
| +--|     mDeviceChannel   |                  |    mHandler          | /mHandler           |
+----|     mRuleChannel     |            /---c2|    mEnv              |c2                   |
     |----------------------|           /      |----------------------|                     |
     |    callInstancePush  |          /       |    driver()          |            +------------------+
     |    callMessagePush   |         /        |                      |            | RuleEventThread  |
     |    setDeviceChannel  |        /         |   handleTimer        |            |------------------|
     |    setRuleChannel    |       /          |   handleClassSync    |            |   mMessageQueue  |
     |                      |      /           |   handleRuleSync     |            +------------------+
     |     handleMessage    |     |            |   handleInstanceAdd  |
     +----------------------+     |            |   handleInstanceDel  |
                                  |            |   handleInstancePut  |
                                  |            +----------------------+
                                  v
                          +---------------+
                          |  Environment  |
                          |               |
                          +---------------+
```

Local Build
===========

1. make

> 除了clips代码(编译后在Makefile中指定), 其他依赖模块会依次编译:
    @cd $(MISC_DIR); make;
    @cd $(MESSAGE_DIR);make;
    @cd $(LOG_DIR);make;
    @cd $(CLIPSCPP_DIR);make;
    @cd $(PAYLOAD_DIR);make;
    @cd $(DRIVER_DIR);make;

2. make clean

> 依赖模块会依次清除:
    @cd $(MISC_DIR); make clean;
    @cd $(MESSAGE_DIR);make clean;
    @cd $(LOG_DIR);make clean;
    @cd $(CLIPSCPP_DIR);make clean;
    @cd $(PAYLOAD_DIR);make clean;
    @cd $(DRIVER_DIR);make clean;


TODO
====

1. 不使用模板的MultiSlot的Fact作为条件, Slot是有顺序的, 判断Fact时, 可以省略后面的, 不可以从中间省略, 例如:

    ```:-
    正确:
      ?fct_t1 <- (time ?year ?month ?day ?hour ?minute ?second $?others)
      ?fct_t2 <- (time ?year ?month ?day ?hour ?minute $?others)
      ?fct_t3 <- (time ?year ?month ?day ?hour $?others)
    错误(语法正确, 意义错误, second其实是hour):
      ?fct_t4 <- (time ?year ?month ?day ?second $?others)
    ```

2. 使用带模板的Fact作为条件, 模板Slot没有顺序限制, 但需要谨记在使用完之后retract掉, 否则不会消失, 例如:

    ```:-1
    (defrule test
        ?fct_t1 <- (datetime (year ?y &:(= ?y 2018) (second ?s))
      =>
        (retract fct_t1)
    )

    触发rule:test
    (assert (datetime (year 2018) (second 10)))
    不触发rule, 所以这个fact没有被retract, 该fact一直存在, 浪费MEM, 可以使用Object(Instance)取代
    (assert (datetime (year 2019) (second 10)))
    ```

3. 云端下发的规则ID及为某个设备创建实例ID时, 为了避免规则ID和实例ID可能是纯数字, 比如设备实例其实就是DeviceID, 使用下面API转:

    ```:-
    加ins-前缀
    std::string innerOfInsname(std::string name);
    std::string outerOfInsname(std::string name);

    加rul-前缀
    std::string innerOfRulename(std::string name);
    std::string outerOfRulename(std::string name);
    ```
