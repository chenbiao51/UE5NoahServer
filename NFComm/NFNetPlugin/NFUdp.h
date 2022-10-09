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

#ifndef NF_UDP_H
#define NF_UDP_H

#include <set>
#include <algorithm>
#include <thread>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/thread.h>
#include <event2/event_compat.h>

#include <event.h>

#include "NFINet.h"
#include "NFIUdp.h"
#include "Dependencies/concurrentqueue/concurrentqueue.h"

#pragma pack(push, 1)



class NFUdp : public NFIUdp
{
public:
    NFUdp()
    {
        mxBase = NULL;

        mstrIP = "";
        mnPort = 0;
        mnCpuCount = 0;
        mbServer = false;
        mbWorking = false;

        mnSendMsgTotal = 0;
        mnReceiveMsgTotal = 0;

		mnBufferSize = 0;
        mbTCPStream = false;
    }

    template<typename BaseType>
    NFUdp(BaseType* pBaseType, void (BaseType::*handleReceive)(const NFSOCK, const int, const char*, const uint32_t), void (BaseType::*handleEvent)(const NFSOCK, const NF_NET_EVENT, NFIUdp*), bool tcpStream = false)
    {
        mxBase = NULL;

        mRecvCB = std::bind(handleReceive, pBaseType, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        mEventCB = std::bind(handleEvent, pBaseType, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        mstrIP = "";
        mnPort = 0;
        mnCpuCount = 0;
        mbServer = false;
        mbWorking = false;
        
        mnSendMsgTotal = 0;
        mnReceiveMsgTotal = 0;

		mnBufferSize = 0;
        mbTCPStream = tcpStream;
    }
    
    virtual ~NFUdp() {};

public:
    virtual bool Execute() override ;

    virtual void Initialization() override ;
    virtual int Initialization(const unsigned int nMaxClient, const unsigned short nPort, const int nCpuCount = 4) override ;
	virtual unsigned int ExpandBufferSize(const unsigned int size) override;
    virtual bool AddUdpObject(const NFSOCK sockIndex, UdpObject* pObject) override ;
    virtual bool Final() override ;

    virtual bool SendMsg(const char* msg, const size_t len, const NFSOCK sockIndex) override ;

    virtual bool SendMsgWithOutHead(const int16_t msgID, const char* msg, const size_t len, const NFSOCK sockIndex) override ;


    virtual bool IsServer() override ;
    virtual bool Log(int severity, const char* msg) override ;

private:
	bool SendMsgWithOutHead(const int16_t msgID, const char* msg, const size_t len, const std::list<NFSOCK>& fdList);

    bool SendMsg(const char* msg, const size_t len, const std::list<NFSOCK>& fdList);


private:
    void ExecuteClose();

    bool Dismantle(NetObject* pObject);


    int InitClientNet();
    int InitServerNet();


    static void listener_cb(const int sockfd, short int which, void *arg);
    static void recvfrom_cb(const int sockfd, short int which, void *arg);
    static void log_cb(int severity, const char* msg);


protected:
    int DeCode(const char* strData, const uint32_t ulen, NFMsgHead& xHead);
    int EnCode(const uint16_t umsgID, const char* strData, const uint32_t unDataLen, std::string& strOutData);

private:
    std::map<NFSOCK, UdpObject*> mmObject;

    int mnMaxConnect;
    std::string mstrIP;
    int mnPort;
    int mnCpuCount;
	bool mbServer;

    unsigned int mnBufferSize;

    bool mbWorking;
    bool mbTCPStream;

    int64_t mnSendMsgTotal;
    int64_t mnReceiveMsgTotal;

    struct event_base* mxBase;
    struct event mxEvent;
    //////////////////////////////////////////////////////////////////////////

    UDP_RECEIVE_FUNCTOR mRecvCB;
    UDP_EVENT_FUNCTOR mEventCB;

    //1: async thread to process net event & msg and main thread to process logic business(decode binary data to message object)
    //2: pass a functor when startup net module to decode binary data to message object with async thread
    // struct NetEvent
	// {
	// 	NF_NET_EVENT event;
	// 	int fd = 0;
	// 	//std::string* data;
	// 	char* data = nullptr;
	// 	int len = 0;
	// 	void* dataObject = nullptr;
	// };
	// moodycamel::ConcurrentQueue<NetEvent> msgQueue;
    //////////////////////////////////////////////////////////////////////////
};

#pragma pack(pop)

#endif
