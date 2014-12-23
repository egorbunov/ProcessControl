#include "ProcController.h"

// ----------- Constants --------------

const string ProcController::CHROME_HOOK_DLL_86 = "indlls\\browserHook86.dll";
const string ProcController::CHROME_HOOK_DLL_64 = "indlls\\browserHook64.dll"; // TODO: do I need this?

//////////////////////////////////////////////////////////////

ProcController::ProcController() : _modsFilename("mode.txt"), _logger("logging\\main_log.txt", true)
{
    loadMods();
    _dllInjector = DllInjector();
    _dllInjector.setLogger(&_logger);

    _controlledBrowsers.insert("chrome.exe");
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
        mbstowcs(wtext, _modsFilename.c_str(), _modsFilename.length() + 1);//Plus null
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
    while (hRes)
    {
        for (int i = 0; i < processes.size(); ++i)
        {
            wcstombs(chStr, pEntry.szExeFile, maxSize * sizeof(WCHAR));
            if (strcmp(chStr, processes[i].getName().c_str()) == 0)
            {
                hProcess = OpenProcess(PROCESS_TERMINATE, 0, (DWORD)pEntry.th32ProcessID);
                if (hProcess != NULL)
                {
                    TerminateProcess(hProcess, 9);
                    CloseHandle(hProcess);
                }
            }
        }
        hRes = Process32Next(hSnapShot,& pEntry);
    }


    CloseHandle(hSnapShot);
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

int ProcController::getModes(vector<Mode> &modes) const {
    modes = _allModes;
    return 0;
}

int ProcController::getMode(string name, Mode &mode) {
    for (int i = 0; i < _allModes.size(); ++i) {
        if (_allModes[i].getName() == name) {
            mode = _allModes[i];
            return 0;
        }
    }

    return 1; // error, no mode found
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
    _killControlledProcesses();
    _hookBrowsers();
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
    const size_t MAX_DIGIT_NUMBER = 5;
    char sharedSizeFileName[] = "Global\\ProcessControlAppSizes";

    const vector<string> &urls = _currentMode->getUrls();
    int size = 0;
    for (int i = 0; i < urls.size(); ++i) {
        size += urls[i].length() + 1 + MAX_DIGIT_NUMBER + 1;
    }

    // creating file with sizes 

    _logger.log("INFO: creating file with size of restricted urls");

    /*
    (MAX_DIGIT_NUMBER + 1) * (2) - number of symbols (bytes) in first shared file.
    First shared file will store:
    1) Number of bytes to store all urls and their lengths separated by '\n' symbol
    2) Number of urls
    One number in one line.
    I suppose that every line of the file can have max length = (MAX_DIGIT_NUMBER + 1)
    */
    if (!_mmfileSizes.create(sharedSizeFileName, (MAX_DIGIT_NUMBER + 1) * 2)) {
        _logger.log("ERROR: cannot create shared file for size of urls.");
        return 1;
    }
    char num[MAX_DIGIT_NUMBER];
    itoa(size, num, 10);
    _mmfileSizes.writeLine(num);
    itoa(urls.size(), num, 10);
    _mmfileSizes.writeLine(num);

    // creating file with urls

    char sharedUrlsFilename[] = "Global\\ProcessControlAppRestrictedURLS";
    if (!_mmfileUrls.create(sharedUrlsFilename, size)) {
        _logger.log("ERROR: cannot create shared file for urls.");
        return 1;
    }
    for (int i = 0; i < urls.size(); ++i) {
        itoa(urls[i].length(), num, 10);
        _mmfileUrls.writeLine(num);
        _mmfileUrls.writeLine(urls[i].c_str());
    }

   /* my_shared_mem::MemMappedFile test;
    bool ans = test.openExisting(sharedSizeFileName, (MAX_DIGIT_NUMBER + 1) * 2, FILE_MAP_READ);
    int sz, cnt;
    test.readInt(&sz);
    test.readInt(&cnt);

    test.close();
    ans = test.openExisting(sharedUrlsFilename, sz, FILE_MAP_READ);
    int x;
    for (int i = 0; i < cnt; ++i) {
        test.readInt(&x);
    }*/
    return 0;
    
}

void ProcController::_destroySessionSharedFiles() {
    _mmfileSizes.close();
    _mmfileUrls.close();
}

int ProcController::_hookBrowserProcess(unsigned long pId)
{
    _logger.log("INFO: Injecting dll into process : %lu", pId);
    if (_dllInjector.inject(pId, CHROME_HOOK_DLL_86)) {
        _logger.log("ERROR: Cannot inject dll.");
        return 1;
    }
    return 0;
}

void ProcController::_hookBrowsers()
{
    const int maxSize = 500; //max process name length
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
    PROCESSENTRY32 pEntry;
    HANDLE hProcess;

    pEntry.dwSize = sizeof(pEntry);
    BOOL hRes = Process32First(hSnapShot, &pEntry);
    char chStr[maxSize];

    DWORD pId;
    set<DWORD> openedBrowsersIds;
    while (hRes)
    {
        for (set<string>::iterator it = _controlledBrowsers.begin(); it != _controlledBrowsers.end(); ++it)
        {
            wcstombs(chStr, pEntry.szExeFile, maxSize * sizeof(WCHAR));
            if (strcmp(chStr, (*it).c_str()) == 0)
                openedBrowsersIds.insert((DWORD)pEntry.th32ProcessID);
        }
        hRes = Process32Next(hSnapShot, &pEntry);
    }
    CloseHandle(hSnapShot);

    vector<DWORD> closedBrowsersIds;
    vector<DWORD> browsersForHookIds;

    // getting browsers process ids, which already closed
    set_difference(_hookedPids.begin(), _hookedPids.end(), openedBrowsersIds.begin(), openedBrowsersIds.end(), 
        inserter(closedBrowsersIds, closedBrowsersIds.end()));

    // browsers ids, which we need to hook
    set_difference(openedBrowsersIds.begin(), openedBrowsersIds.end(), _hookedPids.begin(), _hookedPids.end(),
        inserter(browsersForHookIds, browsersForHookIds.end()));

    for (vector<DWORD>::iterator it = closedBrowsersIds.begin(); it != closedBrowsersIds.end(); ++it)
    {
        _hookedPids.erase(*it);
    }

    for (vector<DWORD>::iterator it = browsersForHookIds.begin(); it != browsersForHookIds.end(); ++it)
    {
        _hookedPids.insert(*it);
        _hookBrowserProcess(*it);
    } 
}

void ProcController::_hookTaskmanagers() {

}

void ProcController::init() {
    this->_createSessionSharedFiles();
}

void ProcController::uninit() {
    this->_destroySessionSharedFiles();
}