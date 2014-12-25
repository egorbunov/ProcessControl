#include "ProcController.h"

// ----------- Constants --------------

const char ProcController::MODES_FILENAME[] = "..\\data\\modes.txt";

const string ProcController::CHROME_HOOK_DLL_86 = "..\\indlls\\chromeHookDll86.dll";
const string ProcController::CHROME_HOOK_DLL = CHROME_HOOK_DLL_86;

const LPCWSTR ProcController::TASKMGR_HOOKER_PROCESS_86 = L"..\\bin\\taskmgrHooker86.exe";
const LPCWSTR ProcController::TASKMGR_HOOKER_PROCESS_64 = L"..\\bin\\taskmgrHooker64.exe";


const string ProcController::CONTROLLED_BROWSERS[] = { "chrome.exe" };

//////////////////////////////////////////////////////////////

ProcController::ProcController() : 
_modsFilename(MODES_FILENAME),
_logger(common_consts::MAIN_LOG_FILENAME, true, true, common_consts::IS_NO_LOGGING)
{
    loadMods();
    _dllInjector = DllInjector();
    _dllInjector.setLogger(&_logger);
    _taskmgrHookerProcess = NULL;

    _is64bitWindows = (bool)IsWow64Process(GetCurrentProcess(), &_is64bitWindows);

    _controlledBrowsers.insert(std::begin(CONTROLLED_BROWSERS), std::end(CONTROLLED_BROWSERS));
}

ProcController::~ProcController()
{
}

int ProcController::loadMods()
{
    std::ifstream in;
    in.open(_modsFilename.c_str());
    if (!in.good()) {
        in.close();
        wchar_t *wtext = (wchar_t *) malloc((_modsFilename.length() + 1) * sizeof(wchar_t));
        mbstowcs(wtext, _modsFilename.c_str(), _modsFilename.length() + 1);
        HANDLE hFile = CreateFile(wtext,
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            CREATE_NEW,
            FILE_ATTRIBUTE_NORMAL,
            NULL);
        CloseHandle(hFile);

        std::ofstream out;
        out.open(_modsFilename.c_str());
        _ASSERT(out.good());
        out << 0 << std::endl;
        out.close();

        
        in.open(_modsFilename.c_str(), std::ifstream::in);
    }

    int count = 0;
    in >> count;
    in.get(); // reading line break

    string str;
    std::istringstream ss;
    for (int i = 0; i < count; ++i)
    {
        stringstream modeStream;
        for (int i = 0; i < Mode::STR_MODE_LINE_COUNT; ++i) {
            if (in.eof()) {
                return ERROR_MODE_FILE_CORRUPTED;
            }
            std::getline(in, str);
            modeStream << str << std::endl;
        }
        _allModes.push_back(Mode(modeStream));
    }

    return 0;
}

void ProcController::_killProcessByName(string& processName)
{
    const int maxSize = 260; //max process name length
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
    PROCESSENTRY32 pEntry;
    pEntry.dwSize = sizeof(pEntry);
    BOOL hRes = Process32First(hSnapShot,& pEntry);
    char chStr[maxSize];
    while (hRes)
    {
        wcstombs(chStr, pEntry.szExeFile, maxSize * sizeof(WCHAR));
        if (strcmp(chStr, processName.c_str()) == 0)
        {
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0,
                (DWORD)pEntry.th32ProcessID);
            if (hProcess != NULL)
            {
                TerminateProcess(hProcess, 9);
                CloseHandle(hProcess);
            }
        }
        hRes = Process32Next(hSnapShot,& pEntry);
    }
    CloseHandle(hSnapShot);
}

void ProcController::_killControlledProcesses()
{
    const int maxSize = 500; //max process name length
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
    PROCESSENTRY32 pEntry;
    HANDLE hProcess;

    pEntry.dwSize = sizeof(pEntry);
    BOOL hRes = Process32First(hSnapShot,& pEntry);
    char chStr[maxSize];

    const vector<ProcessDesc> &processes = _currentMode->getProcesses();
    const vector<string> &urls = _currentMode->getUrls();
    while (hRes) {
        for (int i = 0; i < processes.size(); ++i) {
            wcstombs(chStr, pEntry.szExeFile, maxSize * sizeof(WCHAR));

            if (strcmp(chStr, processes[i].getName().c_str()) == 0) {
                hProcess = OpenProcess(PROCESS_TERMINATE, 0, (DWORD)pEntry.th32ProcessID);
                if (hProcess != NULL) {
                    TerminateProcess(hProcess, 9);
                    CloseHandle(hProcess);
                }
            }
        }
        hRes = Process32Next(hSnapShot,& pEntry);
    }


    CloseHandle(hSnapShot);
}

int ProcController::_killProcessIfRestricted(const PROCESSENTRY32 &pEntry, const char* name) {
    const vector<ProcessDesc> &processes = _currentMode->getProcesses();
    for (int i = 0; i < processes.size(); ++i) {
        if (strcmp(name, processes[i].getName().c_str()) == 0) {
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0, (DWORD)pEntry.th32ProcessID);
            if (hProcess != NULL) {
                TerminateProcess(hProcess, 9);
                CloseHandle(hProcess);
            }
            return 0;
        }
    }
    return 1;
}

int ProcController::_rewriteModeFile()
{
    std::ofstream out;
    out.open(_modsFilename.c_str(), std::ofstream::out);

    _ASSERT(out.good());

    out << _allModes.size() << std::endl;

    for (std::vector<Mode>::iterator& mode = _allModes.begin(); mode != _allModes.end(); ++mode) {
        out << *mode;
    }

    out.close();

    return 0;
}

const vector<Mode> ProcController::getModes() const {
    return _allModes;
}

const Mode& ProcController::getMode(string name) {
    for (int i = 0; i < _allModes.size(); ++i) {
        if (_allModes[i].getName() == name) {
            return _allModes[i];
        }
    }

    throw ModeNotFoundException();
}

int ProcController::getModeProgress(string name) const
{
    for (int i = 0; i < _allModes.size(); ++i) {
        if (_allModes[i].getName() == name)
            return _allModes[i].getProgress();
    }
    return -2;
}

int ProcController::editMode(string oldName, Mode mode)
{
    if (mode.getProcessCount() == 0 && mode.getUrlCount() == 0)
        return ERROR_EMPTY_LISTS;

    if (mode.getProgress() > 100)
        return ERROR_BAD_PROGRESS;

    if (mode.getName().length() == 0)
        return ERROR_BAD_NAME;

    int curMode = -1;
    int count = 0;
    for (int i = 0; i < _allModes.size(); ++i)
    {
        if (_allModes[i].getName() == oldName)
            curMode = i;
        if (_allModes[i].getName() == mode.getName() && oldName != mode.getName())
            return ERROR_MODE_ALREADY_EXISTS;
    }
    if (curMode == -1)
        return ERROR_NO_MODE_TO_EDIT;

    _allModes[curMode] = mode;

    this->_rewriteModeFile();

    return 0;
}

int ProcController::deleteMode(string name)
{
    for (vector<Mode>::iterator mode = _allModes.begin(); mode != _allModes.end(); ++mode)
    {
        if (mode->getName() == name)
        {
            _allModes.erase(mode);
            this->_rewriteModeFile();
            return 0;
        }
    }

    return 1;
}

int ProcController::addNewMode(Mode newMode)
{
    for (vector<Mode>::iterator mode = _allModes.begin(); mode != _allModes.end(); ++mode)
    {
        if (mode->getName() == newMode.getName())
            return ERROR_MODE_ALREADY_EXISTS;
    }

    if (newMode.getProcessCount() == 0 && newMode.getUrlCount() == 0)
        return ERROR_EMPTY_LISTS;

    if (newMode.getProgress() > 100)
        return ERROR_BAD_PROGRESS;

    if (newMode.getName().length() == 0)
        return ERROR_BAD_NAME;

    _allModes.push_back(newMode);

    _rewriteModeFile();

    return 0;
}

int ProcController::setSessionMode(string& name)
{
    std::stringstream ss;
    std::string item;

    for (vector<Mode>::iterator mode = _allModes.begin(); mode != _allModes.end(); ++mode)
    {
        if (mode->getName() == name)
        {
            _currentMode = &(*mode);
        }
    }
    return 1;
}

int ProcController::control()
{
    const int maxSize = 500; //max process name length
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
    PROCESSENTRY32 pEntry;
    HANDLE hProcess;

    pEntry.dwSize = sizeof(pEntry);
    BOOL hRes = Process32First(hSnapShot, &pEntry);
    char chStr[maxSize];

    const vector<ProcessDesc> &processes = _currentMode->getProcesses();
    const vector<string> &urls = _currentMode->getUrls();

    set<DWORD> openedBrowsersIds;

    while (hRes) {
        wcstombs(chStr, pEntry.szExeFile, maxSize * sizeof(WCHAR));

        // killing if process restricted, so if killed (return value == 0), 
        // no need for hooking
        if (_killProcessIfRestricted(pEntry, chStr)) {
            // hooking, if that is supported browser
            if (_controlledBrowsers.find(chStr) != _controlledBrowsers.end()) {
                openedBrowsersIds.insert((DWORD)pEntry.th32ProcessID);
            } 
        }

        hRes = Process32Next(hSnapShot, &pEntry);
    }
    CloseHandle(hSnapShot);

    vector<DWORD> closedIds;
    vector<DWORD> browsersForHookIds;

    // getting browsers process ids, which already closed
    set_difference(_hookedPids.begin(), _hookedPids.end(), openedBrowsersIds.begin(), openedBrowsersIds.end(),
                   inserter(closedIds, closedIds.end()));

    // browsers ids, which we need to hook
    set_difference(openedBrowsersIds.begin(), openedBrowsersIds.end(), _hookedPids.begin(), _hookedPids.end(),
                   inserter(browsersForHookIds, browsersForHookIds.end()));

    for (vector<DWORD>::iterator it = closedIds.begin(); it != closedIds.end(); ++it)
        _hookedPids.erase(*it);

    for (vector<DWORD>::iterator it = browsersForHookIds.begin(); it != browsersForHookIds.end(); ++it) {
        _hookBrowserProcess(*it);
        _hookedPids.insert(*it);
    }

    return 0;
}

string ProcController::getActiveModeName()
{
    return _currentMode->getName();
}

int ProcController::editProgress(string name, int newProgress)
{
    if (newProgress < 0 || newProgress > 100)
        return 1;

    for (vector<Mode>::iterator mode = _allModes.begin(); mode != _allModes.end(); ++mode)
    {
        if (mode->getName() == name)
        {
            mode->setProgress(newProgress);
            this->_rewriteModeFile();
            return 0;
        }
    }
    return 1;
}

int ProcController::_createSessionSharedFiles() {
    const vector<string> &urls = _currentMode->getUrls();
    int size = 0;
    for (int i = 0; i < urls.size(); ++i) {
        size += urls[i].length() + 1 + common_consts::UI_MAX_DIGIT_NUMBER + 1;
    }

    // creating file with sizes 

    _logger.info("creating file with size of restricted urls");
    /*
    (MAX_DIGIT_NUMBER + 1) * (2) - number of symbols (bytes) in first shared file.
    First shared file will store:
    1) Number of bytes to store all urls and their lengths separated by '\n' symbol
    2) Number of urls
    One number in one line.
    I suppose that every line of the file can have max length = (MAX_DIGIT_NUMBER + 1)
    */
    if (!_mmfileSizes.create(browser_hook_consts::SHARED_FILE_WITH_SIZES, 
        (common_consts::UI_MAX_DIGIT_NUMBER + 1) * 2)) {
        _logger.error("cannot create shared file for size of urls.");

        return 1;
    }
    char num[common_consts::UI_MAX_DIGIT_NUMBER];
    itoa(size, num, 10);
    _mmfileSizes.writeLine(num);
    itoa(urls.size(), num, 10);
    _mmfileSizes.writeLine(num);

    // creating file with urls

    if (!_mmfileUrls.create(browser_hook_consts::SHARED_FILE_WITH_URLS, size)) {
        _logger.error("cannot create shared file for urls.");

        return 1;
    }
    for (int i = 0; i < urls.size(); ++i) {
        itoa(urls[i].length(), num, common_consts::DECIMAL_NOTATION);
        _mmfileUrls.writeLine(num);
        _mmfileUrls.writeLine(urls[i].c_str());
    }

    // creating file which will store absolute path for log files
    if (!_mmfileLoggingPath.create(common_consts::SHARED_FILE_WITH_LOG_PATH, common_consts::MAX_PATH_LENGTH)) {
        _logger.error("cannot create shared file for logging path!");

        return 1;
    }
    char buffer[common_consts::MAX_PATH_LENGTH];
    // getting logging path
    GetFullPathNameA(common_consts::REL_LOGGING_PATH, common_consts::MAX_PATH_LENGTH, buffer, NULL);
    _mmfileLoggingPath.writeLine(buffer);

    return 0;
}

void ProcController::_destroySessionSharedFiles() {
    _mmfileSizes.close();
    _mmfileUrls.close();
    _mmfileLoggingPath.close();
}

int ProcController::_hookBrowserProcess(unsigned long pId)
{
    _logger.info("Injecting dll %s into browser process : %lu", CHROME_HOOK_DLL.c_str(), pId);
    if (_dllInjector.inject(pId, CHROME_HOOK_DLL)) {
        _logger.error("Cannot inject dll.");
        return 1;
    }
    return 0;
}

int ProcController::_startTaskmgrHooker() {
    _logger.info("Starting taskmgr.exe hooker.");
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    
    LPCWSTR taskmgrProcessName;
    if (_is64bitWindows) {
        taskmgrProcessName = TASKMGR_HOOKER_PROCESS_64;
    }
    else {
        taskmgrProcessName = TASKMGR_HOOKER_PROCESS_86;
    }

    // creating command line to pass to taskmgr hooker process
    // taskmgr hooker proc. need to know PID of ProcessControll process,
    // session last time and control stab value
    WCHAR args[common_consts::MAX_PATH_LENGTH + 3 * (1 + common_consts::UL_MAX_DIGIT_NUMBER)];

    WCHAR strpid[common_consts::UL_MAX_DIGIT_NUMBER];
    DWORD pid = GetCurrentProcessId();
    _ultow_s(pid, strpid, common_consts::DECIMAL_NOTATION);

    WCHAR strWait[common_consts::UL_MAX_DIGIT_NUMBER];
    _itow_s(CONTROL_STAB, strWait, common_consts::DECIMAL_NOTATION);

    WCHAR strSessionTime[common_consts::UL_MAX_DIGIT_NUMBER];
    _ultow_s(_sessionTime, strSessionTime, common_consts::DECIMAL_NOTATION);

    wcscat_s(args, taskmgrProcessName);
    wcscat_s(args, L" ");
    wcscat_s(args, strSessionTime);
    wcscat_s(args, L" ");
    wcscat_s(args, strWait);
    wcscat_s(args, L" ");
    wcscat_s(args, strpid);

    // Start the child process. 
    if (!CreateProcess(taskmgrProcessName,   // No module name (use command line)
        args,          // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi)            // Pointer to PROCESS_INFORMATION structure
        )
    {
        _logger.error("cannot start taskmgr hooker! error code: %i", GetLastError());
        return 1;
    }

    _taskmgrHookerProcess = pi.hProcess;
    return 0;
}

void ProcController::start(unsigned long sessionTime) {
    _sessionTime = sessionTime;
    _createSessionSharedFiles();

    // running taskmanager hooking process
    if (_startTaskmgrHooker()) {
        _logger.error("cannot run taskmgr hooker!");
    }
}

void ProcController::stop() {
    this->_destroySessionSharedFiles();
    TerminateProcess(_taskmgrHookerProcess, 0);
}