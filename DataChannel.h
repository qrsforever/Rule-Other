/***************************************************************************
 *  DataChannel.h - Data Channel Header
 *
 *  Created: 2018-06-14 14:33:48
 *
 *  Copyright QRS
 ****************************************************************************/

#ifndef __DataChannel_H__
#define __DataChannel_H__

#include <memory>
#include <string>

#ifdef __cplusplus

namespace HB {

typedef enum {
    PT_DEVICE_PROPERTY,
    PT_RULE_VERSION,
} DataPayloadType;

class DataPayload {
public:
    DataPayload() {}
    ~DataPayload() {}
    virtual DataPayloadType type() = 0;
}; /* class DataPayload */

class PropertyPayload : public DataPayload {
public:
    PropertyPayload(std::string name, std::string value)
        : _name(name), _value(value) {}
    ~PropertyPayload() {}
    DataPayloadType type() { return PT_DEVICE_PROPERTY; }
    const char* name() { return _name.c_str(); }
    const char* value() { return _value.c_str(); }
private:
    std::string _name;
    std::string _value;
}; /* PropertyPayload */

class VersionPayload : public DataPayload {
public:
    VersionPayload(std::string version) : _version(version) {}
    ~VersionPayload() {}
    DataPayloadType type() { return PT_RULE_VERSION; }
    const char* version() { return _version.c_str(); }
private:
    std::string _version;
}; /* class VersionPayload */

class DataChannel {
public:
    DataChannel() {}
    virtual ~DataChannel() {}

    virtual bool send(std::string key, int action, std::shared_ptr<DataPayload> payload) = 0;
}; /* class DataChannel */

} /* namespace HB */

#endif /* __cplusplus */

#endif /* __DataChannel_H__ */
