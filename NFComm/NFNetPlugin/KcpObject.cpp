
#include "KcpObject.h"

#define TIMEOUTPERIOD 1000*60


void KcpObject::S_HandleAccept()
{
    NFUINT32 ack = NFKcp::KcpProtoType::ACK;
    memcpy(chacheBytes,(NFUINT8*)&ack,4);
    memcpy(chacheBytes+4,(NFUINT8*)&Id,4);
    memcpy(chacheBytes+8,(NFUINT8*)&reqCId,4);
    nfKcp->Send(&remoteEp,(char*)chacheBytes,12);
}

void KcpObject::S_HandleRecv(const sockaddr_in* premotesocket,const char* data,const NFUINT16& size)
{
    if(premotesocket->sin_addr.s_addr!=remoteEp.sin_addr.s_addr&&premotesocket->sin_port!=remoteEp.sin_port)
    {
        remoteEp = *premotesocket;
    }
    {
        ikcp_input(mkcp,(const char*)data,size);
    }
}



void KcpObject::S_HandlePing()
{
    lastPingTime = NFGetTimeMS();
}


void KcpObject::S_CheckPing(const IUINT32& nowTime)
{
    if(nowTime-lastPingTime>TIMEOUTPERIOD)
    {
        S_Disconnect();
    }
}

void KcpObject::S_Update(const IUINT32& nowTime)
{
    if(nowTime>=nextTimeCallUpdate)
    {
        ikcp_update(mkcp,nowTime);
        nextTimeCallUpdate = ikcp_check(mkcp,nowTime);
    }
    int hr = ikcp_recv(mkcp,(char*)kcpReceive,sizeof(kcpReceive));
    if(hr>0)
    {
        if(mKcpReceiveCompletedCB)
        {
            mKcpReceiveCompletedCB((const uint8_t*)kcpReceive,hr);
        }
    }
}

void KcpObject::S_Send(const char* data,const NFUINT16& size)
{
    ikcp_send(mkcp,data,size);
}

void KcpObject::S_Disconnect()
{
    isConnected = false;
    if(mKcpOnDisconnectCB)
    {
        mKcpOnDisconnectCB();
    }

    KcpObject *kc = nfKcp->reqkcpObject.find(reqCId)->second;
    if(kc)
    {
        if(kc == this)
        {
            nfKcp->reqkcpObject.erase(reqCId);
        }
    }

    nfKcp->mmObject.erase(Id);
    kc = nfKcp->mmObject.find(Id)->second;
    if(kc)
    {
        if(kc)
        {
            delete kc;

        }
    }
    
}

void  KcpObject::C_HandleConnect(uint32_t& id)
{
    if(this->isConnected){return;}
    this->Id = id;
    mkcp = ikcp_create(Id,(void*) this);
    ikcp_nodelay(mkcp,1,10,2,1);
    ikcp_wndsize(mkcp,512,512);
    ikcp_setmtu(mkcp,1400);
    mkcp->output = [](const char* buf,int len ,struct IKCPCB* kcp,void* user)->int{
        int32_t BytesSent= 0;
        return 0;
    };
    this->isConnected = true;
}

void  KcpObject::C_HandleRecv(const char* data,const NFUINT16& size)
{
    ikcp_input(mkcp,(const char*)data ,size);

}

void  KcpObject::C_Update(const IUINT32& nowTime)
{
    if( nowTime >= nextTimeCallUpdate)
    {
        ikcp_update(mkcp,nowTime);
        nextTimeCallUpdate = ikcp_check(mkcp,nowTime);
    }
    int hr = ikcp_recv(mkcp,(char*) kcpReceive,sizeof(kcpReceive));
    if(hr>0)
    {
        if(mKcpReceiveCompletedCB)
        {
            mKcpReceiveCompletedCB((const uint8_t*)kcpReceive , hr);
        }
    }
}

void  KcpObject::C_Send(const char* data,const NFUINT16& size)
{
    if(this->isConnected)
    {
        ikcp_send(mkcp,data,size);
    }
}

bool KcpObject::Execute()
{
    if(this->mbServer)
    {
        S_Update(NFGetTimeMS());
    }
    else 
    {
       C_Update(NFGetTimeMS());
    }
    
    return true;
}

KcpObject::~KcpObject()
{
}
