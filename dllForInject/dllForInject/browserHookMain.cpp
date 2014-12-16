#undef UNICODE

// ------------------ INCLUDES ------------------------
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include "MinHook.h"
#include <stdio.h>
#include <conio.h>
#include <tchar.h>
#include <WinInet.h>

#include "Logger.h"


// ------------------ LIBRARIES -----------------------
#pragma comment(lib, "user32.lib")
// link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Wininet.lib")
#if defined _M_X64
#pragma comment(lib, "libMinHook.x64.lib")
#elif defined _M_IX86
#pragma comment(lib, "libMinHook.x86.lib")
#endif



#define MAX_BUFFER_SIZE 1024 //1k
#define INIT_URL_NUM 10


// ----------------------------------- GLOBALS -----------------------------------------------
static char **g_restrSitesArray;
static int g_numberOfSites;

static const char *g_logfilename = "F:\\gitmain\\ProcessControlProj\\ProcessControl\\ProcessControl\\logging\\dll_log.txt";


//----------------------------------------- GET ADDR INFO HOOK FUNC-------------------------------------------
// typedefing pointer on the function
typedef int(WINAPI *GETADDRINF)(_In_opt_  PCSTR, _In_opt_  PCSTR,_In_opt_ const ADDRINFOA *, _Out_ PADDRINFOA *);
// Pointer to trampoline function, it will be used to call real (not detoured) function
GETADDRINF mGetAddrInfo = NULL;


// Detoured function, real MyGetAddrInfo is replaced with that function
__declspec(dllexport) int WINAPI MyGetAddrInfo(_In_opt_  PCSTR pNodeName,
    _In_opt_  PCSTR pServiceName,
    _In_opt_  const ADDRINFOA *pHints,
    _Out_     PADDRINFOA *ppResult)
{
    int compareAns = 0;

    for (int i = 0; i < g_numberOfSites; ++i) {
        if (strcmp(pNodeName, g_restrSitesArray[i]) == 0) {
            Sleep(100);
            SetLastError(WSAHOST_NOT_FOUND);
            return WSAHOST_NOT_FOUND;
        }
    }
    return mGetAddrInfo(pNodeName, pServiceName, pHints, ppResult);
}


////----------------------------------------- CONNECT HOOK FUNC-------------------------------------------
//typedef int(*CONNECT)(_In_  SOCKET, _In_  const struct sockaddr*, _In_  int);
//
//CONNECT fpConnect = NULL;
//
//int detourConnect(_In_  SOCKET s, _In_  const struct sockaddr *name, _In_  int namelen)
//{
//    return fpConnect(s, name, namelen);
//}


int readRestrictedUrls()
{
    // Open the named pipe
    // Most of these parameters aren't very relevant for pipes.
    
    //MessageBox(
    //    NULL,
    //    "HERE!!!!",
    //    "INFO",
    //    MB_OK
    //    );

    //HANDLE pipe = CreateFile(
    //    "\\\\.\\pipe\\my_restricted_urls_pipe",
    //    GENERIC_READ,                                 // only need read access
    //    FILE_SHARE_READ | FILE_SHARE_WRITE,
    //    NULL,
    //    OPEN_EXISTING,
    //    FILE_ATTRIBUTE_NORMAL,
    //    NULL
    //    );

    //if (pipe == INVALID_HANDLE_VALUE) {
    //    logger.log("Error: cannot connect to pipe");
    //    return 1;
    //}

    //// The read operation will block until there is data to read
    //char buffer[MAX_BUFFER_SIZE + 1];
    //DWORD numBytesRead = 0;
    //int cur_mem_size = INIT_URL_NUM;
    //char** buf;
    //g_numberOfSites = 0;

    //g_restrSitesArray = (char** ) malloc(INIT_URL_NUM * sizeof(char* ));
    //while (true)
    //{
    //    BOOL result = ReadFile(
    //        pipe,
    //        buffer,                                 // the data from the pipe will be put here
    //        (MAX_BUFFER_SIZE - 1) * sizeof(char),   // number of bytes allocated
    //        &numBytesRead,                          // this will store number of bytes actually read
    //        NULL                                    // not using overlapped IO
    //        );

    //    if (result) {
    //        buffer[numBytesRead / sizeof(char)] = '\0'; // null terminate the string
    //        g_numberOfSites += 1;

    //        logger.log("Info: URL readed from pipe = ");
    //        logger.log(buffer);

    //        if (cur_mem_size < g_numberOfSites) {
    //            buf = (char** ) realloc(g_restrSitesArray, (cur_mem_size + INIT_URL_NUM) * sizeof(char* ));

    //            cur_mem_size += INIT_URL_NUM;

    //            if (buf != NULL) {
    //                g_restrSitesArray = buf;
    //            }
    //            else {
    //                logger.log("Error: cannot reallocate url's array");
    //                return 1;
    //            }
    //        }
    //        g_restrSitesArray[g_numberOfSites - 1] = (char* ) malloc(numBytesRead + 2);
    //        strcpy_s(g_restrSitesArray[g_numberOfSites - 1], numBytesRead + 1, buffer);
    //    }
    //    else {
    //        return 1;
    //    }
    //}

    //// Close our pipe handle
    //CloseHandle(pipe);

    return 0;
}


BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved)
{
    if (dwReason == DLL_PROCESS_ATTACH) {
        Logger logger(g_logfilename, true);
        logger.log("INFO: Dll attach.");
        // reading urls from pipe, which is opened by Process Control process
        readRestrictedUrls();

        // Creating a HOOK
        if (MH_Initialize() != MH_OK) {
            logger.log("ERROR: Cannot initialize minhook!");
            return 1;
        }
        if (MH_CreateHook(&getaddrinfo, &MyGetAddrInfo, reinterpret_cast<void**>(&mGetAddrInfo)) != MH_OK) {
            logger.log("INFO: Cannot create hook!");
            return 1;
        }
        if (MH_EnableHook(&getaddrinfo) != MH_OK) {
            logger.log("INFO: Cannot enable hook!");
            return 1;
        }

        //if (MH_CreateHook(&connect, &detourConnect, reinterpret_cast<void**>(&fpConnect)) != MH_OK)
        //    return 1;
        //if (MH_EnableHook(&connect) != MH_OK)
        //    return 1;

    } else if (dwReason == DLL_PROCESS_DETACH) {
        Logger logger(g_logfilename);
        logger.log("INFO: Dll detach.");

        if (g_restrSitesArray != NULL) {
            free(g_restrSitesArray);
        }
        if (MH_DisableHook(&getaddrinfo) != MH_OK) {
            logger.log("ERROR: Cannot disable hook!");
            return 1;
        }

        if (MH_Uninitialize() != MH_OK) {
            logger.log("ERROR: Cannot uninit minhook!");
            return 1;
        }
    }

    return TRUE;
}