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

// static void udp_cb(const int sock, short int which, void *arg)
// {
// 	NFUdpModule* udpModule = (NFUdpModule*)arg;


// 	struct sockaddr_in client_addr;
// 	socklen_t size = sizeof(client_addr);
// 	char buf[BUF_SIZE];
// 	std::string  data(buf);
// 	std::cout << std::this_thread::get_id() << " received:" << data.length() << std::endl;

// 	/* Recv the data, store the address of the sender in server_sin */
// 	if (recvfrom(sock, &buf, sizeof(buf) - 1, 0, (struct sockaddr *) &client_addr, &size) == -1)
// 	{
// 		perror("recvfrom()");
// 		//event_loopbreak();
// 	}

// 	/* Send the data back to the client */
// 	if (sendto(sock, data.c_str(), data.length(), 0, (struct sockaddr *) &client_addr, size) == -1 )
// 	{
// 		perror("sendto()");
// 		//event_loopbreak();
// 	}
// }

// int bind_socket(struct event *ev, int port, void* p)
// {
// 	int                 sock_fd;
// 	int                 flag = 1;
// 	struct sockaddr_in  sin;
// 	sock_fd = socket(AF_INET, SOCK_DGRAM, 0);  //SOCK_DGRAM,when udp
// 	if (sock_fd < 0)
// 	{
// 		perror("socket()");
// 		return -1;
// 	}

// 	if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int)) < 0)
// 	{
// 		perror("setsockopt()");
// 		return 1;
// 	}

// 	memset(&sin, 0, sizeof(sin));
// 	sin.sin_family = AF_INET;
// 	sin.sin_addr.s_addr = INADDR_ANY;
// 	sin.sin_port = htons(port);

// 	if (::bind(sock_fd, (struct sockaddr *)&sin, sizeof(sin)) < 0)
// 	{
// 		perror("bind()");
// 		return -1;
// 	}
// 	else
// 	{
// 		printf("bind() success - [%u]\n", port);
// 	}

// 	event_set(ev, sock_fd, EV_READ | EV_PERSIST, &udp_cb, p);
// 	if (event_add(ev, NULL) == -1)
// 	{
// 		printf("event_add() failed\n");
// 	}

// 	return 0;
// }

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
	return true;
}

bool NFUdpModule::SendMsgToAllClientWithOutHead(const int msgID, const std::string &msg)
{
	return true;
}

bool NFUdpModule::SendMsgPB(const uint16_t msgID, const google::protobuf::Message &xData, const NFSOCK sockIndex)
{
	return true;
}

bool NFUdpModule::SendMsgPB(const uint16_t msgID, const google::protobuf::Message &xData, const NFSOCK sockIndex,const NFGUID id)
{
	return true;
}

bool NFUdpModule::SendMsg(const uint16_t msgID, const std::string &xData, const NFSOCK sockIndex)
{
	return true;
}

bool NFUdpModule::SendMsg(const uint16_t msgID, const std::string &xData, const NFSOCK sockIndex, const NFGUID id)
{
	return true;
}

bool NFUdpModule::SendMsgPBToAllClient(const uint16_t msgID, const google::protobuf::Message &xData)
{
	return true;
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
