/***************************************************************************
 *  MonitorTool.cpp - Monitor tool impl only for debug
 *
 *  Created: 2018-07-18 16:30:15
 *
 *  Copyright QRS
 ****************************************************************************/

#include "MonitorTool.h"
#include "MainPublicHandler.h"
#include "MessageTypes.h"
#include "InstancePayload.h"
#include "RulePayload.h"
#include "RuleEventHandler.h"
#include "RuleEngineService.h"
#include "Log.h"
#include "LogUDP.h"
#include "LogPool.h"
#include "Common.h"

#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef SIM_SUITE
extern "C" HBDeviceManager& deviceManager();
#endif

using namespace UTILS;

namespace HB {

static MonitorTool *gMonitor = 0;
static LogUDP *gLogUDP = 0;

MonitorTool::MonitorTool()
    : mServerPort(8192)
    , mQuitFlag(false), mChannelCallback(0)
{

}

MonitorTool::~MonitorTool()
{

}

int MonitorTool::init(int port)
{
    LOGD("MonitorTool port[%d]\n", port);
    mServerPort = port;
    return 0;
}

int MonitorTool::addClient(int sockfd)
{
    LOGI("add Client: %d\n", sockfd);
    char ok[8] = "success";
    fcntl(sockfd, F_SETFL, O_NONBLOCK);
    send(sockfd, ok, 7, 0);
    mClientSockets.push_back(sockfd);
    /* for auto close socket if 300s no communication */
    mainHandler().removeMessages(MT_MONITOR, MONITOR_CLOSE_CLIENT, sockfd);
    mainHandler().sendMessageDelayed(
        mainHandler().obtainMessage(MT_MONITOR, MONITOR_CLOSE_CLIENT, sockfd), 300000);
    return 0;
}

int MonitorTool::delClient(int sockfd)
{
    LOGI("del Client: %d\n", sockfd);
    mainHandler().removeMessages(MT_MONITOR, MONITOR_CLOSE_CLIENT, sockfd);
    for (size_t i = 0; i < mClientSockets.size(); ++i) {
        if (mClientSockets[i] == sockfd)
            mClientSockets.erase(mClientSockets.begin() + i);
    }
    close(sockfd);
    return 0;
}

void MonitorTool::run()
{
    LOGD("MonitorTool listen thread:[%u]\n", id());

    fd_set rset;
    socklen_t sock_size;
    struct sockaddr_in cli_addr;
    struct sockaddr_in svr_addr;
    int cli_sockfd = -1;
    int svr_sockfd = -1;
    int maxFd = -1;
    int retnum = 0;
    int opt = 1;

    memset(&svr_addr, 0, sizeof(svr_addr));
    svr_addr.sin_family = AF_INET;
    svr_addr.sin_addr.s_addr = INADDR_ANY;
    svr_addr.sin_port = htons(mServerPort);

	if ((svr_sockfd = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
        LOGE("socket error port[%d]\n", mServerPort);
		return;
    }
    setsockopt(svr_sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    for (int i = 0; i < 10; ++i) {
        if (bind(svr_sockfd, (struct sockaddr *)&svr_addr, sizeof(struct sockaddr)) < 0) {
            LOGE("bind error [%s]\n", strerror(errno));
            sleep(3);
            if (i == 9) {
                LOGE("can not bind this port!\n");
                return;
            }
            continue;
        }
        break;
    }
    fcntl(svr_sockfd, F_SETFL, O_NONBLOCK);

    while (!mQuitFlag) {
        maxFd = svr_sockfd;
        FD_ZERO(&rset);
        FD_SET(svr_sockfd, &rset);
        for (size_t i = 0; i < mClientSockets.size(); ++i) {
            if (mClientSockets[i] > maxFd)
                maxFd = mClientSockets[i];
            FD_SET(mClientSockets[i], &rset);
        }
        LOGI("maxFd = %d cliens[%d]\n", maxFd, mClientSockets.size());
        retnum = select(maxFd + 1, &rset, 0, 0, 0);
        if (-1 == retnum) {
            if (EINTR == errno)
                continue;
            return;
        }
        if (FD_ISSET(svr_sockfd, &rset)) {
            if (listen(svr_sockfd, 5) < 0) {
                LOGE("listen error[%s]!\n", strerror(errno));
                continue;
            }
            cli_sockfd = accept(svr_sockfd, (struct sockaddr *)&cli_addr, &sock_size);
            if (cli_sockfd > 0)
                addClient(cli_sockfd);
        } else {
            for (size_t i = 0; i < mClientSockets.size(); ++i) {
                if (FD_ISSET(mClientSockets[i], &rset))
                    doRequest(mClientSockets[i]);
            }
        }
    }
}

int MonitorTool::doRequest(int sockfd)
{
    char buff[2048] = { 0 };
    if (recv(sockfd, buff, 2047, 0) <= 0) {
        LOGE("recv error[%s]!\n", strerror(errno));
        delClient(sockfd);
        return 0;
    }

    LOGD("command: %s\n", buff);

    std::string res("0");
    std::vector<std::string> cmd = stringSplit(buff, ";");
    if (cmd.size() == 0) {
        LOGE("command[%s] format error!\n", buff);
        goto Err;
    }

    if (cmd[0] == "setModuleLogLevel") {
        if (cmd.size() != 3) {
            LOGE("[%d] format error!\n", cmd.size());
            goto Err;
        }
        int l = atoi(cmd[2].c_str());
        if (0 == strcmp(cmd[1].c_str(), "clips")) {
            ruleHandler().sendMessage(ruleHandler().obtainMessage(
                    RET_LOG_LEVEL, l, 0));
        }
        res = int2String(setModuleLogLevel(cmd[1].c_str(), l));
    } else if (cmd[0] == "getModuleLogLevel") {
        if (cmd.size() != 2) {
            LOGE("[%d] format error!\n",cmd.size());
            goto Err;
        }
        res = int2String(getModuleLogLevel(cmd[1].c_str()));
    } else if (cmd[0] == "getModulesName") {
        char names[128];
        res = getModuleLogNames(names, 127);
    } else if (cmd[0] == "quit") {
        LOGD("client[%d] quit\n", sockfd);
        return delClient(sockfd);
    } else if (cmd[0] == "getHomeBrainVersion") {
        res = "1.0.0";
    } else if (cmd[0] == "getRuleEngineVersion") {
        res = "1.0.0";
    } else if (cmd[0] == "startUDPLog") {
        if (cmd.size() != 3) {
            LOGE("[%d] format error!\n",cmd.size());
            goto Err;
        }
        startUDPLog(cmd[1], atoi(cmd[2].c_str()));
    } else if (cmd[0] == "stopUDPLog") {
        stopUDPLog();
    } else if (cmd[0] == "getDevices") {
        res = getDevices();
    } else if (cmd[0] == "getSlots") {
        if (cmd.size() != 2) {
            LOGE("[%d] format error!\n",cmd.size());
            goto Err;
        }
        res = getSlots(cmd[1]);
    } else if (cmd[0] == "getInstaces") {
        if (cmd.size() != 2) {
            LOGE("[%d] format error!\n",cmd.size());
            goto Err;
        }
        res = getInstaces(cmd[1]);
    } else if (cmd[0] == "getInstanceValue") {
        if (cmd.size() != 3) {
            LOGE("[%d] format error!\n",cmd.size());
            goto Err;
        }
        res = ruleEngine().getInstanceValue(innerOfInsname(cmd[1]), cmd[2]);
    } else if (cmd[0] == "updateInstanceValue") {
        if (cmd.size() != 4) {
            LOGE("[%d] format error!\n",cmd.size());
            goto Err;
        }
        if (mChannelCallback) {
            mChannelCallback->onDevicePropertyChanged(cmd[1], cmd[2], cmd[3]);
            res = "0";
        }
    } else if (cmd[0] == "updateDeviceValue") {
        if (cmd.size() != 4) {
            LOGE("[%d] format error!\n",cmd.size());
            goto Err;
        }
        deviceManager().SetDevicePropertyValue(cmd[1], cmd[2], cmd[3], true);
        res = "0";
    } else {
        LOGW("Unkown command[%s]\n", cmd[0].c_str());
        goto Err;
    }
    LOGD("CMD[%s]: [%s]\n", buff, res.c_str());
    if (!res.empty())
        return send(sockfd, res.c_str(), res.size(), 0);
Err:
    return send(sockfd, "-1", 2, 0);
}

void MonitorTool::startUDPLog(std::string &addr, int port)
{
    LOGD("startUDPLog(%s, %d)\n", addr.c_str(), port);
    if (gLogUDP)
        stopUDPLog();
    gLogUDP = new LogUDP(addr.c_str(), port);
    LogPool::getInstance().attachFilter(gLogUDP);
}

void MonitorTool::stopUDPLog()
{
    if (gLogUDP) {
        LogPool::getInstance().detachFilter(gLogUDP);
        delete (gLogUDP);
        gLogUDP = 0;
    }
}

std::string MonitorTool::getDevices()
{
    bool mulflg = false;
    std::string res;
    std::vector<std::string> items = ruleEngine().getDevices();
    for (size_t i = 0; i < items.size(); ++i) {
        if (items[i] == "DEVICE")
            continue;
        if (mulflg)
            res.append(";");
        else
            mulflg = true;
        res.append(items[i]);
    }
    return res;
}

std::string MonitorTool::getSlots(const std::string &clsName)
{
    bool mulflg = false;
    std::string res;
    std::vector<std::string> items = ruleEngine().getSlots(clsName);
    for (size_t i = 0; i < items.size(); ++i) {
        if (mulflg)
            res.append(";");
        else
            mulflg = true;
        res.append(items[i]);
    }
    return res;
}

std::string MonitorTool::getInstaces(const std::string &clsName)
{
    bool mulflg = false;
    std::string res;
    std::vector<std::string> items = ruleEngine().getInstaces(clsName);
    for (size_t i = 0; i < items.size(); ++i) {
        if (mulflg)
            res.append(";");
        else
            mulflg = true;
        res.append(outerOfInsname(items[i]));
    }
    return res;
}

MonitorTool& monitor()
{
    if (0 == gMonitor)
         gMonitor = new MonitorTool();
    return *gMonitor;
}

} /* namespace HB */
