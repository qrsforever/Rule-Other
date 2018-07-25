/***************************************************************************
 *  MonitorTool.h - Monitor tool impl
 *
 *  Created: 2018-07-18 16:29:53
 *
 *  Copyright QRS
 ****************************************************************************/

#ifndef __MonitorTool_H__
#define __MonitorTool_H__

#include "Thread.h"
#include <vector>
#include <string>
#include "DeviceDataChannel.h"

#ifdef __cplusplus

namespace HB {

class MonitorTool : public ::UTILS::Thread {
public:
    MonitorTool();
    ~MonitorTool();

    int init(int port = 8192);
    void run();

    int doRequest(int sockfd);

    int addClient(int sockfd);
    int delClient(int sockfd);

    void startUDPLog(std::string &addr, int port);
    void stopUDPLog();

    std::string getDevices();
    std::string getSlots(const std::string &clsName);
    std::string getInstaces(const std::string &clsName);

    void setDeviceChannel(std::shared_ptr<DeviceDataChannel> channel) { mChannelCallback = channel; }

private:
    int mServerPort;
    bool mQuitFlag;
    std::vector<int> mClientSockets;
    std::shared_ptr<DeviceDataChannel> mChannelCallback;
}; /* class MonitorTool */

MonitorTool& monitor();

} /* namespace HB */

#endif /* __cplusplus */

#endif /* __MonitorTool_H__ */
