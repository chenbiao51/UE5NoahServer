/*
            This file is part of: 
                NoahFrame
            https://github.com/ketoo/NoahGameFrame

   Copyright 2009 - 2021 NoahFrame(NoahGameFrame)

   File creator: lvsheng.huang
   
   NoahFrame is open-source software and you can redistribute it and/or modify
   it under the terms of the License; besides, anyone who use this file/software must include this copyright announcement.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#ifndef NFI_UDP_H
#define NFI_UDP_H

#include <cstring>
#include <cerrno>
#include <cstdio>
#include <csignal>
#include <cstdint>
#include <iostream>
#include <map>
#include <vector>
#include <functional>
#include <memory>
#include <list>
#include <vector>
#include <cassert>
#include "NFComm/NFPluginModule/NFGUID.h"

#if NF_PLATFORM == NF_PLATFORM_WIN
#include <WinSock2.h>
#elif NF_PLATFORM == NF_PLATFORM_APPLE || NF_PLATFORM == NF_PLATFORM_LINUX || NF_PLATFORM == NF_PLATFORM_ANDROID

#if NF_PLATFORM == NF_PLATFORM_APPLE

#include <libkern/OSByteOrder.h>

#endif

#ifdef _XOPEN_SOURCE_EXTENDED
#  include <arpa/inet.h>
# endif

#include <netinet/in.h>
#include <netinet/tcp.h> 
#include <sys/socket.h>
#include <unistd.h>

#endif


#pragma pack(push, 1)

class NFIUdp;

class UdpObject
{
public:
    UdpObject(NFIUdp* pUdp, NFSOCK sock, sockaddr_in& addr, void* pBev)
    {
		logicState = 0;
		gameID = 0;
        fd = sock;
        bNeedRemove = false;

		udpObject = pUdp;

		userData = pBev;
        memset(&sin, 0, sizeof(sin));
        sin = addr;
    }

    virtual ~UdpObject()
    {
    }
	
    int AddBuff(const char* str, size_t len)
    {
        ringBuff.append(str, len);

        return (int) ringBuff.length();
    }

    int CopyBuffTo(char* str, uint32_t start, uint32_t len)
    {
        if (start + len > ringBuff.length())
        {
            return 0;
        }

        memcpy(str, ringBuff.data() + start, len);

        return len;
    }

    int RemoveBuff(uint32_t start, uint32_t len)
    {
        if (start + len > ringBuff.length())
        {
            return 0;
        }

        ringBuff.erase(start, len);

        return (int) ringBuff.length();
    }

    const char* GetBuff()
    {
        return ringBuff.data();
    }

    int GetBuffLen() const
    {
        return (int) ringBuff.length();
    }

    void* GetUserData()
    {
        return userData;
    }

    NFIUdp* GetUdp()
    {
        return udpObject;
    }

    //////////////////////////////////////////////////////////////////////////
    const std::string& GetSecurityKey() const
    {
        return securityKey;
    }

    void SetSecurityKey(const std::string& key)
    {
		securityKey = key;
    }

    int GetConnectKeyState() const
    {
        return logicState;
    }

    void SetConnectKeyState(const int state)
    {
		logicState = state;
    }

    bool NeedRemove()
    {
        return bNeedRemove;
    }

    void SetNeedRemove(bool b)
    {
        bNeedRemove = b;
    }

    const std::string& GetAccount() const
    {
        return account;
    }

    void SetAccount(const std::string& data)
    {
		account = data;
    }

    int GetGameID() const
    {
        return gameID;
    }

    void SetGameID(const int nData)
    {
		gameID = nData;
    }

    const NFGUID& GetUserID()
    {
        return userID;
    }

    void SetUserID(const NFGUID& nUserID)
    {
		userID = nUserID;
    }

    const NFGUID& GetClientID()
    {
        return clientID;
    }

    void SetClientID(const NFGUID& xClientID)
    {
		clientID = xClientID;
    }

    const NFGUID& GetHashIdentID()
		{
        return hashIdentID;
    }

    void SetHashIdentID(const NFGUID& xHashIdentID)
    {
		hashIdentID = xHashIdentID;
    }

    NFSOCK GetRealFD()
    {
        return fd;
    }

private:
    sockaddr_in sin;
    void* userData;
    //ringbuff
    std::string ringBuff;
    std::string account;
    std::string securityKey;

    int32_t logicState;
    int32_t gameID;
    NFGUID userID;//player id
    NFGUID clientID;//temporary client id
    NFGUID hashIdentID;//hash ident, special for distributed
    NFIUdp* udpObject;
    //
    NFSOCK fd;
    bool bNeedRemove;
};


class NFIUdp
{
public:
    virtual ~NFIUdp()
    {}

    //need to call this function every frame to drive network library
    virtual bool Execute() = 0;

    //as client
    virtual void Initialization() = 0;

    //as server
    virtual int Initialization(const unsigned int nMaxClient, const unsigned short nPort, const int nCpuCount = 4) = 0;

    virtual unsigned int ExpandBufferSize(const unsigned int size) = 0;

    virtual bool Final() = 0;

    //send a message with out msg-head[auto add msg-head in this function]
    virtual bool SendMsgWithOutHead(const int16_t msgID, const char* msg, const size_t len, const NFSOCK sockIndex = 0) = 0;

    //send a message with out msg-head[need to add msg-head for this message by youself]
    virtual bool SendMsg(const char* msg, const size_t len, const NFSOCK sockIndex) = 0;


    virtual bool IsServer() = 0;

    virtual bool Log(int severity, const char* msg) = 0;
};

#pragma pack(pop)

#endif
