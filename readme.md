# Process control
Simple application for controlling your computer activity :)

**Capabilities** (25 december, 2014):
* You can create "task" with progress and list of programs and sites, which can't be opened during that "task".
* If program is restricted, it can't be opened (it's automatically closes)
* If site is restricted, it sometimes can't be opened in Chrome browser :D
* You can't kill program process from taskmgr.exe

### Technologies
* [min-hook][mh] library for creating simple function hooks
* [dllinjector][di] for injecting dll's into processes
* qt 5.4.0 msvc 2013

### ...
That is 32-bit application for windows 7 (32-64 bit).

The app runs supporting process for hiding itself in taskmgr.exe, because of if OS is 64 bit, it's complicated to inject 64-bit dll from 32-bit process...

App build in MSVS 2013 with help of QT-plugin for MSVS.

### Install
To install application you can use [msi-installer][installer] in installer folder.

[mh]: http://www.codeproject.com/Articles/44326/MinHook-The-Minimalistic-x-x-API-Hooking-Libra
[di]: https://github.com/OpenSecurityResearch/dllinjector
[installer]: https://github.com/egorbunov/ProcessControl/raw/master/installer/ProcessControlInstall86.msi

<div style="text-align:center"><img src ="https://raw.githubusercontent.com/egorbunov/ProcessControl/master/ProcessControl/Resources/mainTray.ico" /></div>