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

#include <string.h>
#include <atomic>

#include "NFUdp.h"

// #if NF_PLATFORM == NF_PLATFORM_WIN
// #include <WS2tcpip.h>
// #include <winsock2.h>
// #else
#include "NFComm/NFCore/NFException.hpp"

// #if NF_PLATFORM == NF_PLATFORM_APPLE
// #include <arpa/inet.h>
// #endif

// #endif

#include "event2/event.h"
#include "event2/bufferevent_struct.h"

/*
Any one who want to upgrade the networking library(libEvent), please change the size of evbuffer showed below:
*MODIFY--libevent/buffer.c
#define EVBUFFER_MAX_READ	4096
TO
#define EVBUFFER_MAX_READ	65536
*/


#define BUF_SIZE            14500



void NFUdp::listener_cb(const int sock, short int which, void *arg)  //Server
{
	NFUdp* pUdp = (NFUdp*)arg;


	struct sockaddr_in client_addr;
	socklen_t size = sizeof(client_addr);
	char buf[BUF_SIZE];
	std::string  data(buf);
	std::cout << std::this_thread::get_id() << " received:" << data.length() << std::endl;

	/* Recv the data, store the address of the sender in server_sin */
	if (recvfrom(sock, &buf, sizeof(buf) - 1, 0, (struct sockaddr *) &client_addr, &size) == -1)
	{
		perror("recvfrom()");
		//event_loopbreak();
	}

	/* Send the data back to the client */
	// if (sendto(sock, data.c_str(), data.length(), 0, (struct sockaddr *) &client_addr, size) == -1 )
	// {
	// 	perror("sendto()");
	// 	//event_loopbreak();
	// }

    // if (data.length() > 0)
    // {
    //     if (pUdp->mRecvCB)
    //     {
    //         //pUdp->mRecvCB(pObject->GetRealFD(), -1, pObject->GetBuff(), data.length());
    //         pUdp->mnReceiveMsgTotal++;
    //     }  
    // }   
        
}


void NFUdp::recvfrom_cb(const int sock, short int which, void *arg)   //client
{
    UdpObject* pObject = (UdpObject*)arg;
    if (!pObject)
    {
        return;
    }

    NFUdp* pUdp = (NFUdp*)pObject->GetUdp();
    if (!pUdp)
    {
        return;
    }

    struct sockaddr_in client_addr;
	socklen_t size = sizeof(client_addr);
	char buf[BUF_SIZE];
	std::string  data(buf);
	std::cout << std::this_thread::get_id() << " received:" << data.length() << std::endl;

	/* Recv the data, store the address of the sender in server_sin */
	if (recvfrom(sock, &buf, sizeof(buf) - 1, 0, (struct sockaddr *) &client_addr, &size) == -1)
	{
		perror("recvfrom()");
		//event_loopbreak();
	}

    if (data.length() > 0)
    {
        if (pUdp->mRecvCB)
        {
            pUdp->mRecvCB(pObject->GetRealFD(), -1, data.data(), data.length());
            pUdp->mnReceiveMsgTotal++;
        }  
    }   
}

//////////////////////////////////////////////////////////////////////////

bool NFUdp::Execute()
{
    ExecuteClose();

    if (mxBase)
    {
        event_base_loop(mxBase, EVLOOP_ONCE | EVLOOP_NONBLOCK);
    }

    return true;
}


void NFUdp::Initialization()
{
    mbServer = false;
    InitClientNet();
}

int NFUdp::Initialization(const unsigned int nMaxClient, const unsigned short nPort, const int nCpuCount)
{
    mnMaxConnect = nMaxClient;
    mnPort = nPort;
    mnCpuCount = nCpuCount;
    mbServer = true;
    return InitServerNet();
}

unsigned int NFUdp::ExpandBufferSize(const unsigned int size)
{
	if (size > 0)
	{
		mnBufferSize = size;
	}
	return mnBufferSize;
}

bool NFUdp::Final()
{



    if (mxBase)
    {
        event_base_free(mxBase);
        mxBase = NULL;
    }

    return true;
}



bool NFUdp::SendMsg(const char* msg, const size_t len, const NFSOCK sockIndex)
{
    if (len <= 0)
    {
        return false;
    }

	if (!mbWorking)
	{
		return false;
	}
    return false;
}

bool NFUdp::SendMsg(const char* msg, const size_t len, const std::list<NFSOCK>& fdList)
{
	auto it = fdList.begin();
    for (; it != fdList.end(); ++it)
    {
        SendMsg(msg, len, *it);
    }

    return true;
}



bool NFUdp::Dismantle(NetObject* pObject)
{
    bool bNeedDismantle = false;

    int len = pObject->GetBuffLen();
    if (len > NFIMsgHead::NF_Head::NF_HEAD_LENGTH)
    {
        NFMsgHead xHead;
        int nMsgBodyLength = DeCode(pObject->GetBuff(), len, xHead);
        if (nMsgBodyLength > 0 && xHead.GetMsgID() > 0)
        {
            if (mRecvCB)
            {

#if NF_PLATFORM != NF_PLATFORM_WIN
                try
                {
#endif
                
                    mRecvCB(pObject->GetRealFD(), xHead.GetMsgID(), pObject->GetBuff() + NFIMsgHead::NF_Head::NF_HEAD_LENGTH, nMsgBodyLength);
                
#if NF_PLATFORM != NF_PLATFORM_WIN
                }
                catch (const std::exception & e)
                {
                    NFException::StackTrace(xHead.GetMsgID());
                }
                catch (...)
                {
                    NFException::StackTrace(xHead.GetMsgID());
                }
#endif

                mnReceiveMsgTotal++;
            }

			pObject->RemoveBuff(0, nMsgBodyLength + NFIMsgHead::NF_Head::NF_HEAD_LENGTH);

            bNeedDismantle = true;
        }
        else if (0 == nMsgBodyLength)
        {
            

            bNeedDismantle = false;
        }
        else
        {
            
            //pObject->IncreaseError();

            bNeedDismantle = false;

        }
    }

    return bNeedDismantle;
}

bool NFUdp::AddUdpObject(const NFSOCK sockIndex, UdpObject* pObject)
{
    //lock
    return mmObject.insert(std::map<NFSOCK, UdpObject*>::value_type(sockIndex, pObject)).second;
}

int NFUdp::InitClientNet()
{
    std::string ip = mstrIP;
    int nPort = mnPort;

    struct sockaddr_in addr;
    struct bufferevent* bev = NULL;

/* Init. event */
	mxBase = event_init();
	if (mxBase == NULL)
	{
		printf("event_init() failed\n");
		return -1;
	}


    int                 sock_fd;
	int                 flag = 1;
	sock_fd = socket(AF_INET, SOCK_DGRAM, 0);  //SOCK_DGRAM,when udp
	if (sock_fd < 0)
	{
		perror("socket()");
		return -1;
	}

    UdpObject* pObject = new UdpObject(this, sock_fd, addr, bev);
    if (!AddUdpObject(0, pObject))
    {
        assert(0);
        return -1;
    }


	event_set(&mxEvent, sock_fd, EV_READ | EV_PERSIST, &recvfrom_cb, (void*)pObject);
	if (event_add(&mxEvent, NULL) == -1)
	{
		printf("event_add() failed\n");
	}
    return sock_fd;
}

int NFUdp::InitServerNet()
{
    int nCpuCount = mnCpuCount;
    int nPort = mnPort;

    struct sockaddr_in sin;

/* Init. event */
	mxBase = event_init();
	if (mxBase == NULL)
	{
		printf("event_init() failed\n");
		return -1;
	}


    int                 sock_fd;
	int                 flag = 1;
	sock_fd = socket(AF_INET, SOCK_DGRAM, 0);  //SOCK_DGRAM,when udp
	if (sock_fd < 0)
	{
		perror("socket()");
		return -1;
	}

	if (setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(int)) < 0)
	{
		perror("setsockopt()");
		return 1;
	}

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;
	sin.sin_port = htons(nPort);

	if (::bind(sock_fd, (struct sockaddr *)&sin, sizeof(sin)) < 0)
	{
		perror("bind()");
		return -1;
	}
	else
	{
		printf("bind() success - [%u]\n", nPort);
	}

	event_set(&mxEvent, sock_fd, EV_READ | EV_PERSIST, &listener_cb, this);
	if (event_add(&mxEvent, NULL) == -1)
	{
		printf("event_add() failed\n");
	}
    return mnMaxConnect;
}


void NFUdp::ExecuteClose()
{
  
}

void NFUdp::log_cb(int severity, const char* msg)
{
    //LOG(FATAL) << "severity:" << severity << " " << msg; 
}

bool NFUdp::IsServer()
{
    return mbServer;
}

bool NFUdp::Log(int severity, const char* msg)
{
    log_cb(severity, msg);
    return true;
}

bool NFUdp::SendMsgWithOutHead(const int16_t msgID, const char* msg, const size_t len, const NFSOCK sockIndex /*= 0*/)
{
    std::string strOutData;
    int nAllLen = EnCode(msgID, msg, len, strOutData);
    if (nAllLen == len + NFIMsgHead::NF_Head::NF_HEAD_LENGTH)
    {
        
        return SendMsg(strOutData.c_str(), strOutData.length(), sockIndex);
    }

    return false;
}

bool NFUdp::SendMsgWithOutHead(const int16_t msgID, const char* msg, const size_t len, const std::list<NFSOCK>& fdList)
{
    std::string strOutData;
    int nAllLen = EnCode(msgID, msg, len, strOutData);
    if (nAllLen == len + NFIMsgHead::NF_Head::NF_HEAD_LENGTH)
    {
        return SendMsg(strOutData.c_str(), strOutData.length(), fdList);
    }

    return false;
}


int NFUdp::EnCode(const uint16_t umsgID, const char* strData, const uint32_t unDataLen, std::string& strOutData)
{
    NFMsgHead xHead;
    xHead.SetMsgID(umsgID);
    xHead.SetBodyLength(unDataLen);

    char szHead[NFIMsgHead::NF_Head::NF_HEAD_LENGTH] = { 0 };
    xHead.EnCode(szHead);

    strOutData.clear();
    strOutData.append(szHead, NFIMsgHead::NF_Head::NF_HEAD_LENGTH);
    strOutData.append(strData, unDataLen);

    return xHead.GetBodyLength() + NFIMsgHead::NF_Head::NF_HEAD_LENGTH;
}

int NFUdp::DeCode(const char* strData, const uint32_t unAllLen, NFMsgHead& xHead)
{
    
    if (unAllLen < NFIMsgHead::NF_Head::NF_HEAD_LENGTH)
    {
        
        return -1;
    }

    if (NFIMsgHead::NF_Head::NF_HEAD_LENGTH != xHead.DeCode(strData))
    {
        
        return -2;
    }

    if (xHead.GetBodyLength() > (unAllLen - NFIMsgHead::NF_Head::NF_HEAD_LENGTH))
    {
        
        return -3;
    }
    
    return xHead.GetBodyLength();
}
