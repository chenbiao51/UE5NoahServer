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
	m_pLogModule = pPluginManager->FindModule<NFILogModule>();
	return true;
}

bool NFUdpModule::AfterInit()
{

	return true;
}

void NFUdpModule::Initialization()  //client
{
	m_pUdp = NF_NEW NFUdp(this, &NFUdpModule::OnReceiveUdpPack, &NFUdpModule::OnSocketUdpEvent);
    m_pUdp->ExpandBufferSize(mnBufferSize);
    m_pUdp->Initialization();

}

int NFUdpModule::Initialization(const unsigned int nMaxClient, const unsigned short nPort, const int nCpuCount)
{
	m_pUdp = NF_NEW NFUdp(this, &NFUdpModule::OnReceiveUdpPack, &NFUdpModule::OnSocketUdpEvent);
    m_pUdp->ExpandBufferSize(mnBufferSize);
    return m_pUdp->Initialization(nMaxClient, nPort, nCpuCount);

	
}

unsigned int NFUdpModule::ExpandBufferSize(const unsigned int size)
{
	return 0;
}

void NFUdpModule::RemoveReceiveCallBack(const int msgID)
{
	std::map<int, std::list<NET_RECEIVE_FUNCTOR_PTR>>::iterator it = mxReceiveCallBack.find(msgID);
    if (mxReceiveCallBack.end() != it)
    {
        mxReceiveCallBack.erase(it);
    }
}

bool NFUdpModule::AddReceiveCallBack(const int msgID, const NET_RECEIVE_FUNCTOR_PTR &cb)
{
	if (mxReceiveCallBack.find(msgID) == mxReceiveCallBack.end())
    {
		std::list<NET_RECEIVE_FUNCTOR_PTR> xList;
		xList.push_back(cb);
		mxReceiveCallBack.insert(std::map<int, std::list<NET_RECEIVE_FUNCTOR_PTR>>::value_type(msgID, xList));
        return true;
    }

	std::map<int, std::list<NET_RECEIVE_FUNCTOR_PTR>>::iterator it = mxReceiveCallBack.find(msgID);
	it->second.push_back(cb);

    return true;
}

bool NFUdpModule::AddReceiveCallBack(const NET_RECEIVE_FUNCTOR_PTR &cb)
{
	mxCallBackList.push_back(cb);
	return true;
}

bool NFUdpModule::AddEventCallBack(const UDP_EVENT_FUNCTOR_PTR &cb)
{
	mxEventCallBackList.push_back(cb);

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



void NFUdpModule::OnReceiveUdpPack(const NFSOCK sockIndex, const int msgID, const char* msg, const uint32_t len)
{
	m_pLogModule->LogInfo(pPluginManager->GetAppName() + std::to_string(pPluginManager->GetAppID()) + " NFNetModule::OnReceiveNetPack " + std::to_string(msgID), __FILE__, __LINE__);

	NFPerformance performance;

#if NF_PLATFORM != NF_PLATFORM_WIN
	NF_CRASH_TRY
#endif

    std::map<int, std::list<UDP_RECEIVE_FUNCTOR_PTR>>::iterator it = mxReceiveCallBack.find(msgID);
    if (mxReceiveCallBack.end() != it)
    {
		std::list<UDP_RECEIVE_FUNCTOR_PTR>& xFunList = it->second;
		for (std::list<UDP_RECEIVE_FUNCTOR_PTR>::iterator itList = xFunList.begin(); itList != xFunList.end(); ++itList)
		{
			UDP_RECEIVE_FUNCTOR_PTR& pFunPtr = *itList;
			UDP_RECEIVE_FUNCTOR* pFunc = pFunPtr.get();

			pFunc->operator()(sockIndex, msgID, msg, len);
		}
    } 
	else
    {
        for (std::list<UDP_RECEIVE_FUNCTOR_PTR>::iterator itList = mxCallBackList.begin(); itList != mxCallBackList.end(); ++itList)
        {
            UDP_RECEIVE_FUNCTOR_PTR& pFunPtr = *itList;
            UDP_RECEIVE_FUNCTOR* pFunc = pFunPtr.get();

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

void NFUdpModule::OnSocketUdpEvent(const NFSOCK sockIndex, const NF_NET_EVENT eEvent, NFIUdp* pUdp)
{
    for (std::list<UDP_EVENT_FUNCTOR_PTR>::iterator it = mxEventCallBackList.begin();
         it != mxEventCallBackList.end(); ++it)
    {
        UDP_EVENT_FUNCTOR_PTR& pFunPtr = *it;
        UDP_EVENT_FUNCTOR* pFunc = pFunPtr.get();
        pFunc->operator()(sockIndex, eEvent, pUdp);
    }
}
