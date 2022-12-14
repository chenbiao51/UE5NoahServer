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

#ifndef NF_KCP_H
#define NF_KCP_H

#include <set>
#include <algorithm>
#include <thread>
#include <event.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/thread.h>
#include <event2/event_compat.h>
#include "NFINet.h"
#include "NFIKcp.h"
#include "ikcp.h"
#include "KcpObject.h"
#include "Dependencies/concurrentqueue/concurrentqueue.h"

#pragma pack(push, 1)

class  NFKcp;
class  KcpObject;

typedef std::function<void(const NFSOCK sockIndex, const int msgID, const char* msg, const uint32_t len)> KCP_RECEIVE_FUNCTOR;
typedef std::shared_ptr<KCP_RECEIVE_FUNCTOR> KCP_RECEIVE_FUNCTOR_PTR;

typedef std::function<void(const NFSOCK sockIndex, const NF_NET_EVENT nEvent, NFIKcp* pKcp)> KCP_EVENT_FUNCTOR;
typedef std::shared_ptr<KCP_EVENT_FUNCTOR> KCP_EVENT_FUNCTOR_PTR;

class NFKcp : public NFIKcp
{
public:
    NFKcp()
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
    }

    template<typename BaseType>
    NFKcp(BaseType* pBaseType, void (BaseType::*handleReceive)(const NFSOCK, const int, const char*, const uint32_t), void (BaseType::*handleEvent)(const NFSOCK, const NF_NET_EVENT, NFIKcp*))
    {
        mxBase = NULL;

        mRecvCB = std::bind(handleReceive, pBaseType, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
        mEventCB = std::bind(handleEvent, pBaseType, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        mstrIP = "";
        mnPort = 0;
        mnCpuCount = 0;
        mbWorking = false;
        
        mnSendMsgTotal = 0;
        mnReceiveMsgTotal = 0;

		mnBufferSize = 0;

    }
    
    virtual ~NFKcp() {};

public:
    virtual bool Execute() override ;

    virtual void Initialization(const char* ip, const unsigned short nPort) override ;
    virtual int Initialization(const unsigned int nMaxClient, const unsigned short nPort, const int nCpuCount = 4) override ;
	virtual unsigned int ExpandBufferSize(const unsigned int size) override;

    virtual bool Final() override ;

    virtual bool Send(const sockaddr_in* remoteAddr,const char* data, const NFUINT16& size) override ;
    virtual bool SendMsg(const char* msg, const size_t len, const NFSOCK sockIndex) override ;

    virtual bool SendMsgWithOutHead(const int16_t msgID, const char* msg, const size_t len, const NFSOCK sockIndex) override ;

	bool SendMsgToAllClient(const char* msg, const size_t len) override;

    virtual bool SendMsgToAllClientWithOutHead(const int16_t msgID, const char* msg, const size_t len) override ;


    virtual bool CloseKcpObject(const NFUINT32 sockIndex) override ;
    virtual bool AddKcpObject(const NFUINT32 sockIndex, KcpObject* pObject) override ;
    virtual KcpObject* GetKcpObject(const NFUINT32 sockIndex) override ;

    virtual bool IsServer() override ;
    virtual bool Log(int severity, const char* msg) override ;

private:
	bool SendMsgWithOutHead(const int16_t msgID, const char* msg, const size_t len, const std::list<NFSOCK>& fdList);

    bool SendMsg(const char* msg, const size_t len, const std::list<NFSOCK>& fdList);


private:
    void ExecuteClose();
    bool CloseSocketAll();

    bool Dismantle(KcpObject* pObject);


    int InitClientNet();
    int InitServerNet();
    void CloseObject(const NFSOCK sockIndex);


    void log_cb(int severity, const char* msg);
    void event_fatal_cb(int err);
    static void listener_cb(const int sockfd, short int which, void *arg);
    static void recvfrom_cb(const int sockfd, short int which, void *arg);

    
   
protected:
    int DeCode(const char* strData, const uint32_t ulen, NFMsgHead& xHead);
    int EnCode(const uint16_t umsgID, const char* strData, const uint32_t unDataLen, std::string& strOutData);

    void S_HandleAccept(const sockaddr_in* remoteaddr,const char* data ,uint16_t& size);
    void S_HandleConnect(const sockaddr_in* remoteaddr,const char* data ,uint16_t& size);
    void S_HandlePing(const sockaddr_in* remoteaddr,const char* data ,uint16_t& size);
    void S_HandleRecv(const uint32_t& conn,const sockaddr_in* remoteaddr,const char* data ,uint16_t& size);

    KcpObject* C_CreateKcpObject(std::string ip,int32_t port);
    void C_HandleConnect(const char* data,uint16_t& size);
    void C_HandleRecv(const uint32_t& kid,const char* data ,uint16_t& size);


public:
    enum KcpProtoType
    {
        SYN = 1,
        ACK = 2,
        FIN = 3,
        PIN = 4,
    };

    bool mbServer;     //Is a Server or Client
    NFSOCK mSocketFd;

    NFUINT32 IdGenerater = 1000000;       
    std::map<NFUINT32, KcpObject*> mmObject;
    std::map<NFUINT32, KcpObject*> reqkcpObject;   //


    
    int mnMaxConnect;
    std::string mstrIP;
    int mnPort;
    int mnCpuCount;
	

    unsigned int mnBufferSize;

    bool mbWorking;


    int64_t mnSendMsgTotal;
    int64_t mnReceiveMsgTotal;

    struct event_base* mxBase;
    struct event* mxEvent;
    //////////////////////////////////////////////////////////////////////////

    std::function<void(const KcpObject* kobject)> onAcceptObject;
    //KCP_ACCEPT_OBJECT onAcceptObject;
    std::function<void()> onDisconnect;


    KCP_RECEIVE_FUNCTOR mRecvCB;
    KCP_EVENT_FUNCTOR mEventCB;


};

#pragma pack(pop)

#endif
