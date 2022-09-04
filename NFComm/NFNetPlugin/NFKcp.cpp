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

#include "NFKcp.h"

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

//1048576 = 1024 * 1024
#define BUF_SIZE            14500

void NFKcp::event_fatal_cb(int err)
{
    //LOG(FATAL) << "event_fatal_cb " << err;

}

void NFKcp::listener_cb(const int sock, short int which, void *arg)  //server
{
	NFKcp* nfkcp = (NFKcp*)arg;


	struct sockaddr_in client_addr;
	socklen_t size = sizeof(client_addr);
	char buf[BUF_SIZE];
	std::string  data(buf);
	std::cout << std::this_thread::get_id() << " received:" << data.length() << std::endl;

	/* Recv the data, store the address of the sender in server_sin */
    uint16_t isize = recvfrom(sock, &buf, sizeof(buf) - 1, 0, (struct sockaddr *) &client_addr, &size);
	if (isize == -1)
	{
		perror("recvfrom()  error");
	}
    else 
    {
        uint32_t conn = *(uint32_t*)buf;
        switch(conn)
        {
            case NFKcp::KcpProtoType::SYN:
                if(isize != 8)
                {
                    break;
                }
                nfkcp->S_HandleAccept( &client_addr,(const char*)buf,isize);
                break;
            case NFKcp::KcpProtoType::ACK:
                if(isize != 12)
                {
                    break;
                }
                nfkcp->S_HandleConnect( &client_addr,(const char*)buf,isize);
                break;
            case NFKcp::KcpProtoType::PIN:
                if(isize != 8)
                {
                    break;
                }
                nfkcp->S_HandlePing(&client_addr,(const char*)buf,isize);
                break;
            default:
                nfkcp->S_HandleRecv(conn, &client_addr,(const char*)buf,isize);
                break;
        }
        isize = 0;
    }
    

	// /* Send the data back to the client */
	// if (sendto(sock, data.c_str(), data.length(), 0, (struct sockaddr *) &client_addr, size) == -1 )
	// {
	// 	perror("sendto()");
		
	// }
}

void NFKcp::recvfrom_cb(const int sock, short int which, void *arg)  //client
{
    NFKcp* nfkcp = (NFKcp*)arg;


	struct sockaddr_in client_addr;
	socklen_t size = sizeof(client_addr);
	char buf[BUF_SIZE];
	std::string  data(buf);
	std::cout << std::this_thread::get_id() << " received:" << data.length() << std::endl;

	/* Recv the data, store the address of the sender in server_sin */
    uint16_t isize = recvfrom(sock, &buf, sizeof(buf) - 1, 0, (struct sockaddr *) &client_addr, &size);
	if (isize == -1)
	{
		perror("recvfrom()  error");
	}
    else 
    {
        uint32_t conn = *(uint32_t*)buf;
        switch(conn)
        {
            case NFKcp::KcpProtoType::SYN:
                if(isize != 8)
                {
                    break;
                }
                //S_HandleAccept((struct sockaddr *) &client_addr,(const char*)buf,isize);
                break;
            case NFKcp::KcpProtoType::ACK:
                if(isize != 12)
                {
                    break;
                }
                nfkcp->C_HandleConnect((const char*)buf,isize);
                break;
            case NFKcp::KcpProtoType::PIN:
                if(isize != 8)
                {
                    break;
                }
                //S_HandlePing((struct sockaddr *) &client_addr,(const char*)buf,isize);
                break;
            default:
                nfkcp->C_HandleRecv(conn,(const char*)buf,isize);
                break;
        }
        isize = 0;
    }
    
}





//////////////////////////////////////////////////////////////////////////

bool NFKcp::Execute()
{
    if(this->mbServer)
    {
        ExecuteClose();
    }
    else 
    {
        
    }
    if (mxBase)
    {
        event_base_loop(mxBase, EVLOOP_ONCE | EVLOOP_NONBLOCK);
    }
    

    return true;
}


void NFKcp::Initialization(const char* ip, const unsigned short nPort)
{
    mstrIP = ip;
    mnPort = nPort;

    InitClientNet();
}

int NFKcp::Initialization(const unsigned int nMaxClient, const unsigned short nPort, const int nCpuCount)
{
    mnMaxConnect = nMaxClient;
    mnPort = nPort;
    mnCpuCount = nCpuCount;

    return InitServerNet();
}

unsigned int NFKcp::ExpandBufferSize(const unsigned int size)
{
	if (size > 0)
	{
		mnBufferSize = size;
	}
	return mnBufferSize;
}

bool NFKcp::Final()
{

    CloseSocketAll();



    if (mxBase)
    {
        event_base_free(mxBase);
        mxBase = NULL;
    }

    return true;
}

bool NFKcp::SendMsgToAllClient(const char* msg, const size_t len)
{
    if (len <= 0)
    {
        return false;
    }

	if (!mbWorking)
	{
		return false;
	}

	auto it = mmObject.begin();
    for (; it != mmObject.end(); ++it)
    {
        NetObject* pNetObject = (NetObject*)it->second;
        if (pNetObject && !pNetObject->NeedRemove())
        {
            bufferevent* bev = (bufferevent*)pNetObject->GetUserData();
            if (NULL != bev)
            {
                bufferevent_write(bev, msg, len);

                mnSendMsgTotal++;
            }
        }
    }

    return true;
}


bool NFKcp::SendMsg(const char* msg, const size_t len, const NFSOCK sockIndex)
{
    if (len <= 0)
    {
        return false;
    }

	if (!mbWorking)
	{
		return false;
	}

	auto it = mmObject.find(sockIndex);
    if (it != mmObject.end())
    {
        NetObject* pNetObject = (NetObject*)it->second;
        if (pNetObject)
        {
            bufferevent* bev = (bufferevent*)pNetObject->GetUserData();
            if (NULL != bev)
            {
                bufferevent_write(bev, msg, len);

                mnSendMsgTotal++;
                return true;
            }
        }
    }

    return false;
}

bool NFKcp::SendMsg(const char* msg, const size_t len, const std::list<NFSOCK>& fdList)
{
	auto it = fdList.begin();
    for (; it != fdList.end(); ++it)
    {
        SendMsg(msg, len, *it);
    }

    return true;
}

bool NFKcp::CloseKcpObject(const NFUINT32 sockIndex)
{
	auto it = mmObject.find(sockIndex);
    if (it != mmObject.end())
    {
        KcpObject* pObject = it->second;



        return true;
    }

    return false;
}

bool NFKcp::Dismantle(KcpObject* pObject)
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

bool NFKcp::AddKcpObject(const NFUINT32 cid, KcpObject* pObject)
{
    //lock
    return mmObject.insert(std::map<NFUINT32, KcpObject*>::value_type(cid, pObject)).second;
}

int NFKcp::InitClientNet()
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
	struct sockaddr_in  sin;
	sock_fd = socket(AF_INET, SOCK_DGRAM, 0);  //SOCK_DGRAM,when udp
	if (sock_fd < 0)
	{
		perror("socket()");
		return -1;
	}



	event_set(mxEvent, sock_fd, EV_READ | EV_PERSIST, &recvfrom_cb, this);
	if (event_add(mxEvent, NULL) == -1)
	{
		printf("event_add() failed\n");
	}


    return sock_fd;
}

int NFKcp::InitServerNet()
{
    int nCpuCount = mnCpuCount;
    int nPort = mnPort;

    struct sockaddr_in sin;

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

	event_set(mxEvent, sock_fd, EV_READ | EV_PERSIST, listener_cb, this);
	if (event_add(mxEvent, NULL) == -1)
	{
		printf("event_add() failed\n");
	}
    return mnMaxConnect;
}

bool NFKcp::CloseSocketAll()
{
	auto it = mmObject.begin();
    for (; it != mmObject.end(); ++it)
    {

    }

    ExecuteClose();

    mmObject.clear();

    return true;
}

KcpObject* NFKcp::GetKcpObject(const NFUINT32 sockIndex)
{
	auto it = mmObject.find(sockIndex);
    if (it != mmObject.end())
    {
        return it->second;
    }

    return NULL;
}

void NFKcp::CloseObject(const NFSOCK sockIndex)
{
	auto it = mmObject.find(sockIndex);
    if (it != mmObject.end())
    {
        KcpObject* pObject = it->second;

        struct bufferevent* bev = (bufferevent*)pObject->GetUserData();

        bufferevent_free(bev);

        mmObject.erase(it);

        delete pObject;
        pObject = NULL;
    }
}

void NFKcp::ExecuteClose()
{
    // for (int i = 0; i < mvRemoveObject.size(); ++i)
    // {
	// 	NFSOCK nSocketIndex = mvRemoveObject[i];
    //     CloseObject(nSocketIndex);
    // }

    //mvRemoveObject.clear();
}

void NFKcp::log_cb(int severity, const char* msg)
{
    //LOG(FATAL) << "severity:" << severity << " " << msg; 
}

bool NFKcp::IsServer()
{
    return mbServer;
}

bool NFKcp::Log(int severity, const char* msg)
{
    log_cb(severity, msg);
    return true;
}

bool NFKcp::SendMsgWithOutHead(const int16_t msgID, const char* msg, const size_t len, const NFSOCK sockIndex /*= 0*/)
{
    std::string strOutData;
    int nAllLen = EnCode(msgID, msg, len, strOutData);
    if (nAllLen == len + NFIMsgHead::NF_Head::NF_HEAD_LENGTH)
    {
        
        return SendMsg(strOutData.c_str(), strOutData.length(), sockIndex);
    }

    return false;
}

bool NFKcp::SendMsgWithOutHead(const int16_t msgID, const char* msg, const size_t len, const std::list<NFSOCK>& fdList)
{
    std::string strOutData;
    int nAllLen = EnCode(msgID, msg, len, strOutData);
    if (nAllLen == len + NFIMsgHead::NF_Head::NF_HEAD_LENGTH)
    {
        return SendMsg(strOutData.c_str(), strOutData.length(), fdList);
    }

    return false;
}

bool NFKcp::SendMsgToAllClientWithOutHead(const int16_t msgID, const char* msg, const size_t len)
{
    std::string strOutData;
    int nAllLen = EnCode(msgID, msg, len, strOutData);
    if (nAllLen == len + NFIMsgHead::NF_Head::NF_HEAD_LENGTH)
    {
        return SendMsgToAllClient(strOutData.c_str(), (uint32_t) strOutData.length());
    }

    return false;
}

int NFKcp::EnCode(const uint16_t umsgID, const char* strData, const uint32_t unDataLen, std::string& strOutData)
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

int NFKcp::DeCode(const char* strData, const uint32_t unAllLen, NFMsgHead& xHead)
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

void  NFKcp::S_HandleAccept(const sockaddr_in* remoteaddr,const char* data ,uint16_t& size)
{
    uint32_t reqConn = *(uint32_t*)(data + 4);
    KcpObject* pkcpObject;
    auto it = reqkcpObject.find(reqConn);
    if (it != reqkcpObject.end())
    {
        pkcpObject = it->second;
        if(pkcpObject->isConnected)
        {
            reqkcpObject.erase(it);
            return;
        }
    }
    else
    {
        NFUINT32 newId;
        do{
            newId = IdGenerater++;
        }
        while (mmObject.find(newId)!=mmObject.end());
        pkcpObject = new KcpObject(newId,reqConn,*remoteaddr, this );
        reqkcpObject.insert(std::map<NFUINT32, KcpObject*>::value_type(reqConn, pkcpObject));
        AddKcpObject(pkcpObject->Id,pkcpObject);
        if(onAcceptObject)
        {
            onAcceptObject(pkcpObject);
        }
    }
    pkcpObject->S_HandleAccept();
    
}

void NFKcp::S_HandleConnect(const sockaddr_in* remoteaddr,const char* data ,uint16_t& size)
{

}

void NFKcp::S_HandlePing(const sockaddr_in* remoteaddr,const char* data ,uint16_t& size)
{
    uint32_t cId = *(uint32_t*)(data + 4);
    auto it = mmObject.find(cId);
    if (it != mmObject.end())
    {
        KcpObject* pkcpObject = it->second;
        pkcpObject->S_HandlePing();
    }
}

void NFKcp::S_HandleRecv(const uint32_t& cid,const sockaddr_in* remoteaddr,const char* data ,uint16_t& size)
{

    auto it = mmObject.find(cid);
    if (it != mmObject.end())
    {
        KcpObject* pkcpObject = it->second;
        pkcpObject->S_HandleRecv(remoteaddr,data,size);
    }
}

 KcpObject* NFKcp::C_CreateKcpObject(std::string ip,int32_t port)
 {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr((char*)ip.data());

    uint32_t coonid = 0;
    bool bc = false;
    do{
        coonid = NFU32Random();
        bc = reqkcpObject.find(coonid)!= reqkcpObject.end();
    }while(bc);
    KcpObject* kcpObject = new KcpObject(coonid,mSocketFd,addr);
    reqkcpObject.insert(std::map<NFUINT32, KcpObject*>::value_type(kcpObject->reqConn, kcpObject));
    return kcpObject;
 }

void NFKcp::C_HandleConnect(const char* data ,uint16_t& size)
{
    NFUINT32 kid = *(NFUINT32*)(data +4);
    NFUINT32 reqConn = *(NFUINT32*)(data + 8);
    auto it = reqkcpObject.find(kid);
    if (it != reqkcpObject.end())
    {
        KcpObject* pkcpObject = it->second;
        pkcpObject->C_HandleConnect(kid);
        AddKcpObject(kid,pkcpObject);
        reqkcpObject.erase(it);
    }
}

void NFKcp::C_HandleRecv(const uint32_t& kid,const char* data ,uint16_t& size)
{
    auto it = mmObject.find(kid);
    if (it != mmObject.end())
    {
        KcpObject* pkcpObject = it->second;
        pkcpObject->C_HandleRecv(data,size);
    }
}