#ifndef KCP_OBJECT_H
#define KCP_OBJECT_H

#include "ikcp.h"
#include "NFKcp.h"
#include "NFNet.h"
#include "NFIKcp.h"
#include "NFComm/NFPluginModule/NFPlatform.h"

class NFIKcp;
class NFKcp;

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
    // KcpObject(uint32_t reqConn,NFSOCK sockfd,const sockaddr_in remotesocket);
    // KcpObject(NFUINT32 newid,NFUINT32 reqconn,sockaddr_in remotesocket,NFKcp* nfkcp);
    KcpObject(uint32_t reqconn,NFSOCK sockfd,const sockaddr_in remotesocket) //Client
    {
        this->mbServer = false;
        this->reqConn = reqconn;
        this->mSocketFd = sockfd;
        this->remoteEp = remotesocket;
    
    }

    template<typename BaseType>
    KcpObject(NFUINT32 newid,NFUINT32 reqconn,sockaddr_in remotesocket,BaseType* nfkcp) //Server
    {
        this->mbServer = true;
        this->Id = newid;
        this->reqConn = reqconn;
        this->remoteEp = remotesocket;
        this->nfKcp = (NFKcp*)nfkcp;

        mkcp = ikcp_create(newid ,(void*)this);
        ikcp_nodelay(mkcp,1,10,2,1);
        ikcp_wndsize(mkcp,512,512);
        ikcp_setmtu(mkcp,1400);
        mkcp->output = [](const char* buf,int len,struct IKCPCB* kcp,void*  user)->int{
            ((KcpObject*)user)->nfKcp->Send(&(((KcpObject*)user)->remoteEp),(char*)buf,len);
            return 0;
        };
        this->lastPingTime = NFGetTimeMS();
        isConnected = true;
    }


   int AddBuff(const char* str, size_t len)
    {
        ringBuff.append(str, len);

        return (int) ringBuff.length();
    }

    int CopyBuffTo(char* str, uint32_t start, uint32_t len)
    {
        if (start + len > ringBuff.length())
        {
            return 0;
        }

        memcpy(str, ringBuff.data() + start, len);

        return len;
    }

    int RemoveBuff(uint32_t start, uint32_t len)
    {
        if (start + len > ringBuff.length())
        {
            return 0;
        }

        ringBuff.erase(start, len);

        return (int) ringBuff.length();
    }

    const char* GetBuff()
    {
        return ringBuff.data();
    }

    int GetBuffLen() const
    {
        return (int) ringBuff.length();
    }

    void* GetUserData()
    {
        return userData;
    }

    NFKcp* GetKcp()
    {
        return nfKcp;
    }

    //////////////////////////////////////////////////////////////////////////
    const std::string& GetSecurityKey() const
    {
        return securityKey;
    }

    void SetSecurityKey(const std::string& key)
    {
		securityKey = key;
    }

    int GetConnectKeyState() const
    {
        return logicState;
    }

    void SetConnectKeyState(const int state)
    {
		logicState = state;
    }

    bool NeedRemove()
    {
        return bNeedRemove;
    }

    void SetNeedRemove(bool b)
    {
        bNeedRemove = b;
    }

    const std::string& GetAccount() const
    {
        return account;
    }

    void SetAccount(const std::string& data)
    {
		account = data;
    }

    int GetGameID() const
    {
        return gameID;
    }

    void SetGameID(const int nData)
    {
		gameID = nData;
    }

    const NFGUID& GetUserID()
    {
        return userID;
    }

    void SetUserID(const NFGUID& nUserID)
    {
		userID = nUserID;
    }

    const NFGUID& GetClientID()
    {
        return clientID;
    }

    void SetClientID(const NFGUID& xClientID)
    {
		clientID = xClientID;
    }

    const NFGUID& GetHashIdentID()
		{
        return hashIdentID;
    }

    void SetHashIdentID(const NFGUID& xHashIdentID)
    {
		hashIdentID = xHashIdentID;
    }

    NFSOCK GetRealFD()
    {
        return mSocketFd;
    }  
    
public:
   bool mbServer;     //Is a Server or Client
   bool isConnected = false;
   NFUINT32 Id;
   NFUINT32 reqConn;
   NFKcp* nfKcp;
   sockaddr_in remoteEp;
   NFSOCK mSocketFd;
private:
    //sockaddr_in sin;
    void* userData;
    //ringbuff
    std::string ringBuff;
    std::string account;
    std::string securityKey;

    int32_t logicState;
    int32_t gameID;
    NFGUID userID;//player id
    NFGUID clientID;//temporary client id
    NFGUID hashIdentID;//hash ident, special for distributed
    //NFIUdp* udpObject;
    //
    //NFSOCK fd;
    bool bNeedRemove;
public:


    void C_HandleConnect(uint32_t& id);
    void C_HandleRecv(const char* data,const NFUINT16& size);
    void C_Update(const IUINT32& currenttime);
    void C_Send(const char* data,const NFUINT16& size);


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
