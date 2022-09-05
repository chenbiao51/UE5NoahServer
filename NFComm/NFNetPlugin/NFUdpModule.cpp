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

#include "NFUdpModule.h"
#include "NFComm/NFNetPlugin/NFINet.h"
#include "NFComm/NFNetPlugin/NFUdp.h"

#define BUF_SIZE                        14500
//500
NFUdpModule::NFUdpModule(NFIPluginManager* p)
{
	pPluginManager = p;

	mnBufferSize = 0;
}

NFUdpModule::~NFUdpModule()
{

}

bool NFUdpModule::Init()
{
	return true;
}

bool NFUdpModule::AfterInit()
{

	return true;
}

void NFUdpModule::Initialization()  //client
{
	m_pUdp = NF_NEW NFUdp(this, &NFUdpModule::OnReceiveNetPack, &NFUdpModule::OnSocketNetEvent);
    m_pUdp->ExpandBufferSize(mnBufferSize);
    m_pUdp->Initialization();

}

int NFUdpModule::Initialization(const unsigned int nMaxClient, const unsigned short nPort, const int nCpuCount)
{
	m_pUdp = NF_NEW NFUdp(this, &NFUdpModule::OnReceiveNetPack, &NFUdpModule::OnSocketNetEvent);
    m_pUdp->ExpandBufferSize(mnBufferSize);
    return m_pUdp->Initialization(nMaxClient, nPort, nCpuCount);

	
}

unsigned int NFUdpModule::ExpandBufferSize(const unsigned int size)
{
	return 0;
}

void NFUdpModule::RemoveReceiveCallBack(const int msgID)
{

}

bool NFUdpModule::AddReceiveCallBack(const int msgID, const NET_RECEIVE_FUNCTOR_PTR &cb)
{
	return true;
}

bool NFUdpModule::AddReceiveCallBack(const NET_RECEIVE_FUNCTOR_PTR &cb)
{
	return true;
}

bool NFUdpModule::AddEventCallBack(const UDP_EVENT_FUNCTOR_PTR &cb)
{
	return true;
}

bool NFUdpModule::Execute()
{

	if (!m_pUdp)
    {
        return false;
    }
	m_pUdp->Execute();

	return true;
}

bool NFUdpModule::SendMsgWithOutHead(const int msgID, const std::string &msg, const NFSOCK sockIndex)
{
	bool bRet = m_pUdp->SendMsgWithOutHead(msgID, msg.c_str(), (uint32_t) msg.length(), sockIndex);
	if (!bRet)
	{
		std::ostringstream stream;
		stream << " SendMsgWithOutHead failed fd " << sockIndex;
		stream << " msg id " << msgID;
		m_pLogModule->LogError(stream, __FUNCTION__, __LINE__);
	}

	return bRet;
}


bool NFUdpModule::SendMsgPB(const uint16_t msgID, const google::protobuf::Message &xData, const NFSOCK sockIndex)
{
	return true;
}

bool NFUdpModule::SendMsgPB(const uint16_t msgID, const google::protobuf::Message &xData, const NFSOCK sockIndex,const NFGUID id)
{
    NFMsg::MsgBase xMsg;
    if (!xData.SerializeToString(xMsg.mutable_msg_data()))
    {
		std::ostringstream stream;
		stream << " SendMsgPB Message to  " << sockIndex;
		stream << " Failed For Serialize of MsgData, MessageID " << msgID;
		m_pLogModule->LogError(stream, __FUNCTION__, __LINE__);

        return false;
    }

    NFMsg::Ident* pPlayerID = xMsg.mutable_player_id();
    *pPlayerID = NFToPB(id);

    std::string msg;
    if (!xMsg.SerializeToString(&msg))
    {
		std::ostringstream stream;
		stream << " SendMsgPB Message to  " << sockIndex;
		stream << " Failed For Serialize of MsgBase, MessageID " << msgID;
		m_pLogModule->LogError(stream, __FUNCTION__, __LINE__);

        return false;
    }

	return SendMsgWithOutHead(msgID, msg, sockIndex);
}

bool NFUdpModule::SendMsg(const uint16_t msgID, const std::string &xData, const NFSOCK sockIndex)
{
	return SendMsgWithOutHead(msgID, xData, sockIndex);
}

bool NFUdpModule::SendMsg(const uint16_t msgID, const std::string &xData, const NFSOCK sockIndex, const NFGUID id)
{
	NFMsg::MsgBase xMsg;
	xMsg.set_msg_data(xData.data(), xData.length());

	NFMsg::Ident* pPlayerID = xMsg.mutable_player_id();
	*pPlayerID = NFToPB(id);

	std::string msg;
	if (!xMsg.SerializeToString(&msg))
	{
		std::ostringstream stream;
		stream << " SendMsgPB Message to  " << sockIndex;
		stream << " Failed For Serialize of MsgBase, MessageID " << msgID;
		m_pLogModule->LogError(stream, __FUNCTION__, __LINE__);

		return false;
	}

	return SendMsgWithOutHead(msgID, msg, sockIndex);
}



bool NFUdpModule::SendMsgPB(const uint16_t msgID, const google::protobuf::Message &xData, const NFSOCK sockIndex, const std::vector<NFGUID> *pClientIDList)
{
	return true;
}

bool NFUdpModule::SendMsgPB(const uint16_t msgID, const std::string &strData, const NFSOCK sockIndex, const std::vector<NFGUID> *pClientIDList)
{
	return true;
}

NFIUdp *NFUdpModule::GetUdp()
{
	return nullptr;
}

void NFUdpModule::OnReceiveNetPack(const NFSOCK sockIndex, const int msgID, const char *msg, const uint32_t len)
{

}

void NFUdpModule::OnSocketNetEvent(const NFSOCK sockIndex, const NF_NET_EVENT eEvent, NFIUdp *pUdp)
{

}
