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

#ifndef NF_UDP_MODULE_H
#define NF_UDP_MODULE_H

#include <iostream>
#include <event.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/util.h>
#include <event2/thread.h>
#include <event2/event_compat.h>
#include "NFNetModule.h"
#include "NFComm/NFNetPlugin/NFINet.h"
#include "NFComm/NFPluginModule/NFIUdpModule.h"


class NFUdpModule: public NFIUdpModule
{
public:
	NFUdpModule(NFIPluginManager* p);

    virtual ~NFUdpModule();

	virtual bool Init() override ;
	virtual bool AfterInit();

	//as client
	virtual void Initialization() override;

	//as server
	virtual int Initialization(const unsigned int nMaxClient, const unsigned short nPort, const int nCpuCount = 4) override;

	virtual unsigned int ExpandBufferSize(const unsigned int size = 1024 * 1024 * 20) override;

	virtual void RemoveReceiveCallBack(const int msgID);

	virtual bool AddReceiveCallBack(const int msgID, const NET_RECEIVE_FUNCTOR_PTR& cb);

	virtual bool AddReceiveCallBack(const NET_RECEIVE_FUNCTOR_PTR& cb);

	virtual bool AddEventCallBack(const UDP_EVENT_FUNCTOR_PTR& cb);

	virtual bool Execute();


	virtual bool SendMsgWithOutHead(const int msgID, const std::string& msg, const NFSOCK sockIndex);


	virtual bool SendMsgPB(const uint16_t msgID, const google::protobuf::Message& xData, const NFSOCK sockIndex);
	virtual bool SendMsgPB(const uint16_t msgID, const google::protobuf::Message& xData, const NFSOCK sockIndex, const NFGUID id);
	virtual bool SendMsg(const uint16_t msgID, const std::string& xData, const NFSOCK sockIndex);
	virtual bool SendMsg(const uint16_t msgID, const std::string& xData, const NFSOCK sockIndex, const NFGUID id);


	virtual bool SendMsgPB(const uint16_t msgID, const google::protobuf::Message& xData, const NFSOCK sockIndex, const std::vector<NFGUID>* pClientIDList);
	virtual bool SendMsgPB(const uint16_t msgID, const std::string& strData, const NFSOCK sockIndex,  const std::vector<NFGUID>* pClientIDList);

	virtual NFIUdp* GetUdp();

protected:
	void OnReceiveUdpPack(const NFSOCK sockIndex, const int msgID, const char* msg, const uint32_t len);

	void OnSocketUdpEvent(const NFSOCK sockIndex, const NF_NET_EVENT eEvent, NFIUdp* pUdp);


private:
	NFIUdp* m_pUdp;

	unsigned int mnBufferSize;
	std::map<int, std::list<UDP_RECEIVE_FUNCTOR_PTR>> mxReceiveCallBack;
	std::list<UDP_EVENT_FUNCTOR_PTR> mxEventCallBackList;
	std::list<UDP_RECEIVE_FUNCTOR_PTR> mxCallBackList;

	NFILogModule* m_pLogModule;
};

#endif
