---

title: NULL
date: 2018-06-14 15:48:14
tags: [  ]
categories: [  ]

---

Rule Model Design
=================

```



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

Rule Class UML
==============

```


```
