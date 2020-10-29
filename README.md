# VirtualKD-Redux
![](https://github.com/4d61726b/VirtualKD-Redux/blob/master/VirtualKD-Redux/Docs/vmmon.png)

## Features

* Support for the latest VMware Workstation Pro
* Support starting with Windows XP up to the latest Windows 10
* Support for the latest [WinDbg Preview](https://www.microsoft.com/en-us/p/windbg-preview/9pgjgd53tn86)
* Toolchain modernization. Builds with Visual Studio 2017
* Simplified build system. No additional libraries required

## Downloads
[Download the latest VirtualKD-Redux release](https://github.com/4d61726b/VirtualKD-Redux/releases)

## Documentation
* A quick and easy tutorial can be found [here](https://github.com/4d61726b/VirtualKD-Redux/blob/master/VirtualKD-Redux/Docs/Tutorial.md)
* Original documentation (May be out of date) can be found [here](http://sysprogs.com/legacy/virtualkd/dox/)

## Build Instructions
### Prerequisites
* Visual Studio 2017
* Windows Driver Kit 7.1.0
### Steps
1. Set the WDK7_PATH environment variable to the WDK 7.1.0 install path
2. Edit make.bat to modify VS150COMNTOOLS path if needed
3. Run make.bat
4. Use binaries produced in Bundle directory
## Issues or Feature Requests
* Issues, bugs, or feature requests can be raised on the [issue tracker on GitHub](https://github.com/4d61726b/VirtualKD-Redux/issues).
