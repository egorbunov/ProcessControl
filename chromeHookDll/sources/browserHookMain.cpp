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

// ----------------------------------- GLOBALS -----------------------------------------------
char **g_urls;
int g_urlNum;



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
    if (g_urlNum != 0) {
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


int readRestrictedUrls(Logger &logger)
{
    // reading sizes
    my_shared_mem::MemMappedFile mmfileSizes;
    logger.info("opening shared file with size.");
    if (!mmfileSizes.openExisting(browser_hook_consts::SHARED_FILE_WITH_SIZES, 
        (common_consts::UI_MAX_DIGIT_NUMBER + 1) * 2, FILE_MAP_READ)) {
        logger.error("cannot open shared file for size of urls, error code : [%i]", GetLastError());
        return 1;
    }
    int urlsFileSize;
    if (!mmfileSizes.readDecimal(&urlsFileSize)) {
        logger.error("cannot read size from shared file, error code : [%i]", GetLastError());
        return 1;
    }
    int urlNum;
    if (!mmfileSizes.readDecimal(&urlNum)) {
        logger.error("cannot read number of urls from shared file, error code : [%i]", GetLastError());
        return 1;
    }

    logger.info("size = %i", urlsFileSize);
    logger.info("number of urls = %i", urlNum);
    
    mmfileSizes.close();

    // reading urls
    my_shared_mem::MemMappedFile mmfileUrls;
    logger.info("opening shared file with urls.");
    bool res = mmfileUrls.openExisting(browser_hook_consts::SHARED_FILE_WITH_URLS, urlsFileSize, FILE_MAP_READ);

    if (!res) {
        logger.error("cannot open shared file with urls, error code : [%i]", GetLastError());
        return 1;
    }

    g_urls = new char*[urlNum];
    int curSize;
    for (int i = 0; i < urlNum; ++i) {
        if (!mmfileUrls.readDecimal(&curSize)) {
            logger.error("cannot read url length from shared file with urls, error code : [%i]", GetLastError());
            return 1;
        }
        g_urls[i] = new char[curSize + 1];
        if (mmfileUrls.readLine(g_urls[i]) == NULL) {
            logger.error("cannot read url from shared file with urls, error code : [%i]", GetLastError());
            return 1;
        }
        logger.info("Url read = '%s'", g_urls[i]);
    }

    mmfileUrls.close();

    g_urlNum = urlNum;
    return 0;
}


int getLoggingFilename(char *buffer, int bufSize) {
    my_shared_mem::MemMappedFile file;
    if (!file.openExisting(common_consts::SHARED_FILE_WITH_LOG_PATH, 
        common_consts::MAX_PATH_LENGTH, FILE_MAP_READ)) {

        return 1;
    }

    if (buffer == NULL || !file.readLine(buffer))
        return 1;

    strcat_s(buffer, bufSize, browser_hook_consts::LOG_FILE_SUFFIX);

    DWORD pid = GetCurrentProcessId();
    char strPid[common_consts::UL_MAX_DIGIT_NUMBER];
    _ultoa_s(pid, strPid, common_consts::DECIMAL_NOTATION);
    strcat_s(buffer, bufSize, strPid);
    strcat_s(buffer, bufSize, ".txt");
}

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved)
{
    if (dwReason == DLL_PROCESS_ATTACH) {
        char logFilename[common_consts::MAX_PATH_LENGTH];
        getLoggingFilename(logFilename, common_consts::MAX_PATH_LENGTH);
        Logger logger(logFilename, true, true, common_consts::IS_NO_LOGGING);

        g_urlNum = 0;

        logger.info("Dll process attach.");

        readRestrictedUrls(logger);

        logger.info("initializing hook");
        // Creating a HOOK
        if (MH_Initialize() != MH_OK) {
            logger.error("Cannot initialize minhook!");
            return 1;
        }
        if (MH_CreateHook(&getaddrinfo, &MyGetAddrInfo, reinterpret_cast<void**>(&mGetAddrInfo)) != MH_OK) {
            logger.info("Cannot create hook!");
            return 1;
        }
        if (MH_EnableHook(&getaddrinfo) != MH_OK) {
            logger.info("Cannot enable hook!");
            return 1;
        }
    }
    else if (dwReason == DLL_THREAD_ATTACH) {
    }
    else if (dwReason == DLL_PROCESS_DETACH) {
        char logFilename[common_consts::MAX_PATH_LENGTH];
        getLoggingFilename(logFilename, common_consts::MAX_PATH_LENGTH);
        Logger logger(logFilename, false, true, common_consts::IS_NO_LOGGING);

        logger.info("Dll thread deatach.");
        if (MH_DisableHook(&getaddrinfo) != MH_OK) {
            logger.error("Cannot disable hook!");
            return 1;
        }
        if (MH_Uninitialize() != MH_OK) {
            logger.error("Cannot uninit minhook!");
            return 1;
        }
    }
    return TRUE;
}