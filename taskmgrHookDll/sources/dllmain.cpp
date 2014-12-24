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
Logger *g_logger;
FARPROC g_fpQuerySysInfo;

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

    if (SystemInformationClass == SystemProcessInformation) {
        PSYSTEM_PROCESSES infoP = (PSYSTEM_PROCESSES)SystemInformation;
        while (infoP) {
            //g_logger->log("Process ID: %d\n", infoP->ProcessId);
            //g_logger->log("Inherited From ID: %d\n", infoP->InheritedFromProcessId);
            //g_logger->log("Pagefile Usage: %d\n", infoP->VmCounters.PagefileUsage);
            //g_logger->log("Working SetSize: %d\n ", infoP->VmCounters.WorkingSetSize);
            //g_logger->log("PageFault Count: %d\n", infoP->VmCounters.PageFaultCount);
            //g_logger->log("QuadPart (User): %d\n", ((double)infoP->UserTime.QuadPart) / 1e7);
            //g_logger->log("QuadPart (Kernal): %d\n", ((double)infoP->KernelTime.QuadPart) / 1e7);

            char pName[256];
            memset(pName, 0, sizeof(pName));
            WideCharToMultiByte(CP_ACP, 0, infoP->ProcessName.Buffer, infoP->ProcessName.Length, pName, sizeof(pName), NULL, NULL);
            if (strcmp(pName, "taskmgrHooker64.exe") == 0) {
                MessageBox(NULL, pName, "dsadsa", MB_OK);
            }
            if (!infoP->NextEntryDelta) break;
            infoP = (PSYSTEM_PROCESSES)(((LPBYTE)infoP) + infoP->NextEntryDelta);
        }
    }

    return res;
}


BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved)
{
    //MessageBox(NULL, "TEXT", "TEXT", MB_OK);
    if (dwReason == DLL_PROCESS_ATTACH) {
        g_fpQuerySysInfo = GetProcAddress(GetModuleHandle("ntdll.dll"), "NtQuerySystemInformation");

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
        if (MH_CreateHook(g_fpQuerySysInfo,
            &MyNtQuerySystemInformation, 
            reinterpret_cast<void**>(&realNtQuerySystemInfo)) != MH_OK) {

            g_logger->log("INFO: Cannot create hook!");
            return 1;
        }
        if (MH_EnableHook(g_fpQuerySysInfo) != MH_OK) {
            g_logger->log("INFO: Cannot enable hook!");
            return 1;
        }

    }
    else if (dwReason == DLL_THREAD_ATTACH) {
        g_logger->log("INFO: Dll thread attach.");
    }
    else if (dwReason == DLL_PROCESS_DETACH) {
        g_logger->log("INFO: Dll detach.");

        if (MH_DisableHook(g_fpQuerySysInfo) != MH_OK) {
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