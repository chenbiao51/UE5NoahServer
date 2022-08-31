#ifndef KCP_OBJECT_H
#define KCP_OBJECT_H

#include "ikcp.h"
#include "NFKcp.h"
#include "NFComm/NFPluginModule/NFPlatform.h"



typedef std::function<void(const NFUINT8* data, const NFUINT16& size)> KCP_RECEIVE_COMPLETED_FUNCTOR;
typedef std::shared_ptr<KCP_RECEIVE_COMPLETED_FUNCTOR> KCP_RECEIVE_COMPLETED_FUNCTOR_PTR;

typedef std::function<void()> KCP_ONDISCONNECT_FUNCTOR;
typedef std::shared_ptr<KCP_ONDISCONNECT_FUNCTOR> KCP_ONDISCONNECT_FUNCTOR_PTR;

class KcpObject
{
    ikcpcb* mkcp;
    IUINT32 nextTimeCallUpdate = 0;
    NFUINT8 chacheBytes[12] = {0};
    NFUINT8 kcpReceive[1024*4] = {0};

    IUINT32 lastPingTime = 0;
    
public:
   bool mbServer;     //Is a Server or Client
   bool isConnected = false;
   NFUINT32 Id;
   NFUINT32 reqConn;
   NFKcp* nfKcp;
   sockaddr_in remoteEp;
   NFSOCK mSocketFd;
private:
    /* data */
public:
    KcpObject(uint32_t reqConn,NFSOCK sockfd,const sockaddr_in* remotesocket);
    void C_HandleConnect(uint32_t& id);
    void C_HandleRecv(const char* data,const NFUINT16& size);
    void C_Update(const IUINT32& currenttime);
    void C_Send(const char* data,const NFUINT16& size);

    KcpObject(NFUINT32 newid,NFUINT32 reqconn,sockaddr_in remotesocket,NFKcp* nfkcp);
    void S_HandleAccept();
    void S_HandleRecv(const sockaddr_in* remotesocket,const char* data,const NFUINT16& size);
    void S_HandlePing();
    void S_CheckPing(const IUINT32& currenttime);
    void S_Update(const IUINT32& currenttime);
    void S_Send(const char* data,const NFUINT16& size);
    void S_Disconnect();

    bool Execute();   //Loop
    KCP_RECEIVE_COMPLETED_FUNCTOR mKcpReceiveCompletedCB;
    KCP_ONDISCONNECT_FUNCTOR mKcpOnDisconnectCB;
    ~KcpObject();
};

#endif
