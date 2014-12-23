#undef UNICODE

// ------------------ INCLUDES ------------------------
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <WinInet.h>

#include "MinHook.h"
#include "../../commonUtils/headers/Logger.h"
#include "../../commonUtils/headers/memmapfile.h"


// ------------------ LIBRARIES -----------------------
// link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib") // getaddrinfo

#if defined _M_X64
#pragma comment(lib, "libMinHook.x64.lib")
#elif defined _M_IX86
#pragma comment(lib, "libMinHook.x86.lib")
#endif



#define MAX_BUFFER_SIZE 1024 //1k
#define INIT_URL_NUM 10


// ----------------------------------- GLOBALS -----------------------------------------------
char g_logfilename[125] = "F:\\gitmain\\ProcessControlProj\\ProcessControl\\ProcessControl\\logging\\dll_log";
Logger *g_logger;
bool g_isReady;



//----------------------------------------- GET ADDR INFO HOOK FUNC-------------------------------------------
// typedefing pointer on the function
typedef int(WINAPI *GETADDRINF)(_In_opt_  PCSTR, _In_opt_  PCSTR, _In_opt_ const ADDRINFOA *, _Out_ PADDRINFOA *);
// Pointer to trampoline function, it will be used to call real (not detoured) function
GETADDRINF mGetAddrInfo = NULL;


// Detoured function, real MyGetAddrInfo is replaced with that function
__declspec(dllexport) int WINAPI MyGetAddrInfo(_In_opt_  PCSTR pNodeName,
                                               _In_opt_  PCSTR pServiceName,
                                               _In_opt_  const ADDRINFOA *pHints,
                                               _Out_     PADDRINFOA *ppResult)
{
    return mGetAddrInfo(pNodeName, pServiceName, pHints, ppResult);
}


BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved)
{
    if (dwReason == DLL_PROCESS_ATTACH) {
        unsigned long pid = GetCurrentProcessId(); // hope process id will fit in integer
        char strpid[12];
        _ultoa_s(pid, strpid, 10);
        strcat_s(g_logfilename, strpid);
        strcat_s(g_logfilename, "taskmgr.txt");
        g_logger = new Logger(g_logfilename, true);

        g_logger->log("INFO: Dll process attach.");

        g_logger->log("INFO: initializing hook");
        // Creating a HOOK
        if (MH_Initialize() != MH_OK) {
            g_logger->log("ERROR: Cannot initialize minhook!");
            return 1;
        }
        if (MH_CreateHook(&getaddrinfo, &MyGetAddrInfo, reinterpret_cast<void**>(&mGetAddrInfo)) != MH_OK) {
            g_logger->log("INFO: Cannot create hook!");
            return 1;
        }
        if (MH_EnableHook(&getaddrinfo) != MH_OK) {
            g_logger->log("INFO: Cannot enable hook!");
            return 1;
        }

    }
    else if (dwReason == DLL_THREAD_ATTACH) {
        g_logger->log("INFO: Dll thread attach.");
    }
    else if (dwReason == DLL_PROCESS_DETACH) {
        g_logger->log("INFO: Dll detach.");

        if (MH_DisableHook(&getaddrinfo) != MH_OK) {
            g_logger->log("ERROR: Cannot disable hook!");
            return 1;
        }

        if (MH_Uninitialize() != MH_OK) {
            g_logger->log("ERROR: Cannot uninit minhook!");
            return 1;
        }

        //delete g_logger;
    }
    return TRUE;
}