#pragma once

#define VIRTUALKD_SERVICE_PROTOCOL_PIPE_SUFFIX	_T("_virtualkd_svc_100")

/*
    Service command protocol, version 1.00:
        - A client-to-server message consists of a ServiceProtocolPacket structure
        - Server replies with ServiceProtocolReply
        - Commands have no parameters
*/

enum ServiceProtocolCommand
{
    kServiceProtocolCommandBase = 0x50530000,
    kInstantBreak,
    kRevertToLastSnapshot,
};

struct ServiceProtocolPacket
{
    ServiceProtocolCommand Command;
};

struct ServiceProtocolReply
{
    HRESULT Status;
};