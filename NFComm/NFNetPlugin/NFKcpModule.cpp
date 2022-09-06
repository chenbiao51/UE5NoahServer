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

#include "NFKcpModule.h"
#include "NFComm/NFNetPlugin/NFINet.h"

#define BUF_SIZE                        14500
//500


NFKcpModule::NFKcpModule(NFIPluginManager* p)
{
	pPluginManager = p;

	mnBufferSize = 0;
}

NFKcpModule::~NFKcpModule()
{

}

bool NFKcpModule::Init()
{
	return true;
}

bool NFKcpModule::AfterInit()
{

	return true;
}

void NFKcpModule::Initialization(const char* ip, const unsigned short nPort)
{
    m_pKcp = NF_NEW NFKcp(this, &NFKcpModule::OnReceiveNetPack, &NFKcpModule::OnSocketNetEvent);
    m_pKcp->ExpandBufferSize(mnBufferSize);
    m_pKcp->Initialization(ip, nPort);
}

int NFKcpModule::Initialization(const unsigned int nMaxClient, const unsigned short nPort, const int nCpuCount)
{
    m_pKcp = NF_NEW NFKcp(this, &NFKcpModule::OnReceiveNetPack, &NFKcpModule::OnSocketNetEvent);
    m_pKcp->ExpandBufferSize(mnBufferSize);
    return m_pKcp->Initialization(nMaxClient, nPort, nCpuCount);
}


unsigned int NFKcpModule::ExpandBufferSize(const unsigned int size)
{
	return 0;
}

void NFKcpModule::RemoveReceiveCallBack(const int msgID)
{

}

bool NFKcpModule::AddReceiveCallBack(const int msgID, const NET_RECEIVE_FUNCTOR_PTR &cb)
{
	return true;
}

bool NFKcpModule::AddReceiveCallBack(const NET_RECEIVE_FUNCTOR_PTR &cb)
{
	return true;
}

bool NFKcpModule::AddEventCallBack(const NET_EVENT_FUNCTOR_PTR &cb)
{
	return true;
}

bool NFKcpModule::Execute()
{
	// if (mxBase)
	// {
	// 	event_base_loop(mxBase, EVLOOP_ONCE | EVLOOP_NONBLOCK);
	// }

	return true;
}

bool NFKcpModule::SendMsgWithOutHead(const int msgID, const std::string &msg, const NFSOCK sockIndex)
{
	return true;
}

bool NFKcpModule::SendMsgToAllClientWithOutHead(const int msgID, const std::string &msg)
{
	return true;
}

bool NFKcpModule::SendMsgPB(const uint16_t msgID, const google::protobuf::Message &xData, const NFSOCK sockIndex)
{
	return true;
}

bool NFKcpModule::SendMsgPB(const uint16_t msgID, const google::protobuf::Message &xData, const NFSOCK sockIndex,const NFGUID id)
{
	return true;
}

bool NFKcpModule::SendMsg(const uint16_t msgID, const std::string &xData, const NFSOCK sockIndex)
{
	return true;
}

bool NFKcpModule::SendMsg(const uint16_t msgID, const std::string &xData, const NFSOCK sockIndex, const NFGUID id)
{
	return true;
}

bool NFKcpModule::SendMsgPBToAllClient(const uint16_t msgID, const google::protobuf::Message &xData)
{
	return true;
}

bool NFKcpModule::SendMsgPB(const uint16_t msgID, const google::protobuf::Message &xData, const NFSOCK sockIndex, const std::vector<NFGUID> *pClientIDList)
{
	return true;
}

bool NFKcpModule::SendMsgPB(const uint16_t msgID, const std::string &strData, const NFSOCK sockIndex, const std::vector<NFGUID> *pClientIDList)
{
	return true;
}

NFIKcp *NFKcpModule::GetKcp()
{
	return nullptr;
}

void NFKcpModule::OnReceiveNetPack(const NFSOCK sockIndex, const int msgID, const char *msg, const uint32_t len)
{

}

void NFKcpModule::OnSocketNetEvent(const NFSOCK sockIndex, const NF_NET_EVENT eEvent, NFIKcp *pKcp)
{

}
