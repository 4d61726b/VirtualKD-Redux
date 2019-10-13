# VirtualKD-Redux Tutorial

## Guest VM

![](TutorialGuest.gif)

1. Copy the target folder to the guest VM running inside VMware Workstation. If the Guest VM is running a 32-bit OS, copy "target32". Otherwise, if it's running a 64-bit OS, then copy "target64" instead.

2. Run "vminstall.exe" in the guest VM. If using Windows 10, ensure that "Replace kdcom.dll" is checked. Once the installer is complete, allow the guest VM to restart.

3. At the boot manager prompt, ensure the "VKD-Redux" entry is selected then press F8 for advanced options.

4. Select "Disable Driver Signature Enforcement" and boot the OS.

:bulb: You can use virtual machine snapshots to avoid having to perform these steps every time. Once the final step is completed and the OS has successfully booted, make a snapshot that can later be restored.

## Host

![](TutorialHost.gif)

1. If your host OS is a 64-bit machine run "vmmon64.exe". Otherwise, if it's running a 32-bit OS, instead run "vmmon32.exe".

2. If this is your first time running VirtualKD-Redux, ensure that the "Debugger path" is properly set. This can be the path to "windbg.exe" installed on your system.

3. Select the VM you want to attach to and then press "Run debugger".

4. Your debugger is now debugging the guest OS. To validate this, issue the break command to insert a breakpoint.
