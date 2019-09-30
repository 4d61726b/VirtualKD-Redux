/*! \file kdvm.h
    \author Ivan Shcherbakov (Bazis)
    $Id: kdvm.h,v 1.11 2009-07-09 14:26:45 Administrator Exp $
    \brief Contains definitions for extra functions exported by KDVM.DLL
*/

/*! \mainpage Windows Kernel Debugger booster for VMWare and VirtualBox
	\section Overview
	VirtualKD allows speeding up Windows kernel module debugging using VMWare and VirtualBox virtual machine. A basic description of the project and using guidelines are located at the project
	web page: <a href="http://virtualkd.sysprogs.org/">http://virtualkd.sysprogs.org/</a>, while this pages contain information about source code structure and building/debugging hints.
	\section sourcestruct Source structure
	The directory with source code contains the following subdirectories:
	<table>
	<tr><td>Subdirectory</td><td>Contents</td></tr>
	<tr><td>BUNDLE</td><td>Contains a batch for building full set of binary files</td></tr>
	<tr><td>DATABASE</td><td>Contains \ref patching "patching signatures" for different versions of VMWare</td></tr>
	<tr><td>KDCLIENT</td><td>Sources for the DLL that is loaded to VMWARE-VMX.EXE and implements host-side support</td></tr>
	<tr><td>KDPATCH</td><td>Sources for a legacy driver that patches KDCOM.DLL redirecting requests to KDVM.DLL</td></tr>
	<tr><td>KDVM</td><td>Sources for the KD extension DLL that sends all debug packets over a fast interface</td></tr>
	<tr><td>PIPEPROXY</td><td>Sources for a \ref pipeproxy "Named Pipe Proxy" useful in debugging</td></tr>
	<tr><td>RPCDISPATCH</td><td>Source files for host part of debug support. Used by KDCLIENT and PIPEPROXY modules</td></tr>
	<tr><td>VMMON</td><td>Sources for the Virtual Machine Monitor program</td></tr>
	<tr><td>VMXPATCH</td><td>Sources for a simple console program for patching VMWare</td></tr>
	</table>
	You can read the information about the role of each component in the big picture \ref operation "in this article" or explore documentation pages for each class.

	\page operation Principle of operation and main components
	\section normalop Normal kernel debugging
	Normally, when the kernel of the OS being debugged wants to send a packet to the debugger, it calls the KdSendPacket() function inside a corrseponding extension DLL (KDCOM.DLL for COM port-based debugging;
	KD1394.DLL for 1394-based). The code in DLL sends data over the physical port by directly accessing the hardware (avoiding the use of any other kernel functions that can be debugged themselves). VMWare emulates
	a COM port, so all data sent to it using conventional UART controller interface is directed to a named pipe available on host computer. The debugger (WinDBG/KD) reads data from the pipe and interprets it.
	All communication between debugger and OS being debugged is framed in packets, so the lowest level is absolutely transparent to the kernel calling KdSendPacket()/KdReceivePacket(). Most  debugger functionality
	exposed by KD is contained inside the DBGENG.DLL library, including the code parsing reading stream data from the pipe and restoring packets from it.
	\section boostop Boosted kernel debugging
	As it was described before, normally kernel uses the conventional UART controller interface to send data to the debugger. VirtualKD project overrides this behavior.<br/>
	First of all, the <b>KDVM.DLL</b> library implements all the functions exported by KDCOM.DLL and KD1394.DLL. As it is described in <b>readme.txt</b>, there are two ways of making Windows use our DLL:
	either to specify <b>/DEBUGPORT=VM</b> in <b>BOOT.INI </b>, or using the \ref kdpatch "KDPATCH driver".<br/>
	When KDSendPacket() or KdReceivePacket() inside the KDVM.DLL is called, it serializes all its parameters in a single data block and sends it to VMWare using the \ref guestrpc "GuestRPC mechanism".
	On the host side, KDCLIENT.DLL receives the data block and calls the IKdComDispatcher::KdSendPacket() or IKdComDispatcher::KdReceivePacket() methods. This methods are implemented inside the 
	KdComDispatcher class. Basically, the KDVM.DLL marshalls the calls, while KDCLIENT unmarshalls and executes them. This behaviour can be slightly modified using a \ref pipeproxy PIPEPROXY module
	to simplify debugging.<br/>
	The KdSendPacket()/KdReceivePacket() contained in KdComDispatcher class perform the same functionality as the corresponding methods in KDCOM.DLL with one important difference: instead of sending/receiving
	the packets using a slow UART interface, they operate with a named pipe directly (they already run on the host side, as KDVM.DLL/KDCLIENT.DLL perform call marshalling), transferring the debugging data
	very rapidly. From all other points of view, the functionality is identical to KDCOM.DLL one: packet building, checksum computation, packet acknowledging, resynchronizing, etc.<br/>
	Schematically, a single debug packet transfer can be represented by the following diagram:
	\msc
	Kernel,"KDVM.DLL",KdRpcDispatcher,KdComDispatcher,WinDBG;
	Kernel=>"KDVM.DLL" [label="KdSendPacket()", URL="\ref KdSendPacket()"];
	"KDVM.DLL"=>KdRpcDispatcher [label="Marshal params", URL="\ref KdSendPacket()"];
	KdRpcDispatcher=>KdComDispatcher  [label="Unmarshal params", URL="\ref KdComDispatcher::KdSendPacket()"];
	KdComDispatcher=>WinDBG  [label="KDCOM packet", URL="\ref KdComDispatcher::KdSendPacket()"];
	KdComDispatcher<=WinDBG  [label="ACK packet", URL="\ref KdComDispatcher::KdSendPacket()"];
	KdRpcDispatcher<=KdComDispatcher  [label="Return control", URL="\ref KdComDispatcher::KdSendPacket()"];
	"KDVM.DLL"<=KdRpcDispatcher [label="Marshal result", URL="\ref KdSendPacket()"];
	Kernel<="KDVM.DLL" [label="Return control", URL="\ref KdSendPacket()"];
	\endmsc
	Kernel and KDVM.DLL run at the guest (virtual) side, while and KdRpcDispatcher, KdComDispatcher are normally a part of KDCLIENT.DLL and run on the host side.
	\section kdpatch KDPATCH driver
	The KDPATCH driver locates KDCOM.DLL loaded in memory by NTOSKRNL, finds the KdSendPacket() and KdReceivePacket() functions, and inserts jump instructions in the beginning of them to divert control
	to the corresponding functions in KDVM.DLL. The driver supports unloading, in that case it simply restores the original contents of KdSendPacket()/KdReceivePacket() allowing the COM port-based debugging
	to used again.
	\section guestrpc GuestRPC mechanism
	VMWare supports the so-called backdoor mechanism allowing to exchange various data between virtual machine and a host. The backdoor interface is described 
	<a href="http://chitchat.at.infoseek.co.jp/vmware/backdoor.html">here</a> and is used in the <a href="http://open-vm-tools.sourceforge.net/">open-vm-tools</a> project. Aside some simple functions,
	such as querying/setting mouse cursor position, the backdoor interface allows executing commands on host. This mechanism is called GuestRPC. A simple GuestRPC client is called <b>vmwareservice.exe</b> and
	is located in VMWare Tools folder on the guest machine. It allows executing several GuestRPC commands, for example, the following command returns the IP address of the virtual machine:<br/>
<pre>VMWareService.exe --cmd info-get guestinfo.ip</pre>
	The <b>info-get</b> here is the command name, while the <b>"guestinfo.ip"</b> is the command parameter. VMWare process (vmware-vmx.exe) holds an internal table of handlers, that associates each command name
	with a corresponding handler function that receives the command parameters and can also return some data back to VM. Both command name and arguments can be either printable strings, or binary data sequences,
	as the handler table contains both pointer to the command name and its length. However, according to comments in open-vm-tools, in older versions of VMWare the command names were required to be printable and
	to contain no spaces.
	\section patching Patching VMWare
	VMWare patching is implemented using the KDCLIENT.DLL library. When it is loaded to VMWARE-VMX.EXE process, it locates the GuestRPC handler table, finds an unused entry and initializes it with a pointer
	to VirtualKD command handler inside the DLL. As the handler table is an internal part of VMWARE-VMX process and is not exported, its address is unknown. KDCLIENT.DLL uses a kind of a heuristic algorithm
	to detect the table location. The algorithm is implemented in the RPCTableManager class.<br/>
	Once a table is found, the information about it is saved in a patch database. The information about the VMWARE-VMX.EXE size and timestamp is used as a key, and the GuestRPC handler table address and size are
	saved. This mechanism speeds up patching the table when a previously used version of VMWARE-VMX.EXE is detected, as the information about the table is directly read from the corresponding .VMPATCH file
	instead of performing a long scanning operation.
	\section pipeproxy Named Pipe Proxy	
	As it was described above, the KDCOM packet generation/processing logic is implemented inside KDCLIENT.DLL library. This fact makes it not that easy to debug this logic. After changing the source code, the
	original DLL needs to be unloaded from VMWARE-VMX.EXE, new one should be compiled and reloaded. Plus, Edit-And-Continue does not always work correctly in this configuration. To simplify debugging, the so-called
	Pipe Proxy is provided. In proxy configuration, KDCLIENT.DLL simply resends all data blocks containing marshalled call parameters to a named pipe ("kdvmware_proxypipe") using CallNamedPipe() function.
	All further processing is performed inside the PIPEPROXY.EXE process (KdRpcDispatcher and KdComDispatcher classes). As the CallNamedPipe() function is used, nothing bad happens if the PIPEPROXY.EXE
	is terminated. CallNamedPipe() returns an error, KdSendPacket() retries the call from the VM until a new instance of PIPEPROXY.EXE ready to serve requests is started. PIPEPROXY.EXE creates another named
	pipe compatible with WinDBG and translates low-level requests containing parameters for KdSendPacket()/KdReceivePacket() into KDCOM packets transferred using the second pipe.<br/>
	A typical debugging session with PIPEPROXY.EXE consists of building VMMON.EXE and KDCLIENT.DLL in the "Debug (Proxy mode)" configuration, starting VMMON and then loading PIPEPROXY, debugging it,
	changing, unloading, recompiling and loading again. No other changes are required. The proxy functionality is implemented in RpcProxy class.
	\section vm32to64 32-to-64 call interface
	The 32-to-64 call interface allows 32-bit processes (such as MS Visual Studio running <a href="http://visualddk.sysprogs.org">VisualDDK</a>) patching 64-bit VMWare processes by invoking
	patch functions from KDCLIENT64.DLL via 64-bit RUNDLL32.EXE.
	When a function, requiring 64-bit part, is invoked from a 32-bit library, it automatically launches 64-bit RUNDLL32.EXE, that invokes the KdClient32To64Entry() function. When such a call
	is complete, the result is reported via a named pipe.
*/

#pragma once

ULONG KdVMGetActiveCallCount();
