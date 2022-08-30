/*! \file kdpatch.cpp
    \author Ivan Shcherbakov (Bazis)
    $Id: kdpatch.cpp,v 1.12 2009-06-07 10:06:40 Administrator Exp $
    \brief Contains the driver load/unload code
*/

#include "stdafx.h"
#include <Bazislib/bzshlp/WinKernel/driver.h>
#include <Bazislib/bzshlp/WinKernel/device.h>
#include "kdvmguestlib/kdvm.h"
#include "kdvmguestlib/kdxxx.h"
#include "kdvmguestlib/moduleapi.h"

#include <Bazislib/bzscore/WinKernel/security.h>
#include <BazisLib/bzscore/WinKernel/init.h>

using namespace BazisLib;
using namespace BazisLib::DDK;

//! Simplifies inserting JMP commands in the beginning of functions
/*! This class represents a single patched function (with a JMP instruction inserted in the beginning).
    Typically, after creation the FunctionPatch::Patch() method should be called to patch a function. During patching,
    the class instance stores the data that was replaced by inserting a JMP and restores this data back when the
    object is deleted. That way, each patched function should have a corresponding FunctionPatch instance.
*/
class FunctionPatch
{
private:
    char m_PreviousBytes[5];
    void *m_pAddress;

    //! Allows retreiving read/write access to read-only kernel memory.
    /*! This class allows modifying read-only kernel memory. On x86 targets, all kernel memory is always writable
        by kernel-mode code, however, on x64 targets, some memory (such as code segments) is marked as read-only.
        This class allows retreiving a write-enabled pointer to a read-only memory block.
    */
    class MemoryLocker
    {
    private:
        PMDL m_pMdl;
        PVOID m_pPointer;

    public:
        //! Locks read-only pages in memory and creates an additional read-write mapping
        MemoryLocker(void *pData, ULONG size)
        {
            m_pMdl = IoAllocateMdl(pData, size, FALSE, FALSE, NULL);
            ASSERT(m_pMdl);
            MmProbeAndLockPages(m_pMdl, KernelMode, IoReadAccess);
            m_pPointer = MmMapLockedPagesSpecifyCache(m_pMdl, KernelMode, MmNonCached, NULL, FALSE, NormalPagePriority);
            NTSTATUS status = MmProtectMdlSystemAddress(m_pMdl, PAGE_EXECUTE_READWRITE);
            ASSERT(NT_SUCCESS(status));
        }

        //! Destroys the additional read-write mapping
        ~MemoryLocker()
        {
            MmUnmapLockedPages(m_pPointer, m_pMdl);
            MmUnlockPages(m_pMdl);
            IoFreeMdl(m_pMdl);
        }

        //! Returns a write-enabled pointer to a read-only memory block
        void *GetPointer()
        {
            return m_pPointer;
        }

    };

private:
    //! Replaces the first 5 bytes of a function by a JMP instruction
    /*! This method performs the actual patching and is internally used by Patch() method.
        \param pFunc Specifies the address of a function to be patched
        \param pDest Specifies the target address of the JMP instruction
    */
    static void _PatchFunction(void *pFunc, void *pDest)
    {
        MemoryLocker locker(pFunc, 5);

        INT_PTR off = ((char *)pDest - (char *)pFunc) - 5;
#ifdef _AMD64_
        if ((off > 0x7FFFFFFFLL) || (off < -0x7FFFFFFFLL))
            return;
#endif

        unsigned char *p = (unsigned char *)locker.GetPointer();
        p[0] = 0xE9;

        *((unsigned *)(p + 1)) = (unsigned)off;
    }

public:
    FunctionPatch()
        : m_pAddress(NULL)
    {
    }

    ~FunctionPatch()
    {
        if (m_pAddress)
        {
            MemoryLocker locker(m_pAddress, 5);
            memcpy(locker.GetPointer(), m_PreviousBytes, sizeof(m_PreviousBytes));
        }
    }


    //! Inserts the JMP instruction in the beginning of a function and stores the bytes being overwritten
    /*!
        \param pFunc Specifies the address of a function to be patched
        \param pNewFunc Specifies the target address of the JMP instruction
    */
    bool Patch(void *pFunc, void *pNewFunc)
    {
        if (m_pAddress)
            return false;
        memcpy(m_PreviousBytes, pFunc, sizeof(m_PreviousBytes));
        _PatchFunction(pFunc, pNewFunc);
        m_pAddress = pFunc;
        return true;
    }
};

//! Device object for the KDPATCH driver
class KDPatchDevice : public BazisLib::DDK::Device
{
private:
    FunctionPatch m_RecvPatch, m_SendPatch;
    //! Specifies whether the patching was successful
    bool m_bSucceeded;

public:
    //! Locates and patches packet-related functions in KDCOM.DLL
    KDPatchDevice()
        : Device(FILE_DEVICE_UNKNOWN,
            L"KDPatchDevice")
        , m_bSucceeded(false)
    {
        PVOID lpKD = KernelGetModuleBase("KDCOM.DLL");
        if (!lpKD)
            lpKD = KernelGetModuleBase("KDBAZIS.DLL");
        if (lpKD)
        {
            PVOID lpfSend = KernelGetProcAddress(lpKD, "KdSendPacket");
            PVOID lpfRecv = KernelGetProcAddress(lpKD, "KdReceivePacket");
            if (lpfSend && lpfRecv)
            {
                m_SendPatch.Patch(lpfSend, KdSendPacket);
                m_RecvPatch.Patch(lpfRecv, KdReceivePacket);
                m_bSucceeded = true;
            }
        }
    }

    //! Returns whether the KDCOM.DLL patching was successful
    bool IsPatchingSuccessful()
    {
        return m_bSucceeded;
    }

    //! Restores the original state of KDCOM.DLL functions (via FunctionPatch::~FunctionPatch())
    ~KDPatchDevice()
    {
    }

    virtual NTSTATUS DispatchRoutine(IN IncomingIrp *Irp, IO_STACK_LOCATION *IrpSp)
    {
        return __super::DispatchRoutine(Irp, IrpSp);
    }

};

//! Driver object for KDPATCH driver
class KDPatchDriver : public Driver
{
private:
    KDPatchDevice *pDevice;
    bool m_bPatched;

public:
    KDPatchDriver() : DDK::Driver(false), m_bPatched(false)
    {
        pDevice = NULL;
    }
#define KDPATCH_TEST_SEND_RAW_MSGS
    //! This routine is called when the driver is loaded
    virtual NTSTATUS DriverLoad(IN PUNICODE_STRING RegistryPath) override
    {
        NTSTATUS status = __super::DriverLoad(RegistryPath);
        if (!NT_SUCCESS(status))
            return status;

        /*volatile static int loopme = 0;
        static char sz[1024 + 1];
        memset(sz, 'z', sizeof(sz)-1);
        sz[sizeof(sz)-1]  = 0;
#ifdef KDPATCH_TEST_SEND_RAW_MSGS
        struct MsgHeader
        {
            int PacketSubtype;
            int Code6;
            int MsgLen;
            int Unknown;
        } header = {0x3230, 6, sizeof(sz)-1, 0};
        KD_BUFFER buf1 = {0,}, buf2 = {0,};
        buf1.Length = sizeof(header);
        buf1.pData = (PUCHAR)&header;
        buf2.Length = sizeof(sz)-1;
        buf2.pData = (PUCHAR)sz;
        KD_CONTEXT ctx;
#endif
        __asm int 3;
        while (loopme)
        {
#ifdef KDPATCH_TEST_SEND_RAW_MSGS
            KdSendPacket(3, &buf1, &buf2, &ctx);
#else
            DbgPrint(sz);
#endif
        }//*/

        status = KdDebuggerInitialize0(NULL);
        if (!NT_SUCCESS(status))
        {
            DbgPrint("VirtualKD-Redux: Cannot establish connection with the patch DLL in VMWare process! No point in patching KDCOM.\n");
            return status;
        }
        DbgPrint("VirtualKD-Redux: Connection with host established.");

        /*unsigned char sz[32];
        unsigned char sz2[64];
        KD_BUFFER buf1;
        KD_BUFFER buf2;
        buf1.Length = 0;
        buf2.Length = 0;
        buf1.MaxLength = sizeof(sz);
        buf2.MaxLength = sizeof(sz2);
        buf1.pData = sz;
        buf2.pData = sz2;
        KD_CONTEXT context = {0,};
        context.RetryCount = 5;
        ULONG bytes = 0;
        DbgBreakPoint();
        unsigned xstatus = KdReceivePacket(0x1234, &buf1, &buf2, &bytes, &context);
        context.RetryCount += 2;
        buf1.Length = buf1.MaxLength;
        buf2.Length = buf2.MaxLength;
        KdSendPacket(0x1234, &buf1, NULL, &context);
        return STATUS_INSUFFICIENT_RESOURCES;*/

        DbgPrint(" Patching the KDCOM.DLL...\n");
        pDevice = new KDPatchDevice();
        if (!pDevice || !pDevice->IsPatchingSuccessful())
        {
            delete pDevice;
            pDevice = NULL;
            DbgPrint("\nVirtualKD-Redux: Cannot find KDCOM.DLL loaded in memory! Nothing to patch...\n");
            return STATUS_OBJECT_NAME_NOT_FOUND;
        }
        status = pDevice->RegisterDevice(this);
        if (!NT_SUCCESS(status))
            return status;
        m_bPatched = true;
        return STATUS_SUCCESS;
    }

    //! Unpatches the KDCOM.DLL and waits for all active calls to terminate
    virtual ~KDPatchDriver()
    {
        delete pDevice;
        /*
            We cannot use any reliable type of a lock (like mutex or etc), as it is not a very good idea to call any
            kernel functions from a kernel debugger override function (such as our versions of KdSendPacket()/KdReceivePacket()).
            The simpliest thing we can do is to check whether the active call counter remains at zero for some time.
            As we have unpatched the KDCOM, original functions will be called in a normal way, so this will mean that it is safe
            to unload the driver (it is unlikely that a delay between the jump to our function and the increasing of active
            counter will take that long; however, there is no more reliable way to check it).
        */
        LARGE_INTEGER interval;
        interval.QuadPart = -100000;
        for (int i = 0; i < 10; i++)
        {
            if (KdVMGetActiveCallCount())
                i = 0;
            KeDelayExecutionThread(KernelMode, FALSE, &interval);
        }
        if (m_bPatched)
            DbgPrint("VirtualKD-Redux: Successfully unpatched KDCOM.DLL (as you can see ;])\n");
    }
};

//! Creates an instance of the driver object. Called by BazisLib framework.
DDK::Driver *_stdcall CreateMainDriverInstance()
{
    return new KDPatchDriver();
}

extern "C" NTSTATUS _stdcall DriverEntry(
    IN OUT PDRIVER_OBJECT   DriverObject,
    IN PUNICODE_STRING      RegistryPath
)
{
    return BazisCoreDriverEntry(DriverObject, RegistryPath);
}