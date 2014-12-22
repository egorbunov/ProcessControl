# Process control
Simple application for controlling your computer activity :)

**Capabilities** (23 december, 2014):
* You can create "task" with progress and list of programs and sites, which can't be opened during that "task".
* If program is restricted, it can't be opened (it's automatically closes)
* If site is restricted, it sometimes can't be opened in Chrome browser :D

### Technologies
* [min-hook][mh] library for creating simple function hooks
* [dllinjector][di] for injecting dll's into processes
* qt 5.2.1 msvc 2010

### ...
App build in MSVS 2012 with help of QT-plugin for MSVS.

[mh]: http://www.codeproject.com/Articles/44326/MinHook-The-Minimalistic-x-x-API-Hooking-Libra
[di]: https://github.com/OpenSecurityResearch/dllinjector
