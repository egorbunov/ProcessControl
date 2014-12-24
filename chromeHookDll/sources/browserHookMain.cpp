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
char **g_urls;
int g_urlNum;
char g_logfilename[100] = "F:\\gitmain\\ProcessControlProj\\ProcessControl\\ProcessControl\\logging\\dll_log";
Logger *g_logger;
bool g_isReady;



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
    //MessageBox(NULL, pNodeName, "xxx", MB_OK);
    if (g_isReady == true && g_urlNum != NULL) {
        for (int i = 0; i < g_urlNum; ++i) {
            if (g_urls[i] != NULL) {
                if (strstr(g_urls[i], pNodeName) != NULL) {
                    SetLastError(WSAHOST_NOT_FOUND);
                    WSASetLastError(WSAHOST_NOT_FOUND);
                    return WSAHOST_NOT_FOUND;
                }
            }
        }
    }
    return mGetAddrInfo(pNodeName, pServiceName, pHints, ppResult);
}


int readRestrictedUrls()
{
    const size_t MAX_DIGIT_NUMBER = 5;
    static const char *sharedFileWithSizesName = "Global\\ProcessControlAppSizes";
    static const char *sharedFileWithUrlsName = "Global\\ProcessControlAppRestrictedURLS";

    // reading sizes
    my_shared_mem::MemMappedFile mmfileSizes;
    g_logger->info("opening shared file with size.");
    if (!mmfileSizes.openExisting(sharedFileWithSizesName, (MAX_DIGIT_NUMBER + 1) * 2, FILE_MAP_READ)) {
        g_logger->error("cannot open shared file for size of urls, error code : [%i]", GetLastError());
        return 1;
    }
    int urlsFileSize;
    if (!mmfileSizes.readDecimal(&urlsFileSize)) {
        g_logger->error("cannot read size from shared file, error code : [%i]", GetLastError());
        return 1;
    }
    int urlNum;
    if (!mmfileSizes.readDecimal(&urlNum)) {
        g_logger->error("cannot read number of urls from shared file, error code : [%i]", GetLastError());
        return 1;
    }

    g_logger->info("size = %i", urlsFileSize);
    g_logger->info("number of urls = %i", urlNum);
    
    mmfileSizes.close();

    // reading urls
    my_shared_mem::MemMappedFile mmfileUrls;
    g_logger->info("opening shared file with urls.");
    bool res = mmfileUrls.openExisting(sharedFileWithUrlsName, urlsFileSize, FILE_MAP_READ);

    if (!res) {
        g_logger->error("cannot open shared file with urls, error code : [%i]", GetLastError());
        return 1;
    }

    g_urls = new char*[urlNum];
    int curSize;
    for (int i = 0; i < urlNum; ++i) {
        if (!mmfileUrls.readDecimal(&curSize)) {
            g_logger->error("cannot read url length from shared file with urls, error code : [%i]", GetLastError());
            return 1;
        }
        g_urls[i] = new char[curSize + 1];
        if (mmfileUrls.readLine(g_urls[i]) == NULL) {
            g_logger->error("cannot read url from shared file with urls, error code : [%i]", GetLastError());
            return 1;
        }
        g_logger->info("Url read = '%s'", g_urls[i]);
    }

    mmfileUrls.close();

    g_urlNum = urlNum;
    g_isReady = true;

    return 0;
}


BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved)
{
    if (dwReason == DLL_PROCESS_ATTACH) {
        g_isReady = false;
        unsigned long pid = GetCurrentProcessId(); // hope process id will fit in integer
        char strpid[12];
        _ultoa_s(pid, strpid, 10);
        strcat_s(g_logfilename, strpid);
        strcat_s(g_logfilename, ".txt");
        g_logger = new Logger(g_logfilename, true);

        g_logger->info("Dll process attach.");

        readRestrictedUrls();

        g_logger->info("initializing hook");
        // Creating a HOOK
        if (MH_Initialize() != MH_OK) {
            g_logger->error("Cannot initialize minhook!");
            return 1;
        }
        if (MH_CreateHook(&getaddrinfo, &MyGetAddrInfo, reinterpret_cast<void**>(&mGetAddrInfo)) != MH_OK) {
            g_logger->info("Cannot create hook!");
            return 1;
        }
        if (MH_EnableHook(&getaddrinfo) != MH_OK) {
            g_logger->info("Cannot enable hook!");
            return 1;
        }

        //if (MH_CreateHook(&connect, &detourConnect, reinterpret_cast<void**>(&fpConnect)) != MH_OK)
        //    return 1;
        //if (MH_EnableHook(&connect) != MH_OK)
        //    return 1;

    }
    else if (dwReason == DLL_THREAD_ATTACH) {
        g_logger->info("Dll thread attach.");
    }
    else if (dwReason == DLL_PROCESS_DETACH) {
        g_logger->info("Dll detach.");

        if (MH_DisableHook(&getaddrinfo) != MH_OK) {
            g_logger->error("Cannot disable hook!");
            return 1;
        }

        if (MH_Uninitialize() != MH_OK) {
            g_logger->error("Cannot uninit minhook!");
            return 1;
        }

        //delete g_logger;
    }
    return TRUE;
}