#pragma once

#include "rpcdispatch/rpcdisp.h"
#include "patchapi.h"

class IKDClient
{
public:
    virtual unsigned OnRawRequest(const char *pRequestIncludingRpcHeader, unsigned RequestSizeWithRpcHeader, char **ppReply) = 0;
    virtual ~IKDClient() {}

    enum { InterfaceVersion = 3 };
};

class KDClient : public IKDClient
{
private:
    KdRpcDispatcher m_Dispatcher;
    VMType m_VMType;

public:
    KDClient(const TCHAR *pPipeName, VMType vmType);
    ~KDClient();

    virtual unsigned OnRawRequest(const char *pRequestIncludingRpcHeader, unsigned RequestSizeWithRpcHeader, char **ppReply) override
    {
        const size_t headerSize = sizeof(g_szRPCCommandHeader) - 1;
        ASSERT(!memcmp(pRequestIncludingRpcHeader, g_szRPCCommandHeader, headerSize));
        return m_Dispatcher.OnRequest(pRequestIncludingRpcHeader + headerSize, RequestSizeWithRpcHeader - headerSize, ppReply);
    }

    unsigned OnRequest(const char *pRequest, unsigned RequestSize, char **ppReply)
    {
        return m_Dispatcher.OnRequest(pRequest, RequestSize, ppReply);
    }

public:
    static bool sVMWareDirectHandler(void *pContext, int ChannelNumber, char *pCommandBody, unsigned CommandBodyLength, char **ppReply, unsigned *pReplyLen);

};

