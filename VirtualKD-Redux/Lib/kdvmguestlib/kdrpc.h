/*! \file kdrpc.h
    \author Ivan Shcherbakov (Bazis)
    $Id: kdrpc.h,v 1.6 2009-06-07 10:08:59 Administrator Exp $
    \brief Contains definitions for \ref kdrpc_proto "KdXXX() marshalling protocol"
*/

#pragma once

#include "vkdversion.h"

//! \ref guestrpc command name for VirtualKD requests
static char g_szRPCCommandHeader[] = "~kdVMvA ";
static char g_szRPCReplySignature[] = "++kdVMvA ";

//This feature allows skipping some KdCheckForAnyPacket calls without calling guest-to-host channel.
//It can be used to reduce CPU usage. However, supporting this feature implies changing the protocol
//and making the host part (kdclient.dll) incompatible with older versions of KDVM.DLl
//As this feature really does not improve a lot, we keep it disabled until the KDRPC protocol
//changes due to other reasons.
//#define VKD_EXPERIMENTAL_PACKET_POLL_DIVIDER_SUPPORT

//! Specifies possible \ref guestrpc request subtypes for \ref kdrpc_proto "KDRPC protocol"
enum RpcCommand
{
    //! Test connection with KDCLIENT.DLL. Can also be used for benchmarking.
    TestConnection = 't',
    //! Executes KdReceivePacket() on host side
    ReceivePacket = 'r',
    //! Executes KdSendPacket() on host side
    SendPacket = 's',
    //! Reports KDVM.DLL protocol version and requests KDCLIENT.DLL protocol version
    VersionReport = 'v',
};

#ifdef VKD_EXPERIMENTAL_PACKET_POLL_DIVIDER_SUPPORT
#undef VIRTUALKD_REDUX_VER_INT
#define VIRTUALKD_REDUX_VER_INT         0x250
#endif

enum KDRPCGlobalFlags
{
    kKDDebuggerNotPresentSet = 0x01,
    kRetryKdSendPacket = 0x02,
    kKDDebuggerEnabledValueAvailable = 0x04,	//Value is sent in the 2nd byte (0xFF00 mask)
};

/*!
    \page kdrpc_proto KDRPC protocol version A
    Each KDRPC request is executed via \ref guestrpc. A handler is invoked inside
    KDCLIENT.DLL running in VMWARE-VMX.EXE address space.<br/>
    Request format: [header][command][body]<br/>
    Reply format: [signature][specific info]<br/>

    \section kdrpc_cmds Commands
    The following commands are supported by KDRPC:
    \subsection kdrpc_t Connection test
    Body: char array of KDRPC_TEST_BUFFER_SIZE, arr[i] = i & 0xFF;<br/>
    Reply: exact array filled by rule arr[i] = (i ^ 0x55) & 0xFF;<br/>

    \subsection kdrpc_r Receive Packet
    Body:  [packet type:ULONG][globals:4][1st data:SendableKdBuffer][2nd data:SendableKdBuffer][context:KD_CONTEXT]<br/>
    Reply: ['r'][1st data:SendableKdBuffer][2nd data:SendableKdBuffer][context:KD_CONTEXT][code:4][PayloadBytes:4][size1:4][size2:4][globals:4][data]<br/>

    \subsection kdrpc_s Send Packet
    Body:  [1st data:SendableKdBuffer][2nd data:SendableKdBuffer][context:KD_CONTEXT][packet type:4][size1:4][size2:4][globals:4][data]<br/>
    Reply: ['s'][context:KD_CONTEXT]<br/>

    \subsection kdrpc_v Report version
    Body:  [KDVM.DLL protocol version:4]<br/>
    Reply: [KDCLIENT.DLL protocol version:4]<br/>
*/

//! Specifies buffer size for RpcCommand::TestConnection requests
enum { KDRPC_TEST_BUFFER_SIZE = 512 };

//! Specifies number of additional integer parameters used by \ref kdrpc_proto "KDRPC protocol"

#ifdef VKD_EXPERIMENTAL_PACKET_POLL_DIVIDER_SUPPORT
enum { KDRPC_RECV_RETURNED_ULONGS = 6, KDRPC_SEND_PASSED_ULONGS = 4 };
#else
enum { KDRPC_RECV_RETURNED_ULONGS = 5, KDRPC_SEND_PASSED_ULONGS = 4 };
#endif