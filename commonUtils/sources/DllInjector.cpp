#include "../headers/DllInjector.h"


DllInjector::DllInjector()
{
    _logger = NULL;
}


DllInjector::~DllInjector()
{
}

void DllInjector::setLogger(Logger *logger) {
    _logger = logger;
}

LPTHREAD_START_ROUTINE DllInjector::AllocWritePath(HANDLE hTargetProcHandle, char* dllPath, LPVOID *lpExecParam)
{
    unsigned int writeLen = 0;
    LPVOID lpDllAddr = NULL;
    LPVOID lpWriteVal = NULL;
    LPVOID loadLibAddr = NULL;

    lpDllAddr = VirtualAllocEx(hTargetProcHandle, NULL, strlen(dllPath), MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (lpDllAddr == NULL) {
        if (_logger) _logger->error("VirtualAllocEx failed! [%u]", GetLastError());
        return NULL;
    }
    if (WriteProcessMemory(hTargetProcHandle, lpDllAddr, dllPath, strlen(dllPath), NULL) == 0) {
        if (_logger) _logger->error("WriteProcessMemory Failed[%u]", GetLastError());
        return NULL;
    }

    *lpExecParam = (LPVOID *)lpDllAddr;
    loadLibAddr = GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "LoadLibraryA");
    if (loadLibAddr == NULL) {
        if (_logger) _logger->error("Failed to find LoadLibrary in Kernel32!");
        return NULL;
    }

    return (LPTHREAD_START_ROUTINE)loadLibAddr;
}

int DllInjector::injectDLL(HANDLE hTargetProcHandle, LPTHREAD_START_ROUTINE lpStartExecAddr, LPVOID lpExecParam)
{
    HANDLE rThread = NULL;

    rThread = CreateRemoteThread(hTargetProcHandle, NULL, 0, lpStartExecAddr, lpExecParam, 0, NULL);
    if (rThread == NULL) {
        if (_logger) _logger->error("CreateRemoteThread Failed! [%d]", GetLastError());
        return -1;
    }
    if (_logger) _logger->info("Remote thread created!");
    return 0;
}

HANDLE DllInjector::attachToProcess(DWORD procID)
{
    SetDebugPrivileges();

    if (IsWindows7OrGreater()) {
        return OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, procID);
    }
    else {
        if (_logger) _logger->error("get version ex Failed (probably bad win version!)! [%d]", GetLastError());
    }
   /* if (osver.dwMajorVersion == 5) {
        return OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_CREATE_THREAD, 0, procID);
    }*/

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
                if (_logger) _logger->error("AdjustTokenPrivilege Error! [%u]", GetLastError());
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

    if (GetFullPathNameA(dllName.c_str(), _bufferSize, tcDllPath, NULL) == 0) {
        if (_logger) _logger->error("Cannot get full dll path!");
        return -1;
    };
    // Attach to process with OpenProcess()
    hTargetProcHandle = attachToProcess(processId);
    if (hTargetProcHandle == NULL) {
        if (_logger) _logger->error("Could not Attach to Process!!");
        return -1;
    }

    // Copy the DLL via write path method
    lpStartExecAddr = AllocWritePath(hTargetProcHandle, tcDllPath, &lpExecParam);

    if (lpStartExecAddr == NULL) {
        if (_logger) _logger->error("Could not allocate memory!!");
        return -1;
    }

    // Inject the DLL into process via create remote thread method
    if (_logger) _logger->info("INJECTING!");
    injectDLL(hTargetProcHandle, lpStartExecAddr, lpExecParam);
    CloseHandle(hTargetProcHandle);

    return 0;
}