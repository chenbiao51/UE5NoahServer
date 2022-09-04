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

#ifndef NFI_KCP_H
#define NFI_KCP_H

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
#include "NFINet.h"
#include "ikcp.h"
#include "NFComm/NFNetPlugin/KcpObject.h"
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

class NFIKcp;
class KcpObject;
//typedef std::function<void(const KcpObject* kobject)> KCP_ACCEPT_OBJECT;

class NFIKcp
{
public:
    virtual ~NFIKcp()
    {}

    //need to call this function every frame to drive network library
    virtual bool Execute() = 0;

    //as client
    virtual void Initialization(const char* ip, const unsigned short nPort) = 0;

    //as server
    virtual int Initialization(const unsigned int nMaxClient, const unsigned short nPort, const int nCpuCount = 4) = 0;

    virtual unsigned int ExpandBufferSize(const unsigned int size) = 0;

    virtual bool Final() = 0;

    //send a message with out msg-head[auto add msg-head in this function]
    virtual bool SendMsgWithOutHead(const int16_t msgID, const char* msg, const size_t len, const NFSOCK sockIndex = 0) = 0;
    virtual bool Send(const sockaddr_in remoteAddr,const char* data, const NFUINT16& size) =0 ;
    //send a message with out msg-head[need to add msg-head for this message by youself]
    virtual bool SendMsg(const char* msg, const size_t len, const NFSOCK sockIndex) = 0;

    //send a message to all client[need to add msg-head for this message by youself]
    virtual bool SendMsgToAllClient(const char* msg, const size_t len) = 0;

    //send a message with out msg-head to all client[auto add msg-head in this function]
    virtual bool SendMsgToAllClientWithOutHead(const int16_t msgID, const char* msg, const size_t len) = 0;

    virtual bool CloseKcpObject(const NFUINT32 sockIndex) = 0;

    virtual KcpObject* GetKcpObject(const NFUINT32 sockIndex) = 0;

    virtual bool AddKcpObject(const NFUINT32 sockIndex, KcpObject* pObject) = 0;

    virtual bool IsServer() = 0;

    virtual bool Log(int severity, const char* msg) = 0;
};

#pragma pack(pop)

#endif
