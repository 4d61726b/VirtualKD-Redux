

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0499 */
/* at Mon Jan 04 21:11:28 2010
 */
/* Compiler settings for d:/build/ob/bora-222403/bora/apps/foundryCOM/VixCOM.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __VixCOM_h__
#define __VixCOM_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IVixLib_FWD_DEFINED__
#define __IVixLib_FWD_DEFINED__
typedef interface IVixLib IVixLib;
#endif 	/* __IVixLib_FWD_DEFINED__ */


#ifndef __IVixHandle_FWD_DEFINED__
#define __IVixHandle_FWD_DEFINED__
typedef interface IVixHandle IVixHandle;
#endif 	/* __IVixHandle_FWD_DEFINED__ */


#ifndef __IVixHandle2_FWD_DEFINED__
#define __IVixHandle2_FWD_DEFINED__
typedef interface IVixHandle2 IVixHandle2;
#endif 	/* __IVixHandle2_FWD_DEFINED__ */


#ifndef __ICallback_FWD_DEFINED__
#define __ICallback_FWD_DEFINED__
typedef interface ICallback ICallback;
#endif 	/* __ICallback_FWD_DEFINED__ */


#ifndef __IJob_FWD_DEFINED__
#define __IJob_FWD_DEFINED__
typedef interface IJob IJob;
#endif 	/* __IJob_FWD_DEFINED__ */


#ifndef __ISnapshot_FWD_DEFINED__
#define __ISnapshot_FWD_DEFINED__
typedef interface ISnapshot ISnapshot;
#endif 	/* __ISnapshot_FWD_DEFINED__ */


#ifndef __IHost_FWD_DEFINED__
#define __IHost_FWD_DEFINED__
typedef interface IHost IHost;
#endif 	/* __IHost_FWD_DEFINED__ */


#ifndef __IHost2_FWD_DEFINED__
#define __IHost2_FWD_DEFINED__
typedef interface IHost2 IHost2;
#endif 	/* __IHost2_FWD_DEFINED__ */


#ifndef __IVM_FWD_DEFINED__
#define __IVM_FWD_DEFINED__
typedef interface IVM IVM;
#endif 	/* __IVM_FWD_DEFINED__ */


#ifndef __IVM2_FWD_DEFINED__
#define __IVM2_FWD_DEFINED__
typedef interface IVM2 IVM2;
#endif 	/* __IVM2_FWD_DEFINED__ */


#ifndef __VixLib_FWD_DEFINED__
#define __VixLib_FWD_DEFINED__

#ifdef __cplusplus
typedef class VixLib VixLib;
#else
typedef struct VixLib VixLib;
#endif /* __cplusplus */

#endif 	/* __VixLib_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

#ifdef __cplusplus
extern "C"{
#endif 


/* interface __MIDL_itf_VixCOM_0000_0000 */
/* [local] */ 

/* **********************************************************
 * Copyright 2006-2008.  VMware, Inc.  The use of this code is subject to
 * your agreement to the terms of an applicable VMware license.
 * **********************************************************/


#pragma once
#pragma once
#pragma once










extern RPC_IF_HANDLE __MIDL_itf_VixCOM_0000_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_VixCOM_0000_0000_v0_0_s_ifspec;


#ifndef __VixCOM_LIBRARY_DEFINED__
#define __VixCOM_LIBRARY_DEFINED__

/* library VixCOM */
/* [helpstring][version][uuid] */ 

#define	VIXCOM_E_VIX_ERROR	( ( HRESULT  )(( ( ( ( unsigned long  )1 << 31 )  | ( ( unsigned long  )4 << 16 )  )  | ( unsigned long  )0x200 ) ) )


EXTERN_C const IID LIBID_VixCOM;


#ifndef __Constants_MODULE_DEFINED__
#define __Constants_MODULE_DEFINED__


/* module Constants */
/* [dllname][uuid] */ 

const LONG VIX_INVALID_HANDLE	=	0;

const LONG VIX_HANDLETYPE_NONE	=	0;

const LONG VIX_HANDLETYPE_HOST	=	2;

const LONG VIX_HANDLETYPE_VM	=	3;

const LONG VIX_HANDLETYPE_NETWORK	=	5;

const LONG VIX_HANDLETYPE_JOB	=	6;

const LONG VIX_HANDLETYPE_SNAPSHOT	=	7;

const LONG VIX_HANDLETYPE_PROPERTY_LIST	=	9;

const LONG VIX_HANDLETYPE_METADATA_CONTAINER	=	11;

const LONG VIX_OK	=	0;

const LONG VIX_E_FAIL	=	1;

const LONG VIX_E_OUT_OF_MEMORY	=	2;

const LONG VIX_E_INVALID_ARG	=	3;

const LONG VIX_E_FILE_NOT_FOUND	=	4;

const LONG VIX_E_OBJECT_IS_BUSY	=	5;

const LONG VIX_E_NOT_SUPPORTED	=	6;

const LONG VIX_E_FILE_ERROR	=	7;

const LONG VIX_E_DISK_FULL	=	8;

const LONG VIX_E_INCORRECT_FILE_TYPE	=	9;

const LONG VIX_E_CANCELLED	=	10;

const LONG VIX_E_FILE_READ_ONLY	=	11;

const LONG VIX_E_FILE_ALREADY_EXISTS	=	12;

const LONG VIX_E_FILE_ACCESS_ERROR	=	13;

const LONG VIX_E_REQUIRES_LARGE_FILES	=	14;

const LONG VIX_E_FILE_ALREADY_LOCKED	=	15;

const LONG VIX_E_VMDB	=	16;

const LONG VIX_E_NOT_SUPPORTED_ON_REMOTE_OBJECT	=	20;

const LONG VIX_E_FILE_TOO_BIG	=	21;

const LONG VIX_E_FILE_NAME_INVALID	=	22;

const LONG VIX_E_ALREADY_EXISTS	=	23;

const LONG VIX_E_BUFFER_TOOSMALL	=	24;

const LONG VIX_E_OBJECT_NOT_FOUND	=	25;

const LONG VIX_E_HOST_NOT_CONNECTED	=	26;

const LONG VIX_E_INVALID_UTF8_STRING	=	27;

const LONG VIX_E_OPERATION_ALREADY_IN_PROGRESS	=	31;

const LONG VIX_E_UNFINISHED_JOB	=	29;

const LONG VIX_E_NEED_KEY	=	30;

const LONG VIX_E_LICENSE	=	32;

const LONG VIX_E_VM_HOST_DISCONNECTED	=	34;

const LONG VIX_E_AUTHENTICATION_FAIL	=	35;

const LONG VIX_E_HOST_CONNECTION_LOST	=	36;

const LONG VIX_E_INVALID_HANDLE	=	1000;

const LONG VIX_E_NOT_SUPPORTED_ON_HANDLE_TYPE	=	1001;

const LONG VIX_E_TOO_MANY_HANDLES	=	1002;

const LONG VIX_E_NOT_FOUND	=	2000;

const LONG VIX_E_TYPE_MISMATCH	=	2001;

const LONG VIX_E_INVALID_XML	=	2002;

const LONG VIX_E_TIMEOUT_WAITING_FOR_TOOLS	=	3000;

const LONG VIX_E_UNRECOGNIZED_COMMAND	=	3001;

const LONG VIX_E_OP_NOT_SUPPORTED_ON_GUEST	=	3003;

const LONG VIX_E_PROGRAM_NOT_STARTED	=	3004;

const LONG VIX_E_CANNOT_START_READ_ONLY_VM	=	3005;

const LONG VIX_E_VM_NOT_RUNNING	=	3006;

const LONG VIX_E_VM_IS_RUNNING	=	3007;

const LONG VIX_E_CANNOT_CONNECT_TO_VM	=	3008;

const LONG VIX_E_POWEROP_SCRIPTS_NOT_AVAILABLE	=	3009;

const LONG VIX_E_NO_GUEST_OS_INSTALLED	=	3010;

const LONG VIX_E_VM_INSUFFICIENT_HOST_MEMORY	=	3011;

const LONG VIX_E_SUSPEND_ERROR	=	3012;

const LONG VIX_E_VM_NOT_ENOUGH_CPUS	=	3013;

const LONG VIX_E_HOST_USER_PERMISSIONS	=	3014;

const LONG VIX_E_GUEST_USER_PERMISSIONS	=	3015;

const LONG VIX_E_TOOLS_NOT_RUNNING	=	3016;

const LONG VIX_E_GUEST_OPERATIONS_PROHIBITED	=	3017;

const LONG VIX_E_ANON_GUEST_OPERATIONS_PROHIBITED	=	3018;

const LONG VIX_E_ROOT_GUEST_OPERATIONS_PROHIBITED	=	3019;

const LONG VIX_E_MISSING_ANON_GUEST_ACCOUNT	=	3023;

const LONG VIX_E_CANNOT_AUTHENTICATE_WITH_GUEST	=	3024;

const LONG VIX_E_UNRECOGNIZED_COMMAND_IN_GUEST	=	3025;

const LONG VIX_E_CONSOLE_GUEST_OPERATIONS_PROHIBITED	=	3026;

const LONG VIX_E_MUST_BE_CONSOLE_USER	=	3027;

const LONG VIX_E_VMX_MSG_DIALOG_AND_NO_UI	=	3028;

const LONG VIX_E_NOT_ALLOWED_DURING_VM_RECORDING	=	3029;

const LONG VIX_E_NOT_ALLOWED_DURING_VM_REPLAY	=	3030;

const LONG VIX_E_OPERATION_NOT_ALLOWED_FOR_LOGIN_TYPE	=	3031;

const LONG VIX_E_LOGIN_TYPE_NOT_SUPPORTED	=	3032;

const LONG VIX_E_EMPTY_PASSWORD_NOT_ALLOWED_IN_GUEST	=	3033;

const LONG VIX_E_INTERACTIVE_SESSION_NOT_PRESENT	=	3034;

const LONG VIX_E_INTERACTIVE_SESSION_USER_MISMATCH	=	3035;

const LONG VIX_E_UNABLE_TO_REPLAY_VM	=	3039;

const LONG VIX_E_CANNOT_POWER_ON_VM	=	3041;

const LONG VIX_E_NO_DISPLAY_SERVER	=	3043;

const LONG VIX_E_VM_NOT_RECORDING	=	3044;

const LONG VIX_E_VM_NOT_REPLAYING	=	3045;

const LONG VIX_E_VM_NOT_FOUND	=	4000;

const LONG VIX_E_NOT_SUPPORTED_FOR_VM_VERSION	=	4001;

const LONG VIX_E_CANNOT_READ_VM_CONFIG	=	4002;

const LONG VIX_E_TEMPLATE_VM	=	4003;

const LONG VIX_E_VM_ALREADY_LOADED	=	4004;

const LONG VIX_E_VM_ALREADY_UP_TO_DATE	=	4006;

const LONG VIX_E_VM_UNSUPPORTED_GUEST	=	4011;

const LONG VIX_E_UNRECOGNIZED_PROPERTY	=	6000;

const LONG VIX_E_INVALID_PROPERTY_VALUE	=	6001;

const LONG VIX_E_READ_ONLY_PROPERTY	=	6002;

const LONG VIX_E_MISSING_REQUIRED_PROPERTY	=	6003;

const LONG VIX_E_INVALID_SERIALIZED_DATA	=	6004;

const LONG VIX_E_BAD_VM_INDEX	=	8000;

const LONG VIX_E_INVALID_MESSAGE_HEADER	=	10000;

const LONG VIX_E_INVALID_MESSAGE_BODY	=	10001;

const LONG VIX_E_SNAPSHOT_INVAL	=	13000;

const LONG VIX_E_SNAPSHOT_DUMPER	=	13001;

const LONG VIX_E_SNAPSHOT_DISKLIB	=	13002;

const LONG VIX_E_SNAPSHOT_NOTFOUND	=	13003;

const LONG VIX_E_SNAPSHOT_EXISTS	=	13004;

const LONG VIX_E_SNAPSHOT_VERSION	=	13005;

const LONG VIX_E_SNAPSHOT_NOPERM	=	13006;

const LONG VIX_E_SNAPSHOT_CONFIG	=	13007;

const LONG VIX_E_SNAPSHOT_NOCHANGE	=	13008;

const LONG VIX_E_SNAPSHOT_CHECKPOINT	=	13009;

const LONG VIX_E_SNAPSHOT_LOCKED	=	13010;

const LONG VIX_E_SNAPSHOT_INCONSISTENT	=	13011;

const LONG VIX_E_SNAPSHOT_NAMETOOLONG	=	13012;

const LONG VIX_E_SNAPSHOT_VIXFILE	=	13013;

const LONG VIX_E_SNAPSHOT_DISKLOCKED	=	13014;

const LONG VIX_E_SNAPSHOT_DUPLICATEDDISK	=	13015;

const LONG VIX_E_SNAPSHOT_INDEPENDENTDISK	=	13016;

const LONG VIX_E_SNAPSHOT_NONUNIQUE_NAME	=	13017;

const LONG VIX_E_SNAPSHOT_MEMORY_ON_INDEPENDENT_DISK	=	13018;

const LONG VIX_E_SNAPSHOT_MAXSNAPSHOTS	=	13019;

const LONG VIX_E_SNAPSHOT_MIN_FREE_SPACE	=	13020;

const LONG VIX_E_SNAPSHOT_HIERARCHY_TOODEEP	=	13021;

const LONG VIX_E_HOST_DISK_INVALID_VALUE	=	14003;

const LONG VIX_E_HOST_DISK_SECTORSIZE	=	14004;

const LONG VIX_E_HOST_FILE_ERROR_EOF	=	14005;

const LONG VIX_E_HOST_NETBLKDEV_HANDSHAKE	=	14006;

const LONG VIX_E_HOST_SOCKET_CREATION_ERROR	=	14007;

const LONG VIX_E_HOST_SERVER_NOT_FOUND	=	14008;

const LONG VIX_E_HOST_NETWORK_CONN_REFUSED	=	14009;

const LONG VIX_E_HOST_TCP_SOCKET_ERROR	=	14010;

const LONG VIX_E_HOST_TCP_CONN_LOST	=	14011;

const LONG VIX_E_HOST_NBD_HASHFILE_VOLUME	=	14012;

const LONG VIX_E_HOST_NBD_HASHFILE_INIT	=	14013;

const LONG VIX_E_DISK_INVAL	=	16000;

const LONG VIX_E_DISK_NOINIT	=	16001;

const LONG VIX_E_DISK_NOIO	=	16002;

const LONG VIX_E_DISK_PARTIALCHAIN	=	16003;

const LONG VIX_E_DISK_NEEDSREPAIR	=	16006;

const LONG VIX_E_DISK_OUTOFRANGE	=	16007;

const LONG VIX_E_DISK_CID_MISMATCH	=	16008;

const LONG VIX_E_DISK_CANTSHRINK	=	16009;

const LONG VIX_E_DISK_PARTMISMATCH	=	16010;

const LONG VIX_E_DISK_UNSUPPORTEDDISKVERSION	=	16011;

const LONG VIX_E_DISK_OPENPARENT	=	16012;

const LONG VIX_E_DISK_NOTSUPPORTED	=	16013;

const LONG VIX_E_DISK_NEEDKEY	=	16014;

const LONG VIX_E_DISK_NOKEYOVERRIDE	=	16015;

const LONG VIX_E_DISK_NOTENCRYPTED	=	16016;

const LONG VIX_E_DISK_NOKEY	=	16017;

const LONG VIX_E_DISK_INVALIDPARTITIONTABLE	=	16018;

const LONG VIX_E_DISK_NOTNORMAL	=	16019;

const LONG VIX_E_DISK_NOTENCDESC	=	16020;

const LONG VIX_E_DISK_NEEDVMFS	=	16022;

const LONG VIX_E_DISK_RAWTOOBIG	=	16024;

const LONG VIX_E_DISK_TOOMANYOPENFILES	=	16027;

const LONG VIX_E_DISK_TOOMANYREDO	=	16028;

const LONG VIX_E_DISK_RAWTOOSMALL	=	16029;

const LONG VIX_E_DISK_INVALIDCHAIN	=	16030;

const LONG VIX_E_DISK_KEY_NOTFOUND	=	16052;

const LONG VIX_E_DISK_SUBSYSTEM_INIT_FAIL	=	16053;

const LONG VIX_E_DISK_INVALID_CONNECTION	=	16054;

const LONG VIX_E_DISK_ENCODING	=	16061;

const LONG VIX_E_DISK_CANTREPAIR	=	16062;

const LONG VIX_E_DISK_INVALIDDISK	=	16063;

const LONG VIX_E_DISK_NOLICENSE	=	16064;

const LONG VIX_E_DISK_NODEVICE	=	16065;

const LONG VIX_E_DISK_UNSUPPORTEDDEVICE	=	16066;

const LONG VIX_E_CRYPTO_UNKNOWN_ALGORITHM	=	17000;

const LONG VIX_E_CRYPTO_BAD_BUFFER_SIZE	=	17001;

const LONG VIX_E_CRYPTO_INVALID_OPERATION	=	17002;

const LONG VIX_E_CRYPTO_RANDOM_DEVICE	=	17003;

const LONG VIX_E_CRYPTO_NEED_PASSWORD	=	17004;

const LONG VIX_E_CRYPTO_BAD_PASSWORD	=	17005;

const LONG VIX_E_CRYPTO_NOT_IN_DICTIONARY	=	17006;

const LONG VIX_E_CRYPTO_NO_CRYPTO	=	17007;

const LONG VIX_E_CRYPTO_ERROR	=	17008;

const LONG VIX_E_CRYPTO_BAD_FORMAT	=	17009;

const LONG VIX_E_CRYPTO_LOCKED	=	17010;

const LONG VIX_E_CRYPTO_EMPTY	=	17011;

const LONG VIX_E_CRYPTO_KEYSAFE_LOCATOR	=	17012;

const LONG VIX_E_CANNOT_CONNECT_TO_HOST	=	18000;

const LONG VIX_E_NOT_FOR_REMOTE_HOST	=	18001;

const LONG VIX_E_INVALID_HOSTNAME_SPECIFICATION	=	18002;

const LONG VIX_E_SCREEN_CAPTURE_ERROR	=	19000;

const LONG VIX_E_SCREEN_CAPTURE_BAD_FORMAT	=	19001;

const LONG VIX_E_SCREEN_CAPTURE_COMPRESSION_FAIL	=	19002;

const LONG VIX_E_SCREEN_CAPTURE_LARGE_DATA	=	19003;

const LONG VIX_E_GUEST_VOLUMES_NOT_FROZEN	=	20000;

const LONG VIX_E_NOT_A_FILE	=	20001;

const LONG VIX_E_NOT_A_DIRECTORY	=	20002;

const LONG VIX_E_NO_SUCH_PROCESS	=	20003;

const LONG VIX_E_FILE_NAME_TOO_LONG	=	20004;

const LONG VIX_E_TOOLS_INSTALL_NO_IMAGE	=	21000;

const LONG VIX_E_TOOLS_INSTALL_IMAGE_INACCESIBLE	=	21001;

const LONG VIX_E_TOOLS_INSTALL_NO_DEVICE	=	21002;

const LONG VIX_E_TOOLS_INSTALL_DEVICE_NOT_CONNECTED	=	21003;

const LONG VIX_E_TOOLS_INSTALL_CANCELLED	=	21004;

const LONG VIX_E_TOOLS_INSTALL_INIT_FAILED	=	21005;

const LONG VIX_E_TOOLS_INSTALL_AUTO_NOT_SUPPORTED	=	21006;

const LONG VIX_E_TOOLS_INSTALL_GUEST_NOT_READY	=	21007;

const LONG VIX_E_TOOLS_INSTALL_SIG_CHECK_FAILED	=	21008;

const LONG VIX_E_TOOLS_INSTALL_ERROR	=	21009;

const LONG VIX_E_TOOLS_INSTALL_ALREADY_UP_TO_DATE	=	21010;

const LONG VIX_E_TOOLS_INSTALL_IN_PROGRESS	=	21011;

const LONG VIX_E_WRAPPER_WORKSTATION_NOT_INSTALLED	=	22001;

const LONG VIX_E_WRAPPER_VERSION_NOT_FOUND	=	22002;

const LONG VIX_E_WRAPPER_SERVICEPROVIDER_NOT_FOUND	=	22003;

const LONG VIX_E_WRAPPER_PLAYER_NOT_INSTALLED	=	22004;

const LONG VIX_E_MNTAPI_MOUNTPT_NOT_FOUND	=	24000;

const LONG VIX_E_MNTAPI_MOUNTPT_IN_USE	=	24001;

const LONG VIX_E_MNTAPI_DISK_NOT_FOUND	=	24002;

const LONG VIX_E_MNTAPI_DISK_NOT_MOUNTED	=	24003;

const LONG VIX_E_MNTAPI_DISK_IS_MOUNTED	=	24004;

const LONG VIX_E_MNTAPI_DISK_NOT_SAFE	=	24005;

const LONG VIX_E_MNTAPI_DISK_CANT_OPEN	=	24006;

const LONG VIX_E_MNTAPI_CANT_READ_PARTS	=	24007;

const LONG VIX_E_MNTAPI_UMOUNT_APP_NOT_FOUND	=	24008;

const LONG VIX_E_MNTAPI_UMOUNT	=	24009;

const LONG VIX_E_MNTAPI_NO_MOUNTABLE_PARTITONS	=	24010;

const LONG VIX_E_MNTAPI_PARTITION_RANGE	=	24011;

const LONG VIX_E_MNTAPI_PERM	=	24012;

const LONG VIX_E_MNTAPI_DICT	=	24013;

const LONG VIX_E_MNTAPI_DICT_LOCKED	=	24014;

const LONG VIX_E_MNTAPI_OPEN_HANDLES	=	24015;

const LONG VIX_E_MNTAPI_CANT_MAKE_VAR_DIR	=	24016;

const LONG VIX_E_MNTAPI_NO_ROOT	=	24017;

const LONG VIX_E_MNTAPI_LOOP_FAILED	=	24018;

const LONG VIX_E_MNTAPI_DAEMON	=	24019;

const LONG VIX_E_MNTAPI_INTERNAL	=	24020;

const LONG VIX_E_MNTAPI_SYSTEM	=	24021;

const LONG VIX_E_MNTAPI_NO_CONNECTION_DETAILS	=	24022;

const LONG VIX_E_MNTAPI_INCOMPATIBLE_VERSION	=	24300;

const LONG VIX_E_MNTAPI_OS_ERROR	=	24301;

const LONG VIX_E_MNTAPI_DRIVE_LETTER_IN_USE	=	24302;

const LONG VIX_E_MNTAPI_DRIVE_LETTER_ALREADY_ASSIGNED	=	24303;

const LONG VIX_E_MNTAPI_VOLUME_NOT_MOUNTED	=	24304;

const LONG VIX_E_MNTAPI_VOLUME_ALREADY_MOUNTED	=	24305;

const LONG VIX_E_MNTAPI_FORMAT_FAILURE	=	24306;

const LONG VIX_E_MNTAPI_NO_DRIVER	=	24307;

const LONG VIX_E_MNTAPI_ALREADY_OPENED	=	24308;

const LONG VIX_E_MNTAPI_ITEM_NOT_FOUND	=	24309;

const LONG VIX_E_MNTAPI_UNSUPPROTED_BOOT_LOADER	=	24310;

const LONG VIX_E_MNTAPI_UNSUPPROTED_OS	=	24311;

const LONG VIX_E_MNTAPI_CODECONVERSION	=	24312;

const LONG VIX_E_MNTAPI_REGWRITE_ERROR	=	24313;

const LONG VIX_E_MNTAPI_UNSUPPORTED_FT_VOLUME	=	24314;

const LONG VIX_E_MNTAPI_PARTITION_NOT_FOUND	=	24315;

const LONG VIX_E_MNTAPI_PUTFILE_ERROR	=	24316;

const LONG VIX_E_MNTAPI_GETFILE_ERROR	=	24317;

const LONG VIX_E_MNTAPI_REG_NOT_OPENED	=	24318;

const LONG VIX_E_MNTAPI_REGDELKEY_ERROR	=	24319;

const LONG VIX_E_MNTAPI_CREATE_PARTITIONTABLE_ERROR	=	24320;

const LONG VIX_E_MNTAPI_OPEN_FAILURE	=	24321;

const LONG VIX_E_MNTAPI_VOLUME_NOT_WRITABLE	=	24322;

const LONG VIX_PROPERTYTYPE_ANY	=	0;

const LONG VIX_PROPERTYTYPE_INTEGER	=	1;

const LONG VIX_PROPERTYTYPE_STRING	=	2;

const LONG VIX_PROPERTYTYPE_BOOL	=	3;

const LONG VIX_PROPERTYTYPE_HANDLE	=	4;

const LONG VIX_PROPERTYTYPE_INT64	=	5;

const LONG VIX_PROPERTYTYPE_BLOB	=	6;

const LONG VIX_PROPERTY_NONE	=	0;

const LONG VIX_PROPERTY_META_DATA_CONTAINER	=	2;

const LONG VIX_PROPERTY_HOST_HOSTTYPE	=	50;

const LONG VIX_PROPERTY_HOST_API_VERSION	=	51;

const LONG VIX_PROPERTY_VM_NUM_VCPUS	=	101;

const LONG VIX_PROPERTY_VM_VMX_PATHNAME	=	103;

const LONG VIX_PROPERTY_VM_VMTEAM_PATHNAME	=	105;

const LONG VIX_PROPERTY_VM_MEMORY_SIZE	=	106;

const LONG VIX_PROPERTY_VM_READ_ONLY	=	107;

const LONG VIX_PROPERTY_VM_NAME	=	108;

const LONG VIX_PROPERTY_VM_GUESTOS	=	109;

const LONG VIX_PROPERTY_VM_IN_VMTEAM	=	128;

const LONG VIX_PROPERTY_VM_POWER_STATE	=	129;

const LONG VIX_PROPERTY_VM_TOOLS_STATE	=	152;

const LONG VIX_PROPERTY_VM_IS_RUNNING	=	196;

const LONG VIX_PROPERTY_VM_SUPPORTED_FEATURES	=	197;

const LONG VIX_PROPERTY_VM_IS_RECORDING	=	236;

const LONG VIX_PROPERTY_VM_IS_REPLAYING	=	237;

const LONG VIX_PROPERTY_JOB_RESULT_ERROR_CODE	=	3000;

const LONG VIX_PROPERTY_JOB_RESULT_VM_IN_GROUP	=	3001;

const LONG VIX_PROPERTY_JOB_RESULT_USER_MESSAGE	=	3002;

const LONG VIX_PROPERTY_JOB_RESULT_EXIT_CODE	=	3004;

const LONG VIX_PROPERTY_JOB_RESULT_COMMAND_OUTPUT	=	3005;

const LONG VIX_PROPERTY_JOB_RESULT_HANDLE	=	3010;

const LONG VIX_PROPERTY_JOB_RESULT_GUEST_OBJECT_EXISTS	=	3011;

const LONG VIX_PROPERTY_JOB_RESULT_GUEST_PROGRAM_ELAPSED_TIME	=	3017;

const LONG VIX_PROPERTY_JOB_RESULT_GUEST_PROGRAM_EXIT_CODE	=	3018;

const LONG VIX_PROPERTY_JOB_RESULT_ITEM_NAME	=	3035;

const LONG VIX_PROPERTY_JOB_RESULT_FOUND_ITEM_DESCRIPTION	=	3036;

const LONG VIX_PROPERTY_JOB_RESULT_SHARED_FOLDER_COUNT	=	3046;

const LONG VIX_PROPERTY_JOB_RESULT_SHARED_FOLDER_HOST	=	3048;

const LONG VIX_PROPERTY_JOB_RESULT_SHARED_FOLDER_FLAGS	=	3049;

const LONG VIX_PROPERTY_JOB_RESULT_PROCESS_ID	=	3051;

const LONG VIX_PROPERTY_JOB_RESULT_PROCESS_OWNER	=	3052;

const LONG VIX_PROPERTY_JOB_RESULT_PROCESS_COMMAND	=	3053;

const LONG VIX_PROPERTY_JOB_RESULT_FILE_FLAGS	=	3054;

const LONG VIX_PROPERTY_JOB_RESULT_PROCESS_START_TIME	=	3055;

const LONG VIX_PROPERTY_JOB_RESULT_VM_VARIABLE_STRING	=	3056;

const LONG VIX_PROPERTY_JOB_RESULT_PROCESS_BEING_DEBUGGED	=	3057;

const LONG VIX_PROPERTY_JOB_RESULT_SCREEN_IMAGE_SIZE	=	3058;

const LONG VIX_PROPERTY_JOB_RESULT_SCREEN_IMAGE_DATA	=	3059;

const LONG VIX_PROPERTY_JOB_RESULT_FILE_SIZE	=	3061;

const LONG VIX_PROPERTY_JOB_RESULT_FILE_MOD_TIME	=	3062;

const LONG VIX_PROPERTY_FOUND_ITEM_LOCATION	=	4010;

const LONG VIX_PROPERTY_SNAPSHOT_DISPLAYNAME	=	4200;

const LONG VIX_PROPERTY_SNAPSHOT_DESCRIPTION	=	4201;

const LONG VIX_PROPERTY_SNAPSHOT_POWERSTATE	=	4205;

const LONG VIX_PROPERTY_SNAPSHOT_IS_REPLAYABLE	=	4207;

const LONG VIX_PROPERTY_GUEST_SHAREDFOLDERS_SHARES_PATH	=	4525;

const LONG VIX_PROPERTY_VM_ENCRYPTION_PASSWORD	=	7001;

const LONG VIX_EVENTTYPE_JOB_COMPLETED	=	2;

const LONG VIX_EVENTTYPE_JOB_PROGRESS	=	3;

const LONG VIX_EVENTTYPE_FIND_ITEM	=	8;

const LONG VIX_EVENTTYPE_CALLBACK_SIGNALLED	=	2;

const LONG VIX_FILE_ATTRIBUTES_DIRECTORY	=	0x1;

const LONG VIX_FILE_ATTRIBUTES_SYMLINK	=	0x2;

const LONG VIX_HOSTOPTION_USE_EVENT_PUMP	=	0x8;

const LONG VIX_SERVICEPROVIDER_DEFAULT	=	1;

const LONG VIX_SERVICEPROVIDER_VMWARE_SERVER	=	2;

const LONG VIX_SERVICEPROVIDER_VMWARE_WORKSTATION	=	3;

const LONG VIX_SERVICEPROVIDER_VMWARE_PLAYER	=	4;

const LONG VIX_SERVICEPROVIDER_VMWARE_VI_SERVER	=	10;

const LONG VIX_API_VERSION	=	-1;

const LONG VIX_FIND_RUNNING_VMS	=	1;

const LONG VIX_FIND_REGISTERED_VMS	=	4;

const LONG VIX_VMOPEN_NORMAL	=	0;

const LONG VIX_PUMPEVENTOPTION_NONE	=	0;

const LONG VIX_VMPOWEROP_NORMAL	=	0;

const LONG VIX_VMPOWEROP_FROM_GUEST	=	0x4;

const LONG VIX_VMPOWEROP_SUPPRESS_SNAPSHOT_POWERON	=	0x80;

const LONG VIX_VMPOWEROP_LAUNCH_GUI	=	0x200;

const LONG VIX_VMPOWEROP_START_VM_PAUSED	=	0x1000;

const LONG VIX_VMDELETE_DISK_FILES	=	0x2;

const LONG VIX_POWERSTATE_POWERING_OFF	=	0x1;

const LONG VIX_POWERSTATE_POWERED_OFF	=	0x2;

const LONG VIX_POWERSTATE_POWERING_ON	=	0x4;

const LONG VIX_POWERSTATE_POWERED_ON	=	0x8;

const LONG VIX_POWERSTATE_SUSPENDING	=	0x10;

const LONG VIX_POWERSTATE_SUSPENDED	=	0x20;

const LONG VIX_POWERSTATE_TOOLS_RUNNING	=	0x40;

const LONG VIX_POWERSTATE_RESETTING	=	0x80;

const LONG VIX_POWERSTATE_BLOCKED_ON_MSG	=	0x100;

const LONG VIX_POWERSTATE_PAUSED	=	0x200;

const LONG VIX_POWERSTATE_RESUMING	=	0x800;

const LONG VIX_TOOLSSTATE_UNKNOWN	=	0x1;

const LONG VIX_TOOLSSTATE_RUNNING	=	0x2;

const LONG VIX_TOOLSSTATE_NOT_INSTALLED	=	0x4;

const LONG VIX_VM_SUPPORT_SHARED_FOLDERS	=	0x1;

const LONG VIX_VM_SUPPORT_MULTIPLE_SNAPSHOTS	=	0x2;

const LONG VIX_VM_SUPPORT_TOOLS_INSTALL	=	0x4;

const LONG VIX_VM_SUPPORT_HARDWARE_UPGRADE	=	0x8;

const LONG VIX_LOGIN_IN_GUEST_REQUIRE_INTERACTIVE_ENVIRONMENT	=	0x8;

const LONG VIX_RUNPROGRAM_RETURN_IMMEDIATELY	=	0x1;

const LONG VIX_RUNPROGRAM_ACTIVATE_WINDOW	=	0x2;

const LONG VIX_VM_GUEST_VARIABLE	=	1;

const LONG VIX_VM_CONFIG_RUNTIME_ONLY	=	2;

const LONG VIX_GUEST_ENVIRONMENT_VARIABLE	=	3;

const LONG VIX_SNAPSHOT_REMOVE_CHILDREN	=	0x1;

const LONG VIX_SNAPSHOT_INCLUDE_MEMORY	=	0x2;

const LONG VIX_SHAREDFOLDER_WRITE_ACCESS	=	0x4;

const LONG VIX_CAPTURESCREENFORMAT_PNG	=	0x1;

const LONG VIX_CAPTURESCREENFORMAT_PNG_NOCOMPRESS	=	0x2;

const LONG VIX_CLONETYPE_FULL	=	0;

const LONG VIX_CLONETYPE_LINKED	=	1;

const LONG VIX_INSTALLTOOLS_MOUNT_TOOLS_INSTALLER	=	0;

const LONG VIX_INSTALLTOOLS_AUTO_UPGRADE	=	0x1;

const LONG VIX_INSTALLTOOLS_RETURN_IMMEDIATELY	=	0x2;

#endif /* __Constants_MODULE_DEFINED__ */

#ifndef __IVixLib_INTERFACE_DEFINED__
#define __IVixLib_INTERFACE_DEFINED__

/* interface IVixLib */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IVixLib;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("94EAA428-2460-470D-8F66-2A4270AFF20A")
    IVixLib : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Connect( 
            /* [in] */ LONG apiVersion,
            /* [in] */ LONG hostType,
            /* [in] */ BSTR hostName,
            /* [in] */ LONG hostPort,
            /* [in] */ BSTR userName,
            /* [in] */ BSTR password,
            /* [in] */ LONG options,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **connectJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetErrorText( 
            /* [in] */ ULONGLONG err,
            /* [in] */ BSTR locale,
            /* [retval][out] */ BSTR *errorString) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ErrorCode( 
            /* [in] */ ULONGLONG err,
            /* [retval][out] */ SHORT *errorCode) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ErrorIndicatesSuccess( 
            /* [in] */ ULONGLONG err,
            /* [retval][out] */ VARIANT_BOOL *isSuccess) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ErrorIndicatesFailure( 
            /* [in] */ ULONGLONG err,
            /* [retval][out] */ VARIANT_BOOL *isFailure) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IVixLibVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVixLib * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVixLib * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVixLib * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IVixLib * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IVixLib * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IVixLib * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IVixLib * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Connect )( 
            IVixLib * This,
            /* [in] */ LONG apiVersion,
            /* [in] */ LONG hostType,
            /* [in] */ BSTR hostName,
            /* [in] */ LONG hostPort,
            /* [in] */ BSTR userName,
            /* [in] */ BSTR password,
            /* [in] */ LONG options,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **connectJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetErrorText )( 
            IVixLib * This,
            /* [in] */ ULONGLONG err,
            /* [in] */ BSTR locale,
            /* [retval][out] */ BSTR *errorString);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ErrorCode )( 
            IVixLib * This,
            /* [in] */ ULONGLONG err,
            /* [retval][out] */ SHORT *errorCode);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ErrorIndicatesSuccess )( 
            IVixLib * This,
            /* [in] */ ULONGLONG err,
            /* [retval][out] */ VARIANT_BOOL *isSuccess);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ErrorIndicatesFailure )( 
            IVixLib * This,
            /* [in] */ ULONGLONG err,
            /* [retval][out] */ VARIANT_BOOL *isFailure);
        
        END_INTERFACE
    } IVixLibVtbl;

    interface IVixLib
    {
        CONST_VTBL struct IVixLibVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVixLib_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IVixLib_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IVixLib_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IVixLib_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IVixLib_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IVixLib_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IVixLib_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IVixLib_Connect(This,apiVersion,hostType,hostName,hostPort,userName,password,options,propertyList,jobDoneCallback,connectJob)	\
    ( (This)->lpVtbl -> Connect(This,apiVersion,hostType,hostName,hostPort,userName,password,options,propertyList,jobDoneCallback,connectJob) ) 

#define IVixLib_GetErrorText(This,err,locale,errorString)	\
    ( (This)->lpVtbl -> GetErrorText(This,err,locale,errorString) ) 

#define IVixLib_ErrorCode(This,err,errorCode)	\
    ( (This)->lpVtbl -> ErrorCode(This,err,errorCode) ) 

#define IVixLib_ErrorIndicatesSuccess(This,err,isSuccess)	\
    ( (This)->lpVtbl -> ErrorIndicatesSuccess(This,err,isSuccess) ) 

#define IVixLib_ErrorIndicatesFailure(This,err,isFailure)	\
    ( (This)->lpVtbl -> ErrorIndicatesFailure(This,err,isFailure) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IVixLib_INTERFACE_DEFINED__ */


#ifndef __IVixHandle_INTERFACE_DEFINED__
#define __IVixHandle_INTERFACE_DEFINED__

/* interface IVixHandle */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IVixHandle;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("70AED194-6CFA-4BA3-8E63-D32A3573A171")
    IVixHandle : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetHandleType( 
            /* [retval][out] */ LONG *handleType) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetProperties( 
            /* [in] */ VARIANT propertyIDs,
            /* [out][in] */ VARIANT *propertiesArray,
            /* [retval][out] */ ULONGLONG *error) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetPropertyType( 
            /* [in] */ LONG propertyID,
            /* [out] */ LONG *propertyType,
            /* [retval][out] */ ULONGLONG *error) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Equals( 
            /* [in] */ IVixHandle *handle,
            /* [retval][out] */ VARIANT_BOOL *isEqual) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IVixHandleVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVixHandle * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVixHandle * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVixHandle * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IVixHandle * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IVixHandle * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IVixHandle * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IVixHandle * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetHandleType )( 
            IVixHandle * This,
            /* [retval][out] */ LONG *handleType);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetProperties )( 
            IVixHandle * This,
            /* [in] */ VARIANT propertyIDs,
            /* [out][in] */ VARIANT *propertiesArray,
            /* [retval][out] */ ULONGLONG *error);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPropertyType )( 
            IVixHandle * This,
            /* [in] */ LONG propertyID,
            /* [out] */ LONG *propertyType,
            /* [retval][out] */ ULONGLONG *error);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Equals )( 
            IVixHandle * This,
            /* [in] */ IVixHandle *handle,
            /* [retval][out] */ VARIANT_BOOL *isEqual);
        
        END_INTERFACE
    } IVixHandleVtbl;

    interface IVixHandle
    {
        CONST_VTBL struct IVixHandleVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVixHandle_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IVixHandle_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IVixHandle_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IVixHandle_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IVixHandle_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IVixHandle_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IVixHandle_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IVixHandle_GetHandleType(This,handleType)	\
    ( (This)->lpVtbl -> GetHandleType(This,handleType) ) 

#define IVixHandle_GetProperties(This,propertyIDs,propertiesArray,error)	\
    ( (This)->lpVtbl -> GetProperties(This,propertyIDs,propertiesArray,error) ) 

#define IVixHandle_GetPropertyType(This,propertyID,propertyType,error)	\
    ( (This)->lpVtbl -> GetPropertyType(This,propertyID,propertyType,error) ) 

#define IVixHandle_Equals(This,handle,isEqual)	\
    ( (This)->lpVtbl -> Equals(This,handle,isEqual) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IVixHandle_INTERFACE_DEFINED__ */


#ifndef __IVixHandle2_INTERFACE_DEFINED__
#define __IVixHandle2_INTERFACE_DEFINED__

/* interface IVixHandle2 */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IVixHandle2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2F5A88E5-E1BE-4d51-9240-2FA39A69606F")
    IVixHandle2 : public IVixHandle
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Close( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IVixHandle2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVixHandle2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVixHandle2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVixHandle2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IVixHandle2 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IVixHandle2 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IVixHandle2 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IVixHandle2 * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetHandleType )( 
            IVixHandle2 * This,
            /* [retval][out] */ LONG *handleType);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetProperties )( 
            IVixHandle2 * This,
            /* [in] */ VARIANT propertyIDs,
            /* [out][in] */ VARIANT *propertiesArray,
            /* [retval][out] */ ULONGLONG *error);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetPropertyType )( 
            IVixHandle2 * This,
            /* [in] */ LONG propertyID,
            /* [out] */ LONG *propertyType,
            /* [retval][out] */ ULONGLONG *error);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Equals )( 
            IVixHandle2 * This,
            /* [in] */ IVixHandle *handle,
            /* [retval][out] */ VARIANT_BOOL *isEqual);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Close )( 
            IVixHandle2 * This);
        
        END_INTERFACE
    } IVixHandle2Vtbl;

    interface IVixHandle2
    {
        CONST_VTBL struct IVixHandle2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVixHandle2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IVixHandle2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IVixHandle2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IVixHandle2_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IVixHandle2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IVixHandle2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IVixHandle2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IVixHandle2_GetHandleType(This,handleType)	\
    ( (This)->lpVtbl -> GetHandleType(This,handleType) ) 

#define IVixHandle2_GetProperties(This,propertyIDs,propertiesArray,error)	\
    ( (This)->lpVtbl -> GetProperties(This,propertyIDs,propertiesArray,error) ) 

#define IVixHandle2_GetPropertyType(This,propertyID,propertyType,error)	\
    ( (This)->lpVtbl -> GetPropertyType(This,propertyID,propertyType,error) ) 

#define IVixHandle2_Equals(This,handle,isEqual)	\
    ( (This)->lpVtbl -> Equals(This,handle,isEqual) ) 


#define IVixHandle2_Close(This)	\
    ( (This)->lpVtbl -> Close(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IVixHandle2_INTERFACE_DEFINED__ */


#ifndef __ICallback_INTERFACE_DEFINED__
#define __ICallback_INTERFACE_DEFINED__

/* interface ICallback */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ICallback;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("82BF64C6-E21C-4B78-9665-6933E767B430")
    ICallback : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OnVixEvent( 
            /* [in] */ IJob *job,
            /* [in] */ LONG eventType,
            /* [in] */ IVixHandle *moreEventInfo) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICallbackVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICallback * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICallback * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICallback * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ICallback * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ICallback * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ICallback * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ICallback * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OnVixEvent )( 
            ICallback * This,
            /* [in] */ IJob *job,
            /* [in] */ LONG eventType,
            /* [in] */ IVixHandle *moreEventInfo);
        
        END_INTERFACE
    } ICallbackVtbl;

    interface ICallback
    {
        CONST_VTBL struct ICallbackVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICallback_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICallback_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICallback_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICallback_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ICallback_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ICallback_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ICallback_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ICallback_OnVixEvent(This,job,eventType,moreEventInfo)	\
    ( (This)->lpVtbl -> OnVixEvent(This,job,eventType,moreEventInfo) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICallback_INTERFACE_DEFINED__ */


#ifndef __IJob_INTERFACE_DEFINED__
#define __IJob_INTERFACE_DEFINED__

/* interface IJob */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IJob;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("77A6B455-C303-4B67-810D-B9E6077E75AE")
    IJob : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Wait( 
            /* [in] */ VARIANT propertyIDs,
            /* [out][in] */ VARIANT *propertiesArray,
            /* [retval][out] */ ULONGLONG *error) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WaitWithoutResults( 
            /* [retval][out] */ ULONGLONG *error) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CheckCompletion( 
            /* [out] */ VARIANT_BOOL *isComplete,
            /* [retval][out] */ ULONGLONG *error) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetError( 
            /* [retval][out] */ ULONGLONG *error) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetNumProperties( 
            /* [in] */ LONG resultPropertyID,
            /* [retval][out] */ LONG *numProperties) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetNthProperties( 
            /* [in] */ LONG index,
            /* [in] */ VARIANT propertyIDs,
            /* [out][in] */ VARIANT *propertiesArray,
            /* [retval][out] */ ULONGLONG *error) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IJobVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IJob * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IJob * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IJob * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IJob * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IJob * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IJob * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IJob * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Wait )( 
            IJob * This,
            /* [in] */ VARIANT propertyIDs,
            /* [out][in] */ VARIANT *propertiesArray,
            /* [retval][out] */ ULONGLONG *error);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WaitWithoutResults )( 
            IJob * This,
            /* [retval][out] */ ULONGLONG *error);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CheckCompletion )( 
            IJob * This,
            /* [out] */ VARIANT_BOOL *isComplete,
            /* [retval][out] */ ULONGLONG *error);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetError )( 
            IJob * This,
            /* [retval][out] */ ULONGLONG *error);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetNumProperties )( 
            IJob * This,
            /* [in] */ LONG resultPropertyID,
            /* [retval][out] */ LONG *numProperties);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetNthProperties )( 
            IJob * This,
            /* [in] */ LONG index,
            /* [in] */ VARIANT propertyIDs,
            /* [out][in] */ VARIANT *propertiesArray,
            /* [retval][out] */ ULONGLONG *error);
        
        END_INTERFACE
    } IJobVtbl;

    interface IJob
    {
        CONST_VTBL struct IJobVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IJob_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IJob_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IJob_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IJob_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IJob_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IJob_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IJob_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IJob_Wait(This,propertyIDs,propertiesArray,error)	\
    ( (This)->lpVtbl -> Wait(This,propertyIDs,propertiesArray,error) ) 

#define IJob_WaitWithoutResults(This,error)	\
    ( (This)->lpVtbl -> WaitWithoutResults(This,error) ) 

#define IJob_CheckCompletion(This,isComplete,error)	\
    ( (This)->lpVtbl -> CheckCompletion(This,isComplete,error) ) 

#define IJob_GetError(This,error)	\
    ( (This)->lpVtbl -> GetError(This,error) ) 

#define IJob_GetNumProperties(This,resultPropertyID,numProperties)	\
    ( (This)->lpVtbl -> GetNumProperties(This,resultPropertyID,numProperties) ) 

#define IJob_GetNthProperties(This,index,propertyIDs,propertiesArray,error)	\
    ( (This)->lpVtbl -> GetNthProperties(This,index,propertyIDs,propertiesArray,error) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IJob_INTERFACE_DEFINED__ */


#ifndef __ISnapshot_INTERFACE_DEFINED__
#define __ISnapshot_INTERFACE_DEFINED__

/* interface ISnapshot */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_ISnapshot;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("89DB37B8-CE29-4B14-AA05-6D5AEABE2FB0")
    ISnapshot : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetNumChildren( 
            /* [out] */ LONG *numChildSnapshots,
            /* [retval][out] */ ULONGLONG *error) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetChild( 
            /* [in] */ LONG index,
            /* [out] */ ISnapshot **childSnapshot,
            /* [retval][out] */ ULONGLONG *error) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetParent( 
            /* [out] */ ISnapshot **parentSnapshot,
            /* [retval][out] */ ULONGLONG *error) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ISnapshotVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ISnapshot * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ISnapshot * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ISnapshot * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ISnapshot * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ISnapshot * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ISnapshot * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ISnapshot * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetNumChildren )( 
            ISnapshot * This,
            /* [out] */ LONG *numChildSnapshots,
            /* [retval][out] */ ULONGLONG *error);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetChild )( 
            ISnapshot * This,
            /* [in] */ LONG index,
            /* [out] */ ISnapshot **childSnapshot,
            /* [retval][out] */ ULONGLONG *error);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetParent )( 
            ISnapshot * This,
            /* [out] */ ISnapshot **parentSnapshot,
            /* [retval][out] */ ULONGLONG *error);
        
        END_INTERFACE
    } ISnapshotVtbl;

    interface ISnapshot
    {
        CONST_VTBL struct ISnapshotVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ISnapshot_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ISnapshot_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ISnapshot_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ISnapshot_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ISnapshot_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ISnapshot_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ISnapshot_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ISnapshot_GetNumChildren(This,numChildSnapshots,error)	\
    ( (This)->lpVtbl -> GetNumChildren(This,numChildSnapshots,error) ) 

#define ISnapshot_GetChild(This,index,childSnapshot,error)	\
    ( (This)->lpVtbl -> GetChild(This,index,childSnapshot,error) ) 

#define ISnapshot_GetParent(This,parentSnapshot,error)	\
    ( (This)->lpVtbl -> GetParent(This,parentSnapshot,error) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ISnapshot_INTERFACE_DEFINED__ */


#ifndef __IHost_INTERFACE_DEFINED__
#define __IHost_INTERFACE_DEFINED__

/* interface IHost */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IHost;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1305925C-B00E-443A-BB13-61D06CBDAB97")
    IHost : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OpenVM( 
            /* [in] */ BSTR vmxFilePath,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **openJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Disconnect( void) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RegisterVM( 
            /* [in] */ BSTR vmxFilePath,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **registerJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UnregisterVM( 
            /* [in] */ BSTR vmxFilePath,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **unregisterJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FindItems( 
            /* [in] */ LONG searchType,
            /* [in] */ IVixHandle *searchCriteria,
            /* [in] */ LONG timeout,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **findJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PumpEvents( 
            /* [in] */ LONG options) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IHostVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IHost * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IHost * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IHost * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IHost * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IHost * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IHost * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IHost * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OpenVM )( 
            IHost * This,
            /* [in] */ BSTR vmxFilePath,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **openJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Disconnect )( 
            IHost * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RegisterVM )( 
            IHost * This,
            /* [in] */ BSTR vmxFilePath,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **registerJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *UnregisterVM )( 
            IHost * This,
            /* [in] */ BSTR vmxFilePath,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **unregisterJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FindItems )( 
            IHost * This,
            /* [in] */ LONG searchType,
            /* [in] */ IVixHandle *searchCriteria,
            /* [in] */ LONG timeout,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **findJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *PumpEvents )( 
            IHost * This,
            /* [in] */ LONG options);
        
        END_INTERFACE
    } IHostVtbl;

    interface IHost
    {
        CONST_VTBL struct IHostVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IHost_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IHost_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IHost_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IHost_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IHost_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IHost_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IHost_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IHost_OpenVM(This,vmxFilePath,jobDoneCallback,openJob)	\
    ( (This)->lpVtbl -> OpenVM(This,vmxFilePath,jobDoneCallback,openJob) ) 

#define IHost_Disconnect(This)	\
    ( (This)->lpVtbl -> Disconnect(This) ) 

#define IHost_RegisterVM(This,vmxFilePath,jobDoneCallback,registerJob)	\
    ( (This)->lpVtbl -> RegisterVM(This,vmxFilePath,jobDoneCallback,registerJob) ) 

#define IHost_UnregisterVM(This,vmxFilePath,jobDoneCallback,unregisterJob)	\
    ( (This)->lpVtbl -> UnregisterVM(This,vmxFilePath,jobDoneCallback,unregisterJob) ) 

#define IHost_FindItems(This,searchType,searchCriteria,timeout,jobDoneCallback,findJob)	\
    ( (This)->lpVtbl -> FindItems(This,searchType,searchCriteria,timeout,jobDoneCallback,findJob) ) 

#define IHost_PumpEvents(This,options)	\
    ( (This)->lpVtbl -> PumpEvents(This,options) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IHost_INTERFACE_DEFINED__ */


#ifndef __IHost2_INTERFACE_DEFINED__
#define __IHost2_INTERFACE_DEFINED__

/* interface IHost2 */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IHost2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("320645AD-DCB7-4843-86D4-ABE15BE5CEDA")
    IHost2 : public IHost
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OpenVMEx( 
            /* [in] */ BSTR vmxFilePath,
            /* [in] */ LONG openVMOptions,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **openJob) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IHost2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IHost2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IHost2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IHost2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IHost2 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IHost2 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IHost2 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IHost2 * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OpenVM )( 
            IHost2 * This,
            /* [in] */ BSTR vmxFilePath,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **openJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Disconnect )( 
            IHost2 * This);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RegisterVM )( 
            IHost2 * This,
            /* [in] */ BSTR vmxFilePath,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **registerJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *UnregisterVM )( 
            IHost2 * This,
            /* [in] */ BSTR vmxFilePath,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **unregisterJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FindItems )( 
            IHost2 * This,
            /* [in] */ LONG searchType,
            /* [in] */ IVixHandle *searchCriteria,
            /* [in] */ LONG timeout,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **findJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *PumpEvents )( 
            IHost2 * This,
            /* [in] */ LONG options);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OpenVMEx )( 
            IHost2 * This,
            /* [in] */ BSTR vmxFilePath,
            /* [in] */ LONG openVMOptions,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **openJob);
        
        END_INTERFACE
    } IHost2Vtbl;

    interface IHost2
    {
        CONST_VTBL struct IHost2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IHost2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IHost2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IHost2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IHost2_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IHost2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IHost2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IHost2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IHost2_OpenVM(This,vmxFilePath,jobDoneCallback,openJob)	\
    ( (This)->lpVtbl -> OpenVM(This,vmxFilePath,jobDoneCallback,openJob) ) 

#define IHost2_Disconnect(This)	\
    ( (This)->lpVtbl -> Disconnect(This) ) 

#define IHost2_RegisterVM(This,vmxFilePath,jobDoneCallback,registerJob)	\
    ( (This)->lpVtbl -> RegisterVM(This,vmxFilePath,jobDoneCallback,registerJob) ) 

#define IHost2_UnregisterVM(This,vmxFilePath,jobDoneCallback,unregisterJob)	\
    ( (This)->lpVtbl -> UnregisterVM(This,vmxFilePath,jobDoneCallback,unregisterJob) ) 

#define IHost2_FindItems(This,searchType,searchCriteria,timeout,jobDoneCallback,findJob)	\
    ( (This)->lpVtbl -> FindItems(This,searchType,searchCriteria,timeout,jobDoneCallback,findJob) ) 

#define IHost2_PumpEvents(This,options)	\
    ( (This)->lpVtbl -> PumpEvents(This,options) ) 


#define IHost2_OpenVMEx(This,vmxFilePath,openVMOptions,propertyList,jobDoneCallback,openJob)	\
    ( (This)->lpVtbl -> OpenVMEx(This,vmxFilePath,openVMOptions,propertyList,jobDoneCallback,openJob) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IHost2_INTERFACE_DEFINED__ */


#ifndef __IVM_INTERFACE_DEFINED__
#define __IVM_INTERFACE_DEFINED__

/* interface IVM */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IVM;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("11E879B1-7969-4654-BE4B-3F05679F742E")
    IVM : public IDispatch
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PowerOn( 
            /* [in] */ LONG powerOnOptions,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **powerJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE PowerOff( 
            /* [in] */ LONG powerOffOptions,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **powerJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Reset( 
            /* [in] */ LONG resetOptions,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **resetJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Suspend( 
            /* [in] */ LONG suspendOptions,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **suspendJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Delete( 
            /* [in] */ LONG deleteOptions,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **deleteJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WaitForToolsInGuest( 
            /* [in] */ LONG timeoutInSeconds,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **waitJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LoginInGuest( 
            /* [in] */ BSTR userName,
            /* [in] */ BSTR password,
            /* [in] */ LONG options,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **loginJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE LogoutFromGuest( 
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **logoutJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RunProgramInGuest( 
            /* [in] */ BSTR guestProgramName,
            /* [in] */ BSTR commandLineArgs,
            /* [in] */ LONG options,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **runJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ListProcessesInGuest( 
            /* [in] */ LONG options,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **listJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE KillProcessInGuest( 
            /* [in] */ ULONGLONG pid,
            /* [in] */ LONG options,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **killJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RunScriptInGuest( 
            /* [in] */ BSTR interrpeter,
            /* [in] */ BSTR scriptText,
            /* [in] */ LONG options,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **runJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE OpenUrlInGuest( 
            /* [in] */ BSTR url,
            /* [in] */ LONG windowState,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **openJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CopyFileFromHostToGuest( 
            /* [in] */ BSTR hostPathName,
            /* [in] */ BSTR guestPathName,
            /* [in] */ LONG options,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **copyJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CopyFileFromGuestToHost( 
            /* [in] */ BSTR guestPathName,
            /* [in] */ BSTR hostPathName,
            /* [in] */ LONG options,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **copyJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DeleteFileInGuest( 
            /* [in] */ BSTR guestPathName,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **deleteJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE FileExistsInGuest( 
            /* [in] */ BSTR guestPathName,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **fileJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RenameFileInGuest( 
            /* [in] */ BSTR oldName,
            /* [in] */ BSTR newName,
            /* [in] */ LONG options,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **renameJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateTempFileInGuest( 
            /* [in] */ LONG options,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **createJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ListDirectoryInGuest( 
            /* [in] */ BSTR pathName,
            /* [in] */ LONG options,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **listJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateDirectoryInGuest( 
            /* [in] */ BSTR pathName,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **createJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DeleteDirectoryInGuest( 
            /* [in] */ BSTR pathName,
            /* [in] */ LONG options,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **deleteJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE DirectoryExistsInGuest( 
            /* [in] */ BSTR pathName,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **directoryJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE ReadVariable( 
            /* [in] */ LONG variableType,
            /* [in] */ BSTR name,
            /* [in] */ LONG options,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **readJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE WriteVariable( 
            /* [in] */ LONG variableType,
            /* [in] */ BSTR valueName,
            /* [in] */ BSTR value,
            /* [in] */ LONG options,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **writeJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetNumRootSnapshots( 
            /* [out] */ LONG *result,
            /* [retval][out] */ ULONGLONG *error) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetRootSnapshot( 
            /* [in] */ LONG index,
            /* [out] */ ISnapshot **snapshot,
            /* [retval][out] */ ULONGLONG *error) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetCurrentSnapshot( 
            /* [out] */ ISnapshot **snapshot,
            /* [retval][out] */ ULONGLONG *error) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetNamedSnapshot( 
            /* [in] */ BSTR name,
            /* [out] */ ISnapshot **snapshot,
            /* [retval][out] */ ULONGLONG *error) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RemoveSnapshot( 
            /* [in] */ ISnapshot *snapshot,
            /* [in] */ LONG options,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **removeJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RevertToSnapshot( 
            /* [in] */ ISnapshot *snapshot,
            /* [in] */ LONG options,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **revertJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CreateSnapshot( 
            /* [in] */ BSTR name,
            /* [in] */ BSTR description,
            /* [in] */ LONG options,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **createJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE EnableSharedFolders( 
            /* [in] */ VARIANT_BOOL enabled,
            /* [in] */ LONG options,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **enableJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetNumSharedFolders( 
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **getJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetSharedFolderState( 
            /* [in] */ LONG index,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **getJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE SetSharedFolderState( 
            /* [in] */ BSTR shareName,
            /* [in] */ BSTR hostPathName,
            /* [in] */ LONG flags,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **setJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE AddSharedFolder( 
            /* [in] */ BSTR shareName,
            /* [in] */ BSTR hostPathName,
            /* [in] */ LONG flags,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **addJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE RemoveSharedFolder( 
            /* [in] */ BSTR shareName,
            /* [in] */ LONG flags,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **removeJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE UpgradeVirtualHardware( 
            /* [in] */ LONG options,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **upgradeJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE InstallTools( 
            /* [in] */ LONG options,
            /* [in] */ BSTR commandLineArgs,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **installJob) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IVMVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVM * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVM * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVM * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IVM * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IVM * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IVM * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IVM * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *PowerOn )( 
            IVM * This,
            /* [in] */ LONG powerOnOptions,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **powerJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *PowerOff )( 
            IVM * This,
            /* [in] */ LONG powerOffOptions,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **powerJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IVM * This,
            /* [in] */ LONG resetOptions,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **resetJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Suspend )( 
            IVM * This,
            /* [in] */ LONG suspendOptions,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **suspendJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Delete )( 
            IVM * This,
            /* [in] */ LONG deleteOptions,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **deleteJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WaitForToolsInGuest )( 
            IVM * This,
            /* [in] */ LONG timeoutInSeconds,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **waitJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LoginInGuest )( 
            IVM * This,
            /* [in] */ BSTR userName,
            /* [in] */ BSTR password,
            /* [in] */ LONG options,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **loginJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LogoutFromGuest )( 
            IVM * This,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **logoutJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RunProgramInGuest )( 
            IVM * This,
            /* [in] */ BSTR guestProgramName,
            /* [in] */ BSTR commandLineArgs,
            /* [in] */ LONG options,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **runJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ListProcessesInGuest )( 
            IVM * This,
            /* [in] */ LONG options,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **listJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *KillProcessInGuest )( 
            IVM * This,
            /* [in] */ ULONGLONG pid,
            /* [in] */ LONG options,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **killJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RunScriptInGuest )( 
            IVM * This,
            /* [in] */ BSTR interrpeter,
            /* [in] */ BSTR scriptText,
            /* [in] */ LONG options,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **runJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OpenUrlInGuest )( 
            IVM * This,
            /* [in] */ BSTR url,
            /* [in] */ LONG windowState,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **openJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CopyFileFromHostToGuest )( 
            IVM * This,
            /* [in] */ BSTR hostPathName,
            /* [in] */ BSTR guestPathName,
            /* [in] */ LONG options,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **copyJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CopyFileFromGuestToHost )( 
            IVM * This,
            /* [in] */ BSTR guestPathName,
            /* [in] */ BSTR hostPathName,
            /* [in] */ LONG options,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **copyJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DeleteFileInGuest )( 
            IVM * This,
            /* [in] */ BSTR guestPathName,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **deleteJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FileExistsInGuest )( 
            IVM * This,
            /* [in] */ BSTR guestPathName,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **fileJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RenameFileInGuest )( 
            IVM * This,
            /* [in] */ BSTR oldName,
            /* [in] */ BSTR newName,
            /* [in] */ LONG options,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **renameJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateTempFileInGuest )( 
            IVM * This,
            /* [in] */ LONG options,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **createJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ListDirectoryInGuest )( 
            IVM * This,
            /* [in] */ BSTR pathName,
            /* [in] */ LONG options,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **listJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateDirectoryInGuest )( 
            IVM * This,
            /* [in] */ BSTR pathName,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **createJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DeleteDirectoryInGuest )( 
            IVM * This,
            /* [in] */ BSTR pathName,
            /* [in] */ LONG options,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **deleteJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DirectoryExistsInGuest )( 
            IVM * This,
            /* [in] */ BSTR pathName,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **directoryJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReadVariable )( 
            IVM * This,
            /* [in] */ LONG variableType,
            /* [in] */ BSTR name,
            /* [in] */ LONG options,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **readJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WriteVariable )( 
            IVM * This,
            /* [in] */ LONG variableType,
            /* [in] */ BSTR valueName,
            /* [in] */ BSTR value,
            /* [in] */ LONG options,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **writeJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetNumRootSnapshots )( 
            IVM * This,
            /* [out] */ LONG *result,
            /* [retval][out] */ ULONGLONG *error);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetRootSnapshot )( 
            IVM * This,
            /* [in] */ LONG index,
            /* [out] */ ISnapshot **snapshot,
            /* [retval][out] */ ULONGLONG *error);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCurrentSnapshot )( 
            IVM * This,
            /* [out] */ ISnapshot **snapshot,
            /* [retval][out] */ ULONGLONG *error);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetNamedSnapshot )( 
            IVM * This,
            /* [in] */ BSTR name,
            /* [out] */ ISnapshot **snapshot,
            /* [retval][out] */ ULONGLONG *error);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RemoveSnapshot )( 
            IVM * This,
            /* [in] */ ISnapshot *snapshot,
            /* [in] */ LONG options,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **removeJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RevertToSnapshot )( 
            IVM * This,
            /* [in] */ ISnapshot *snapshot,
            /* [in] */ LONG options,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **revertJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateSnapshot )( 
            IVM * This,
            /* [in] */ BSTR name,
            /* [in] */ BSTR description,
            /* [in] */ LONG options,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **createJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *EnableSharedFolders )( 
            IVM * This,
            /* [in] */ VARIANT_BOOL enabled,
            /* [in] */ LONG options,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **enableJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetNumSharedFolders )( 
            IVM * This,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **getJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetSharedFolderState )( 
            IVM * This,
            /* [in] */ LONG index,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **getJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetSharedFolderState )( 
            IVM * This,
            /* [in] */ BSTR shareName,
            /* [in] */ BSTR hostPathName,
            /* [in] */ LONG flags,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **setJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddSharedFolder )( 
            IVM * This,
            /* [in] */ BSTR shareName,
            /* [in] */ BSTR hostPathName,
            /* [in] */ LONG flags,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **addJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RemoveSharedFolder )( 
            IVM * This,
            /* [in] */ BSTR shareName,
            /* [in] */ LONG flags,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **removeJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *UpgradeVirtualHardware )( 
            IVM * This,
            /* [in] */ LONG options,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **upgradeJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *InstallTools )( 
            IVM * This,
            /* [in] */ LONG options,
            /* [in] */ BSTR commandLineArgs,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **installJob);
        
        END_INTERFACE
    } IVMVtbl;

    interface IVM
    {
        CONST_VTBL struct IVMVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVM_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IVM_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IVM_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IVM_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IVM_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IVM_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IVM_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IVM_PowerOn(This,powerOnOptions,propertyList,jobDoneCallback,powerJob)	\
    ( (This)->lpVtbl -> PowerOn(This,powerOnOptions,propertyList,jobDoneCallback,powerJob) ) 

#define IVM_PowerOff(This,powerOffOptions,jobDoneCallback,powerJob)	\
    ( (This)->lpVtbl -> PowerOff(This,powerOffOptions,jobDoneCallback,powerJob) ) 

#define IVM_Reset(This,resetOptions,jobDoneCallback,resetJob)	\
    ( (This)->lpVtbl -> Reset(This,resetOptions,jobDoneCallback,resetJob) ) 

#define IVM_Suspend(This,suspendOptions,jobDoneCallback,suspendJob)	\
    ( (This)->lpVtbl -> Suspend(This,suspendOptions,jobDoneCallback,suspendJob) ) 

#define IVM_Delete(This,deleteOptions,jobDoneCallback,deleteJob)	\
    ( (This)->lpVtbl -> Delete(This,deleteOptions,jobDoneCallback,deleteJob) ) 

#define IVM_WaitForToolsInGuest(This,timeoutInSeconds,jobDoneCallback,waitJob)	\
    ( (This)->lpVtbl -> WaitForToolsInGuest(This,timeoutInSeconds,jobDoneCallback,waitJob) ) 

#define IVM_LoginInGuest(This,userName,password,options,jobDoneCallback,loginJob)	\
    ( (This)->lpVtbl -> LoginInGuest(This,userName,password,options,jobDoneCallback,loginJob) ) 

#define IVM_LogoutFromGuest(This,jobDoneCallback,logoutJob)	\
    ( (This)->lpVtbl -> LogoutFromGuest(This,jobDoneCallback,logoutJob) ) 

#define IVM_RunProgramInGuest(This,guestProgramName,commandLineArgs,options,propertyList,jobDoneCallback,runJob)	\
    ( (This)->lpVtbl -> RunProgramInGuest(This,guestProgramName,commandLineArgs,options,propertyList,jobDoneCallback,runJob) ) 

#define IVM_ListProcessesInGuest(This,options,jobDoneCallback,listJob)	\
    ( (This)->lpVtbl -> ListProcessesInGuest(This,options,jobDoneCallback,listJob) ) 

#define IVM_KillProcessInGuest(This,pid,options,jobDoneCallback,killJob)	\
    ( (This)->lpVtbl -> KillProcessInGuest(This,pid,options,jobDoneCallback,killJob) ) 

#define IVM_RunScriptInGuest(This,interrpeter,scriptText,options,propertyList,jobDoneCallback,runJob)	\
    ( (This)->lpVtbl -> RunScriptInGuest(This,interrpeter,scriptText,options,propertyList,jobDoneCallback,runJob) ) 

#define IVM_OpenUrlInGuest(This,url,windowState,propertyList,jobDoneCallback,openJob)	\
    ( (This)->lpVtbl -> OpenUrlInGuest(This,url,windowState,propertyList,jobDoneCallback,openJob) ) 

#define IVM_CopyFileFromHostToGuest(This,hostPathName,guestPathName,options,propertyList,jobDoneCallback,copyJob)	\
    ( (This)->lpVtbl -> CopyFileFromHostToGuest(This,hostPathName,guestPathName,options,propertyList,jobDoneCallback,copyJob) ) 

#define IVM_CopyFileFromGuestToHost(This,guestPathName,hostPathName,options,propertyList,jobDoneCallback,copyJob)	\
    ( (This)->lpVtbl -> CopyFileFromGuestToHost(This,guestPathName,hostPathName,options,propertyList,jobDoneCallback,copyJob) ) 

#define IVM_DeleteFileInGuest(This,guestPathName,jobDoneCallback,deleteJob)	\
    ( (This)->lpVtbl -> DeleteFileInGuest(This,guestPathName,jobDoneCallback,deleteJob) ) 

#define IVM_FileExistsInGuest(This,guestPathName,jobDoneCallback,fileJob)	\
    ( (This)->lpVtbl -> FileExistsInGuest(This,guestPathName,jobDoneCallback,fileJob) ) 

#define IVM_RenameFileInGuest(This,oldName,newName,options,propertyList,jobDoneCallback,renameJob)	\
    ( (This)->lpVtbl -> RenameFileInGuest(This,oldName,newName,options,propertyList,jobDoneCallback,renameJob) ) 

#define IVM_CreateTempFileInGuest(This,options,propertyList,jobDoneCallback,createJob)	\
    ( (This)->lpVtbl -> CreateTempFileInGuest(This,options,propertyList,jobDoneCallback,createJob) ) 

#define IVM_ListDirectoryInGuest(This,pathName,options,jobDoneCallback,listJob)	\
    ( (This)->lpVtbl -> ListDirectoryInGuest(This,pathName,options,jobDoneCallback,listJob) ) 

#define IVM_CreateDirectoryInGuest(This,pathName,propertyList,jobDoneCallback,createJob)	\
    ( (This)->lpVtbl -> CreateDirectoryInGuest(This,pathName,propertyList,jobDoneCallback,createJob) ) 

#define IVM_DeleteDirectoryInGuest(This,pathName,options,jobDoneCallback,deleteJob)	\
    ( (This)->lpVtbl -> DeleteDirectoryInGuest(This,pathName,options,jobDoneCallback,deleteJob) ) 

#define IVM_DirectoryExistsInGuest(This,pathName,jobDoneCallback,directoryJob)	\
    ( (This)->lpVtbl -> DirectoryExistsInGuest(This,pathName,jobDoneCallback,directoryJob) ) 

#define IVM_ReadVariable(This,variableType,name,options,jobDoneCallback,readJob)	\
    ( (This)->lpVtbl -> ReadVariable(This,variableType,name,options,jobDoneCallback,readJob) ) 

#define IVM_WriteVariable(This,variableType,valueName,value,options,jobDoneCallback,writeJob)	\
    ( (This)->lpVtbl -> WriteVariable(This,variableType,valueName,value,options,jobDoneCallback,writeJob) ) 

#define IVM_GetNumRootSnapshots(This,result,error)	\
    ( (This)->lpVtbl -> GetNumRootSnapshots(This,result,error) ) 

#define IVM_GetRootSnapshot(This,index,snapshot,error)	\
    ( (This)->lpVtbl -> GetRootSnapshot(This,index,snapshot,error) ) 

#define IVM_GetCurrentSnapshot(This,snapshot,error)	\
    ( (This)->lpVtbl -> GetCurrentSnapshot(This,snapshot,error) ) 

#define IVM_GetNamedSnapshot(This,name,snapshot,error)	\
    ( (This)->lpVtbl -> GetNamedSnapshot(This,name,snapshot,error) ) 

#define IVM_RemoveSnapshot(This,snapshot,options,jobDoneCallback,removeJob)	\
    ( (This)->lpVtbl -> RemoveSnapshot(This,snapshot,options,jobDoneCallback,removeJob) ) 

#define IVM_RevertToSnapshot(This,snapshot,options,propertyList,jobDoneCallback,revertJob)	\
    ( (This)->lpVtbl -> RevertToSnapshot(This,snapshot,options,propertyList,jobDoneCallback,revertJob) ) 

#define IVM_CreateSnapshot(This,name,description,options,propertyList,jobDoneCallback,createJob)	\
    ( (This)->lpVtbl -> CreateSnapshot(This,name,description,options,propertyList,jobDoneCallback,createJob) ) 

#define IVM_EnableSharedFolders(This,enabled,options,jobDoneCallback,enableJob)	\
    ( (This)->lpVtbl -> EnableSharedFolders(This,enabled,options,jobDoneCallback,enableJob) ) 

#define IVM_GetNumSharedFolders(This,jobDoneCallback,getJob)	\
    ( (This)->lpVtbl -> GetNumSharedFolders(This,jobDoneCallback,getJob) ) 

#define IVM_GetSharedFolderState(This,index,jobDoneCallback,getJob)	\
    ( (This)->lpVtbl -> GetSharedFolderState(This,index,jobDoneCallback,getJob) ) 

#define IVM_SetSharedFolderState(This,shareName,hostPathName,flags,jobDoneCallback,setJob)	\
    ( (This)->lpVtbl -> SetSharedFolderState(This,shareName,hostPathName,flags,jobDoneCallback,setJob) ) 

#define IVM_AddSharedFolder(This,shareName,hostPathName,flags,jobDoneCallback,addJob)	\
    ( (This)->lpVtbl -> AddSharedFolder(This,shareName,hostPathName,flags,jobDoneCallback,addJob) ) 

#define IVM_RemoveSharedFolder(This,shareName,flags,jobDoneCallback,removeJob)	\
    ( (This)->lpVtbl -> RemoveSharedFolder(This,shareName,flags,jobDoneCallback,removeJob) ) 

#define IVM_UpgradeVirtualHardware(This,options,jobDoneCallback,upgradeJob)	\
    ( (This)->lpVtbl -> UpgradeVirtualHardware(This,options,jobDoneCallback,upgradeJob) ) 

#define IVM_InstallTools(This,options,commandLineArgs,jobDoneCallback,installJob)	\
    ( (This)->lpVtbl -> InstallTools(This,options,commandLineArgs,jobDoneCallback,installJob) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IVM_INTERFACE_DEFINED__ */


#ifndef __IVM2_INTERFACE_DEFINED__
#define __IVM2_INTERFACE_DEFINED__

/* interface IVM2 */
/* [unique][helpstring][nonextensible][dual][uuid][object] */ 


EXTERN_C const IID IID_IVM2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("F1D44D76-784D-44ff-8FEA-D35A154825A0")
    IVM2 : public IVM
    {
    public:
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Pause( 
            /* [in] */ LONG options,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **pauseJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Unpause( 
            /* [in] */ LONG options,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **unpauseJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE Clone( 
            /* [in] */ ISnapshot *snapshot,
            /* [in] */ LONG cloneType,
            /* [in] */ BSTR destConfigPathName,
            /* [in] */ LONG cloneOptions,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **cloneJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE BeginRecording( 
            /* [in] */ BSTR name,
            /* [in] */ BSTR description,
            /* [in] */ LONG options,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **recordingJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE EndRecording( 
            /* [in] */ LONG options,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **recordingJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE BeginReplay( 
            /* [in] */ ISnapshot *recording,
            /* [in] */ LONG options,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **replayJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE EndReplay( 
            /* [in] */ LONG options,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **replayJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE GetFileInfoInGuest( 
            /* [in] */ BSTR pathName,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **getFileInfoJob) = 0;
        
        virtual /* [helpstring][id] */ HRESULT STDMETHODCALLTYPE CaptureScreenImage( 
            /* [in] */ LONG captureType,
            /* [in] */ IVixHandle *additionalProperties,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **installJob) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IVM2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IVM2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IVM2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IVM2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IVM2 * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IVM2 * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IVM2 * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IVM2 * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *PowerOn )( 
            IVM2 * This,
            /* [in] */ LONG powerOnOptions,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **powerJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *PowerOff )( 
            IVM2 * This,
            /* [in] */ LONG powerOffOptions,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **powerJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IVM2 * This,
            /* [in] */ LONG resetOptions,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **resetJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Suspend )( 
            IVM2 * This,
            /* [in] */ LONG suspendOptions,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **suspendJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Delete )( 
            IVM2 * This,
            /* [in] */ LONG deleteOptions,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **deleteJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WaitForToolsInGuest )( 
            IVM2 * This,
            /* [in] */ LONG timeoutInSeconds,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **waitJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LoginInGuest )( 
            IVM2 * This,
            /* [in] */ BSTR userName,
            /* [in] */ BSTR password,
            /* [in] */ LONG options,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **loginJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *LogoutFromGuest )( 
            IVM2 * This,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **logoutJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RunProgramInGuest )( 
            IVM2 * This,
            /* [in] */ BSTR guestProgramName,
            /* [in] */ BSTR commandLineArgs,
            /* [in] */ LONG options,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **runJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ListProcessesInGuest )( 
            IVM2 * This,
            /* [in] */ LONG options,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **listJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *KillProcessInGuest )( 
            IVM2 * This,
            /* [in] */ ULONGLONG pid,
            /* [in] */ LONG options,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **killJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RunScriptInGuest )( 
            IVM2 * This,
            /* [in] */ BSTR interrpeter,
            /* [in] */ BSTR scriptText,
            /* [in] */ LONG options,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **runJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *OpenUrlInGuest )( 
            IVM2 * This,
            /* [in] */ BSTR url,
            /* [in] */ LONG windowState,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **openJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CopyFileFromHostToGuest )( 
            IVM2 * This,
            /* [in] */ BSTR hostPathName,
            /* [in] */ BSTR guestPathName,
            /* [in] */ LONG options,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **copyJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CopyFileFromGuestToHost )( 
            IVM2 * This,
            /* [in] */ BSTR guestPathName,
            /* [in] */ BSTR hostPathName,
            /* [in] */ LONG options,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **copyJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DeleteFileInGuest )( 
            IVM2 * This,
            /* [in] */ BSTR guestPathName,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **deleteJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *FileExistsInGuest )( 
            IVM2 * This,
            /* [in] */ BSTR guestPathName,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **fileJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RenameFileInGuest )( 
            IVM2 * This,
            /* [in] */ BSTR oldName,
            /* [in] */ BSTR newName,
            /* [in] */ LONG options,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **renameJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateTempFileInGuest )( 
            IVM2 * This,
            /* [in] */ LONG options,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **createJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ListDirectoryInGuest )( 
            IVM2 * This,
            /* [in] */ BSTR pathName,
            /* [in] */ LONG options,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **listJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateDirectoryInGuest )( 
            IVM2 * This,
            /* [in] */ BSTR pathName,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **createJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DeleteDirectoryInGuest )( 
            IVM2 * This,
            /* [in] */ BSTR pathName,
            /* [in] */ LONG options,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **deleteJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *DirectoryExistsInGuest )( 
            IVM2 * This,
            /* [in] */ BSTR pathName,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **directoryJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *ReadVariable )( 
            IVM2 * This,
            /* [in] */ LONG variableType,
            /* [in] */ BSTR name,
            /* [in] */ LONG options,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **readJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *WriteVariable )( 
            IVM2 * This,
            /* [in] */ LONG variableType,
            /* [in] */ BSTR valueName,
            /* [in] */ BSTR value,
            /* [in] */ LONG options,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **writeJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetNumRootSnapshots )( 
            IVM2 * This,
            /* [out] */ LONG *result,
            /* [retval][out] */ ULONGLONG *error);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetRootSnapshot )( 
            IVM2 * This,
            /* [in] */ LONG index,
            /* [out] */ ISnapshot **snapshot,
            /* [retval][out] */ ULONGLONG *error);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetCurrentSnapshot )( 
            IVM2 * This,
            /* [out] */ ISnapshot **snapshot,
            /* [retval][out] */ ULONGLONG *error);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetNamedSnapshot )( 
            IVM2 * This,
            /* [in] */ BSTR name,
            /* [out] */ ISnapshot **snapshot,
            /* [retval][out] */ ULONGLONG *error);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RemoveSnapshot )( 
            IVM2 * This,
            /* [in] */ ISnapshot *snapshot,
            /* [in] */ LONG options,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **removeJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RevertToSnapshot )( 
            IVM2 * This,
            /* [in] */ ISnapshot *snapshot,
            /* [in] */ LONG options,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **revertJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CreateSnapshot )( 
            IVM2 * This,
            /* [in] */ BSTR name,
            /* [in] */ BSTR description,
            /* [in] */ LONG options,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **createJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *EnableSharedFolders )( 
            IVM2 * This,
            /* [in] */ VARIANT_BOOL enabled,
            /* [in] */ LONG options,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **enableJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetNumSharedFolders )( 
            IVM2 * This,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **getJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetSharedFolderState )( 
            IVM2 * This,
            /* [in] */ LONG index,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **getJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *SetSharedFolderState )( 
            IVM2 * This,
            /* [in] */ BSTR shareName,
            /* [in] */ BSTR hostPathName,
            /* [in] */ LONG flags,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **setJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *AddSharedFolder )( 
            IVM2 * This,
            /* [in] */ BSTR shareName,
            /* [in] */ BSTR hostPathName,
            /* [in] */ LONG flags,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **addJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *RemoveSharedFolder )( 
            IVM2 * This,
            /* [in] */ BSTR shareName,
            /* [in] */ LONG flags,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **removeJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *UpgradeVirtualHardware )( 
            IVM2 * This,
            /* [in] */ LONG options,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **upgradeJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *InstallTools )( 
            IVM2 * This,
            /* [in] */ LONG options,
            /* [in] */ BSTR commandLineArgs,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **installJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Pause )( 
            IVM2 * This,
            /* [in] */ LONG options,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **pauseJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Unpause )( 
            IVM2 * This,
            /* [in] */ LONG options,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **unpauseJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IVM2 * This,
            /* [in] */ ISnapshot *snapshot,
            /* [in] */ LONG cloneType,
            /* [in] */ BSTR destConfigPathName,
            /* [in] */ LONG cloneOptions,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **cloneJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *BeginRecording )( 
            IVM2 * This,
            /* [in] */ BSTR name,
            /* [in] */ BSTR description,
            /* [in] */ LONG options,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **recordingJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *EndRecording )( 
            IVM2 * This,
            /* [in] */ LONG options,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **recordingJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *BeginReplay )( 
            IVM2 * This,
            /* [in] */ ISnapshot *recording,
            /* [in] */ LONG options,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **replayJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *EndReplay )( 
            IVM2 * This,
            /* [in] */ LONG options,
            /* [in] */ IVixHandle *propertyList,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **replayJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *GetFileInfoInGuest )( 
            IVM2 * This,
            /* [in] */ BSTR pathName,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **getFileInfoJob);
        
        /* [helpstring][id] */ HRESULT ( STDMETHODCALLTYPE *CaptureScreenImage )( 
            IVM2 * This,
            /* [in] */ LONG captureType,
            /* [in] */ IVixHandle *additionalProperties,
            /* [in] */ ICallback *jobDoneCallback,
            /* [retval][out] */ IJob **installJob);
        
        END_INTERFACE
    } IVM2Vtbl;

    interface IVM2
    {
        CONST_VTBL struct IVM2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IVM2_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IVM2_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IVM2_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IVM2_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IVM2_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IVM2_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IVM2_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define IVM2_PowerOn(This,powerOnOptions,propertyList,jobDoneCallback,powerJob)	\
    ( (This)->lpVtbl -> PowerOn(This,powerOnOptions,propertyList,jobDoneCallback,powerJob) ) 

#define IVM2_PowerOff(This,powerOffOptions,jobDoneCallback,powerJob)	\
    ( (This)->lpVtbl -> PowerOff(This,powerOffOptions,jobDoneCallback,powerJob) ) 

#define IVM2_Reset(This,resetOptions,jobDoneCallback,resetJob)	\
    ( (This)->lpVtbl -> Reset(This,resetOptions,jobDoneCallback,resetJob) ) 

#define IVM2_Suspend(This,suspendOptions,jobDoneCallback,suspendJob)	\
    ( (This)->lpVtbl -> Suspend(This,suspendOptions,jobDoneCallback,suspendJob) ) 

#define IVM2_Delete(This,deleteOptions,jobDoneCallback,deleteJob)	\
    ( (This)->lpVtbl -> Delete(This,deleteOptions,jobDoneCallback,deleteJob) ) 

#define IVM2_WaitForToolsInGuest(This,timeoutInSeconds,jobDoneCallback,waitJob)	\
    ( (This)->lpVtbl -> WaitForToolsInGuest(This,timeoutInSeconds,jobDoneCallback,waitJob) ) 

#define IVM2_LoginInGuest(This,userName,password,options,jobDoneCallback,loginJob)	\
    ( (This)->lpVtbl -> LoginInGuest(This,userName,password,options,jobDoneCallback,loginJob) ) 

#define IVM2_LogoutFromGuest(This,jobDoneCallback,logoutJob)	\
    ( (This)->lpVtbl -> LogoutFromGuest(This,jobDoneCallback,logoutJob) ) 

#define IVM2_RunProgramInGuest(This,guestProgramName,commandLineArgs,options,propertyList,jobDoneCallback,runJob)	\
    ( (This)->lpVtbl -> RunProgramInGuest(This,guestProgramName,commandLineArgs,options,propertyList,jobDoneCallback,runJob) ) 

#define IVM2_ListProcessesInGuest(This,options,jobDoneCallback,listJob)	\
    ( (This)->lpVtbl -> ListProcessesInGuest(This,options,jobDoneCallback,listJob) ) 

#define IVM2_KillProcessInGuest(This,pid,options,jobDoneCallback,killJob)	\
    ( (This)->lpVtbl -> KillProcessInGuest(This,pid,options,jobDoneCallback,killJob) ) 

#define IVM2_RunScriptInGuest(This,interrpeter,scriptText,options,propertyList,jobDoneCallback,runJob)	\
    ( (This)->lpVtbl -> RunScriptInGuest(This,interrpeter,scriptText,options,propertyList,jobDoneCallback,runJob) ) 

#define IVM2_OpenUrlInGuest(This,url,windowState,propertyList,jobDoneCallback,openJob)	\
    ( (This)->lpVtbl -> OpenUrlInGuest(This,url,windowState,propertyList,jobDoneCallback,openJob) ) 

#define IVM2_CopyFileFromHostToGuest(This,hostPathName,guestPathName,options,propertyList,jobDoneCallback,copyJob)	\
    ( (This)->lpVtbl -> CopyFileFromHostToGuest(This,hostPathName,guestPathName,options,propertyList,jobDoneCallback,copyJob) ) 

#define IVM2_CopyFileFromGuestToHost(This,guestPathName,hostPathName,options,propertyList,jobDoneCallback,copyJob)	\
    ( (This)->lpVtbl -> CopyFileFromGuestToHost(This,guestPathName,hostPathName,options,propertyList,jobDoneCallback,copyJob) ) 

#define IVM2_DeleteFileInGuest(This,guestPathName,jobDoneCallback,deleteJob)	\
    ( (This)->lpVtbl -> DeleteFileInGuest(This,guestPathName,jobDoneCallback,deleteJob) ) 

#define IVM2_FileExistsInGuest(This,guestPathName,jobDoneCallback,fileJob)	\
    ( (This)->lpVtbl -> FileExistsInGuest(This,guestPathName,jobDoneCallback,fileJob) ) 

#define IVM2_RenameFileInGuest(This,oldName,newName,options,propertyList,jobDoneCallback,renameJob)	\
    ( (This)->lpVtbl -> RenameFileInGuest(This,oldName,newName,options,propertyList,jobDoneCallback,renameJob) ) 

#define IVM2_CreateTempFileInGuest(This,options,propertyList,jobDoneCallback,createJob)	\
    ( (This)->lpVtbl -> CreateTempFileInGuest(This,options,propertyList,jobDoneCallback,createJob) ) 

#define IVM2_ListDirectoryInGuest(This,pathName,options,jobDoneCallback,listJob)	\
    ( (This)->lpVtbl -> ListDirectoryInGuest(This,pathName,options,jobDoneCallback,listJob) ) 

#define IVM2_CreateDirectoryInGuest(This,pathName,propertyList,jobDoneCallback,createJob)	\
    ( (This)->lpVtbl -> CreateDirectoryInGuest(This,pathName,propertyList,jobDoneCallback,createJob) ) 

#define IVM2_DeleteDirectoryInGuest(This,pathName,options,jobDoneCallback,deleteJob)	\
    ( (This)->lpVtbl -> DeleteDirectoryInGuest(This,pathName,options,jobDoneCallback,deleteJob) ) 

#define IVM2_DirectoryExistsInGuest(This,pathName,jobDoneCallback,directoryJob)	\
    ( (This)->lpVtbl -> DirectoryExistsInGuest(This,pathName,jobDoneCallback,directoryJob) ) 

#define IVM2_ReadVariable(This,variableType,name,options,jobDoneCallback,readJob)	\
    ( (This)->lpVtbl -> ReadVariable(This,variableType,name,options,jobDoneCallback,readJob) ) 

#define IVM2_WriteVariable(This,variableType,valueName,value,options,jobDoneCallback,writeJob)	\
    ( (This)->lpVtbl -> WriteVariable(This,variableType,valueName,value,options,jobDoneCallback,writeJob) ) 

#define IVM2_GetNumRootSnapshots(This,result,error)	\
    ( (This)->lpVtbl -> GetNumRootSnapshots(This,result,error) ) 

#define IVM2_GetRootSnapshot(This,index,snapshot,error)	\
    ( (This)->lpVtbl -> GetRootSnapshot(This,index,snapshot,error) ) 

#define IVM2_GetCurrentSnapshot(This,snapshot,error)	\
    ( (This)->lpVtbl -> GetCurrentSnapshot(This,snapshot,error) ) 

#define IVM2_GetNamedSnapshot(This,name,snapshot,error)	\
    ( (This)->lpVtbl -> GetNamedSnapshot(This,name,snapshot,error) ) 

#define IVM2_RemoveSnapshot(This,snapshot,options,jobDoneCallback,removeJob)	\
    ( (This)->lpVtbl -> RemoveSnapshot(This,snapshot,options,jobDoneCallback,removeJob) ) 

#define IVM2_RevertToSnapshot(This,snapshot,options,propertyList,jobDoneCallback,revertJob)	\
    ( (This)->lpVtbl -> RevertToSnapshot(This,snapshot,options,propertyList,jobDoneCallback,revertJob) ) 

#define IVM2_CreateSnapshot(This,name,description,options,propertyList,jobDoneCallback,createJob)	\
    ( (This)->lpVtbl -> CreateSnapshot(This,name,description,options,propertyList,jobDoneCallback,createJob) ) 

#define IVM2_EnableSharedFolders(This,enabled,options,jobDoneCallback,enableJob)	\
    ( (This)->lpVtbl -> EnableSharedFolders(This,enabled,options,jobDoneCallback,enableJob) ) 

#define IVM2_GetNumSharedFolders(This,jobDoneCallback,getJob)	\
    ( (This)->lpVtbl -> GetNumSharedFolders(This,jobDoneCallback,getJob) ) 

#define IVM2_GetSharedFolderState(This,index,jobDoneCallback,getJob)	\
    ( (This)->lpVtbl -> GetSharedFolderState(This,index,jobDoneCallback,getJob) ) 

#define IVM2_SetSharedFolderState(This,shareName,hostPathName,flags,jobDoneCallback,setJob)	\
    ( (This)->lpVtbl -> SetSharedFolderState(This,shareName,hostPathName,flags,jobDoneCallback,setJob) ) 

#define IVM2_AddSharedFolder(This,shareName,hostPathName,flags,jobDoneCallback,addJob)	\
    ( (This)->lpVtbl -> AddSharedFolder(This,shareName,hostPathName,flags,jobDoneCallback,addJob) ) 

#define IVM2_RemoveSharedFolder(This,shareName,flags,jobDoneCallback,removeJob)	\
    ( (This)->lpVtbl -> RemoveSharedFolder(This,shareName,flags,jobDoneCallback,removeJob) ) 

#define IVM2_UpgradeVirtualHardware(This,options,jobDoneCallback,upgradeJob)	\
    ( (This)->lpVtbl -> UpgradeVirtualHardware(This,options,jobDoneCallback,upgradeJob) ) 

#define IVM2_InstallTools(This,options,commandLineArgs,jobDoneCallback,installJob)	\
    ( (This)->lpVtbl -> InstallTools(This,options,commandLineArgs,jobDoneCallback,installJob) ) 


#define IVM2_Pause(This,options,propertyList,jobDoneCallback,pauseJob)	\
    ( (This)->lpVtbl -> Pause(This,options,propertyList,jobDoneCallback,pauseJob) ) 

#define IVM2_Unpause(This,options,propertyList,jobDoneCallback,unpauseJob)	\
    ( (This)->lpVtbl -> Unpause(This,options,propertyList,jobDoneCallback,unpauseJob) ) 

#define IVM2_Clone(This,snapshot,cloneType,destConfigPathName,cloneOptions,propertyList,jobDoneCallback,cloneJob)	\
    ( (This)->lpVtbl -> Clone(This,snapshot,cloneType,destConfigPathName,cloneOptions,propertyList,jobDoneCallback,cloneJob) ) 

#define IVM2_BeginRecording(This,name,description,options,propertyList,jobDoneCallback,recordingJob)	\
    ( (This)->lpVtbl -> BeginRecording(This,name,description,options,propertyList,jobDoneCallback,recordingJob) ) 

#define IVM2_EndRecording(This,options,propertyList,jobDoneCallback,recordingJob)	\
    ( (This)->lpVtbl -> EndRecording(This,options,propertyList,jobDoneCallback,recordingJob) ) 

#define IVM2_BeginReplay(This,recording,options,propertyList,jobDoneCallback,replayJob)	\
    ( (This)->lpVtbl -> BeginReplay(This,recording,options,propertyList,jobDoneCallback,replayJob) ) 

#define IVM2_EndReplay(This,options,propertyList,jobDoneCallback,replayJob)	\
    ( (This)->lpVtbl -> EndReplay(This,options,propertyList,jobDoneCallback,replayJob) ) 

#define IVM2_GetFileInfoInGuest(This,pathName,jobDoneCallback,getFileInfoJob)	\
    ( (This)->lpVtbl -> GetFileInfoInGuest(This,pathName,jobDoneCallback,getFileInfoJob) ) 

#define IVM2_CaptureScreenImage(This,captureType,additionalProperties,jobDoneCallback,installJob)	\
    ( (This)->lpVtbl -> CaptureScreenImage(This,captureType,additionalProperties,jobDoneCallback,installJob) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __IVM2_INTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_VixLib;

#ifdef __cplusplus

class DECLSPEC_UUID("6874E949-7186-4308-A1B9-D55A91F60728")
VixLib;
#endif
#endif /* __VixCOM_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif


