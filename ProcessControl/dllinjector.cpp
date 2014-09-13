#include "dllinjector.h"


DllInjector::DllInjector()
{
}


DllInjector::~DllInjector()
{
}

LPTHREAD_START_ROUTINE DllInjector::AllocWritePath(HANDLE hTargetProcHandle, char* dllPath, LPVOID *lpExecParam)
{
    unsigned int writeLen = 0;
    LPVOID lpDllAddr = NULL;
    LPVOID lpWriteVal = NULL;
    LPVOID loadLibAddr = NULL;

    lpDllAddr = VirtualAllocEx(hTargetProcHandle, NULL, strlen(dllPath), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (WriteProcessMemory(hTargetProcHandle, lpDllAddr, dllPath, strlen(dllPath), NULL) == 0) {
        fprintf(stderr, "\n[!] WriteProcessMemory Failed [%u]\n", GetLastError());
        return NULL;
    }

    *lpExecParam = (LPVOID *)lpDllAddr;
    loadLibAddr = GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "LoadLibraryA");
    if (loadLibAddr == NULL) {
        fprintf(stderr, "\n[!] Failed to find LoadLibrary in Kernel32! Quiting...\n");
        return NULL;
    }

    return (LPTHREAD_START_ROUTINE)loadLibAddr;
}

int DllInjector::injectDLL(HANDLE hTargetProcHandle, LPTHREAD_START_ROUTINE lpStartExecAddr, LPVOID lpExecParam)
{
    HANDLE rThread = NULL;

    rThread = CreateRemoteThread(hTargetProcHandle, NULL, 0, lpStartExecAddr, lpExecParam, 0, NULL);
    if (rThread == NULL) {
        fprintf(stderr, "\n[!] CreateRemoteThread Failed! [%d] Exiting....\n", GetLastError());
        return -1;
    }

    return 0;
}

HANDLE DllInjector::attachToProcess(DWORD procID)
{
    OSVERSIONINFO osver;

    SetDebugPrivileges();

    osver.dwOSVersionInfoSize = sizeof(osver);
    if (GetVersionEx(&osver)) {
        if (osver.dwMajorVersion == 5) {
            //printf("\t[+] Detected Windows XP\n");
            return OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_CREATE_THREAD, 0, procID);
        }
        if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 0) {
            //printf("\t[+] Detected Windows Vista\n");
            return NULL;
        }
        if (osver.dwMajorVersion == 6 && osver.dwMinorVersion == 1)	{
            return OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, procID);
        }
    }
    else {
        fprintf(stderr, "\n[!] Could not detect OS version\n");
    }
    return NULL;
}

int DllInjector::SetDebugPrivileges(void)
{
    TOKEN_PRIVILEGES priv = { 0 };
    HANDLE hToken = NULL;

    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        priv.PrivilegeCount = 1;
        priv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

        if (LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &priv.Privileges[0].Luid)) {
            if (AdjustTokenPrivileges(hToken, FALSE, &priv, 0, NULL, NULL) == 0) {
                fprintf(stderr, "AdjustTokenPrivilege Error! [%u]\n", GetLastError());
            }
        }
        CloseHandle(hToken);
    }
    return GetLastError();
}

int DllInjector::inject(unsigned long processId, std::string dllName)
{
    LPTHREAD_START_ROUTINE lpStartExecAddr = NULL;
    LPVOID lpExecParam = NULL;
    HANDLE hTargetProcHandle = NULL;

    char* lpcDll = NULL;
    char tcDllPath[_bufferSize] = "";

    GetFullPathNameA(dllName.c_str(), _bufferSize, tcDllPath, NULL);
    // Attach to process with OpenProcess()
    hTargetProcHandle = attachToProcess(processId);
    if (hTargetProcHandle == NULL) {
        fprintf(stderr, "\n[!] ERROR: Could not Attach to Process!!\n");
        return -1;
    }

    // Copy the DLL via write path method
    lpStartExecAddr = AllocWritePath(hTargetProcHandle, tcDllPath, &lpExecParam);

    if (lpStartExecAddr == NULL) {
        fprintf(stderr, "\n[!] ERROR: Could not allocate memory!!\n");
        return -1;
    }

    // Inject the DLL into process via create remote thread method
    injectDLL(hTargetProcHandle, lpStartExecAddr, lpExecParam);
    CloseHandle(hTargetProcHandle);

    return 0;
}