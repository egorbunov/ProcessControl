#ifndef DLL_INJECTOR_H_INCLUDED__
#define DLL_INJECTOR_H_INCLUDED__

#include <cstdlib>
#include <windows.h>
#include <cstdio>
#include <tlhelp32.h>
#include <dos.h>
#include <cstdio>
#include <tchar.h>
#include <conio.h>
#include <iostream>
#include <cwchar>
#include <string>
#include "Logger.h"

class DllInjector
{
    Logger *_logger;
    static const int _bufferSize = 1024;
    LPTHREAD_START_ROUTINE AllocWritePath(HANDLE hTargetProcHandle, char* dllPath, LPVOID *lpExecParam);
    int injectDLL(HANDLE hTargetProcHandle, LPTHREAD_START_ROUTINE lpStartExecAddr, LPVOID lpExecParam);
    HANDLE attachToProcess(DWORD procID);
    int SetDebugPrivileges(void);

public:
    int inject(unsigned long processId, std::string dllName);
    void setLogger(Logger *logger);
    DllInjector();
    ~DllInjector();
};

#endif

