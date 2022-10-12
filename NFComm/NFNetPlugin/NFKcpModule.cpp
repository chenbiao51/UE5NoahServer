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
    m_pKcp = NF_NEW NFKcp(this, &NFKcpModule::OnReceiveKcpPack, &NFKcpModule::OnSocketKcpEvent);
    m_pKcp->ExpandBufferSize(mnBufferSize);
    m_pKcp->Initialization(ip, nPort);
}

int NFKcpModule::Initialization(const unsigned int nMaxClient, const unsigned short nPort, const int nCpuCount)
{
    m_pKcp = NF_NEW NFKcp(this, &NFKcpModule::OnReceiveKcpPack, &NFKcpModule::OnSocketKcpEvent);
    m_pKcp->ExpandBufferSize(mnBufferSize);
    return m_pKcp->Initialization(nMaxClient, nPort, nCpuCount);
}


unsigned int NFKcpModule::ExpandBufferSize(const unsigned int size)
{
	if (size > 0)
    {
        mnBufferSize = size;
        if (m_pKcp)
        {
            m_pKcp->ExpandBufferSize(mnBufferSize);
        }
    }
    return mnBufferSize;
}

void NFKcpModule::RemoveReceiveCallBack(const int msgID)
{
	std::map<int, std::list<KCP_RECEIVE_FUNCTOR_PTR>>::iterator it = mxReceiveCallBack.find(msgID);
    if (mxReceiveCallBack.end() != it)
    {
        mxReceiveCallBack.erase(it);
    }
}

bool NFKcpModule::AddReceiveCallBack(const int msgID, const KCP_RECEIVE_FUNCTOR_PTR &cb)
{
	if (mxReceiveCallBack.find(msgID) == mxReceiveCallBack.end())
    {
		std::list<KCP_RECEIVE_FUNCTOR_PTR> xList;
		xList.push_back(cb);
		mxReceiveCallBack.insert(std::map<int, std::list<KCP_RECEIVE_FUNCTOR_PTR>>::value_type(msgID, xList));
        return true;
    }

	std::map<int, std::list<KCP_RECEIVE_FUNCTOR_PTR>>::iterator it = mxReceiveCallBack.find(msgID);
	it->second.push_back(cb);

    return true;
}

bool NFKcpModule::AddReceiveCallBack(const KCP_RECEIVE_FUNCTOR_PTR &cb)
{
	mxCallBackList.push_back(cb);

    return true;
}

bool NFKcpModule::AddEventCallBack(const KCP_EVENT_FUNCTOR_PTR &cb)
{
	mxEventCallBackList.push_back(cb);

    return true;
}

bool NFKcpModule::Execute()
{
	if (!m_pKcp)
    {
        return false;
    }
    KeepAlive();

	m_pKcp->Execute();

	return true;
}

bool NFKcpModule::SendMsgWithOutHead(const int msgID, const std::string &msg, const NFSOCK sockIndex)
{
	bool bRet = m_pKcp->SendMsgWithOutHead(msgID, msg.c_str(), (uint32_t) msg.length(), sockIndex);
	if (!bRet)
	{
		std::ostringstream stream;
		stream << " SendMsgWithOutHead failed fd " << sockIndex;
		stream << " msg id " << msgID;
		m_pLogModule->LogError(stream, __FUNCTION__, __LINE__);
	}

	return bRet;
}

bool NFKcpModule::SendMsgToAllClientWithOutHead(const int msgID, const std::string &msg)
{
	bool bRet = m_pKcp->SendMsgToAllClientWithOutHead(msgID, msg.c_str(), (uint32_t) msg.length());
	if (!bRet)
	{
		std::ostringstream stream;
		stream << " SendMsgToAllClientWithOutHead failed";
		stream << " msg id " << msgID;
		m_pLogModule->LogError(stream, __FUNCTION__, __LINE__);
	}

	return bRet;
}

bool NFKcpModule::SendMsgPB(const uint16_t msgID, const google::protobuf::Message &xData, const NFSOCK sockIndex)
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
	*pPlayerID = NFToPB(NFGUID());

	std::string msg;
	if (!xMsg.SerializeToString(&msg))
	{
		std::ostringstream stream;
		stream << " SendMsgPB Message to  " << sockIndex;
		stream << " Failed For Serialize of MsgBase, MessageID " << msgID;
		m_pLogModule->LogError(stream, __FUNCTION__, __LINE__);

		return false;
	}

	SendMsgWithOutHead(msgID, msg, sockIndex);

	return true;
}

bool NFKcpModule::SendMsgPB(const uint16_t msgID, const google::protobuf::Message &xData, const NFSOCK sockIndex,const NFGUID id)
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

bool NFKcpModule::SendMsg(const uint16_t msgID, const std::string &xData, const NFSOCK sockIndex)
{
	return SendMsgWithOutHead(msgID, xData, sockIndex);
}

bool NFKcpModule::SendMsg(const uint16_t msgID, const std::string &xData, const NFSOCK sockIndex, const NFGUID id)
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

bool NFKcpModule::SendMsgPBToAllClient(const uint16_t msgID, const google::protobuf::Message &xData)
{
	NFMsg::MsgBase xMsg;
    if (!xData.SerializeToString(xMsg.mutable_msg_data()))
    {
		std::ostringstream stream;
		stream << " SendMsgPBToAllClient";
		stream << " Failed For Serialize of MsgData, MessageID " << msgID;
		m_pLogModule->LogError(stream, __FUNCTION__, __LINE__);

        return false;
    }

    std::string msg;
    if (!xMsg.SerializeToString(&msg))
    {
		std::ostringstream stream;
		stream << " SendMsgPBToAllClient";
		stream << " Failed For Serialize of MsgBase, MessageID " << msgID;
		m_pLogModule->LogError(stream, __FUNCTION__, __LINE__);

        return false;
    }

    return SendMsgToAllClientWithOutHead(msgID, msg);
}

bool NFKcpModule::SendMsgPB(const uint16_t msgID, const google::protobuf::Message &xData, const NFSOCK sockIndex, const std::vector<NFGUID> *pClientIDList)
{
	if (!m_pKcp)
    {
		std::ostringstream stream;
		stream << " m_pNet SendMsgPB faailed fd " << sockIndex;
		stream << " Failed For Serialize of MsgBase, MessageID " << msgID;
		m_pLogModule->LogError(stream, __FUNCTION__, __LINE__);

        return false;
    }

    NFMsg::MsgBase xMsg;
    if (!xData.SerializeToString(xMsg.mutable_msg_data()))
    {
		std::ostringstream stream;
		stream << " SendMsgPB faailed fd " << sockIndex;
		stream << " Failed For Serialize of MsgBase, MessageID " << msgID;
		m_pLogModule->LogError(stream, __FUNCTION__, __LINE__);

        return false;
    }


    NFMsg::Ident* pPlayerID = xMsg.mutable_player_id();
    *pPlayerID = NFToPB(NFGUID());
    if (pClientIDList)
    {
        for (int i = 0; i < pClientIDList->size(); ++i)
        {
            const NFGUID& ClientID = (*pClientIDList)[i];

            NFMsg::Ident* pData = xMsg.add_player_client_list();
            if (pData)
            {
                *pData = NFToPB(ClientID);
            }
        }
    }

    std::string msg;
    if (!xMsg.SerializeToString(&msg))
    {
		std::ostringstream stream;
		stream << " SendMsgPB faailed fd " << sockIndex;
		stream << " Failed For Serialize of MsgBase, MessageID " << msgID;
		m_pLogModule->LogError(stream, __FUNCTION__, __LINE__);

        return false;
    }

    return SendMsgWithOutHead(msgID, msg, sockIndex);
}

bool NFKcpModule::SendMsgPB(const uint16_t msgID, const std::string &strData, const NFSOCK sockIndex, const std::vector<NFGUID> *pClientIDList)
{
	if (!m_pKcp)
    {
		std::ostringstream stream;
		stream << " SendMsgPB NULL Of Net faailed fd " << sockIndex;
		stream << " Failed For Serialize of MsgBase, MessageID " << msgID;
		m_pLogModule->LogError(stream, __FUNCTION__, __LINE__);

        return false;
    }

    NFMsg::MsgBase xMsg;
    xMsg.set_msg_data(strData.data(), strData.length());

    NFMsg::Ident* pPlayerID = xMsg.mutable_player_id();
    *pPlayerID = NFToPB(NFGUID());
    if (pClientIDList)
    {
        for (int i = 0; i < pClientIDList->size(); ++i)
        {
            const NFGUID& ClientID = (*pClientIDList)[i];

            NFMsg::Ident* pData = xMsg.add_player_client_list();
            if (pData)
            {
                *pData = NFToPB(ClientID);
            }
        }
    }

    std::string msg;
    if (!xMsg.SerializeToString(&msg))
    {
		std::ostringstream stream;
		stream << " SendMsgPB failed fd " << sockIndex;
		stream << " Failed For Serialize of MsgBase, MessageID " << msgID;
		m_pLogModule->LogError(stream, __FUNCTION__, __LINE__);

        return false;
    }

    return SendMsgWithOutHead(msgID, msg, sockIndex);
}

NFIKcp *NFKcpModule::GetKcp()
{
	return m_pKcp;
}

void NFKcpModule::OnReceiveKcpPack(const NFSOCK sockIndex, const int msgID, const char* msg, const uint32_t len)
{
	//m_pLogModule->LogInfo(pPluginManager->GetAppName() + std::to_string(pPluginManager->GetAppID()) + " NFNetModule::OnReceiveNetPack " + std::to_string(msgID), __FILE__, __LINE__);

	NFPerformance performance;

#if NF_PLATFORM != NF_PLATFORM_WIN
	NF_CRASH_TRY
#endif

    std::map<int, std::list<KCP_RECEIVE_FUNCTOR_PTR>>::iterator it = mxReceiveCallBack.find(msgID);
    if (mxReceiveCallBack.end() != it)
    {
		std::list<KCP_RECEIVE_FUNCTOR_PTR>& xFunList = it->second;
		for (std::list<KCP_RECEIVE_FUNCTOR_PTR>::iterator itList = xFunList.begin(); itList != xFunList.end(); ++itList)
		{
			KCP_RECEIVE_FUNCTOR_PTR& pFunPtr = *itList;
			KCP_RECEIVE_FUNCTOR* pFunc = pFunPtr.get();

			pFunc->operator()(sockIndex, msgID, msg, len);
		}
    } 
	else
    {
        for (std::list<KCP_RECEIVE_FUNCTOR_PTR>::iterator itList = mxCallBackList.begin(); itList != mxCallBackList.end(); ++itList)
        {
            KCP_RECEIVE_FUNCTOR_PTR& pFunPtr = *itList;
            KCP_RECEIVE_FUNCTOR* pFunc = pFunPtr.get();

            pFunc->operator()(sockIndex, msgID, msg, len);
        }
    }

#if NF_PLATFORM != NF_PLATFORM_WIN
	NF_CRASH_END
#endif
/*
	if (performance.CheckTimePoint(5))
	{
		std::ostringstream os;
		os << "---------------net module performance problem------------------- ";
		os << performance.TimeScope();
		os << "---------- MsgID: ";
		os << msgID;
		m_pLogModule->LogWarning(NFGUID(0, msgID), os, __FUNCTION__, __LINE__);
	}
 */
}

void NFKcpModule::OnSocketKcpEvent(const NFSOCK sockIndex, const NF_NET_EVENT eEvent, NFIKcp* pNet)
{
    for (std::list<KCP_EVENT_FUNCTOR_PTR>::iterator it = mxEventCallBackList.begin();
         it != mxEventCallBackList.end(); ++it)
    {
        KCP_EVENT_FUNCTOR_PTR& pFunPtr = *it;
        KCP_EVENT_FUNCTOR* pFunc = pFunPtr.get();
        pFunc->operator()(sockIndex, eEvent, pNet);
    }
}

void NFKcpModule::KeepAlive()
{
    if (!m_pKcp)
    {
        return;
    }

    if (m_pKcp->IsServer())
    {
        return;
    }

    if (nLastTime + 10 > GetPluginManager()->GetNowTime())
    {
        return;
    }

    nLastTime = GetPluginManager()->GetNowTime();

    NFMsg::ServerHeartBeat xMsg;
    xMsg.set_count(0);

    SendMsgPB(NFMsg::EGameMsgID::STS_HEART_BEAT, xMsg, 0);

}