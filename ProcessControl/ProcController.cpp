#include "ProcController.h"

int ProcController::_controllersCount = 0;
const string ProcController::_dll86name = "indlls\\browserHook86.dll";

// Is it useful??????????????????????????????????????????????
const string ProcController::_dll64name = "indlls\\browserHook64.dll";
//////////////////////////////////////////////////////////////

ProcController::ProcController() : _modsFilename("mode.txt"), _logger("logging\\main_log.txt", true)
{
    _controllersCount += 1;
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

    string s, f;
    std::istringstream ss;
    Mode newMode;
    for (int i = 0; i < count; ++i)
    {
        newMode.processes.clear();
        newMode.urls.clear();
        //reading name and description
        std::getline(in, newMode.name);
        std::getline(in, newMode.description);

        // reading processes
        std::getline(in, s);
        ss.str(s);
        while (!ss.eof())
        {
            std::getline(ss, f, '|');
            if (f != "")
                newMode.processes.push_back(f);
        }
        ss.clear();

        // reading urls
        std::getline(in, s);
        ss.str(s);
        //ss.seekg(0, std::ios::beg);
        while (!ss.eof())
        {
            std::getline(ss, f, '|');
            if (f != "")
                newMode.urls.push_back(f);
        }
        ss.clear();
        //reading progress
        in >> newMode.progress;
        in.get(); // line break read

        _allModes.push_back(newMode);
    }

    return 0;
}

/// <summary>
/// Kills process by process name
/// </summary>
/// <param name="processName">process name.</param>
void ProcController::killProcessByName(string& processName)
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

/// <summary>
/// Kills all controlled by current mode processes
/// </summary>
void ProcController::killControlledProcesses()
{
    const int maxSize = 500; //max process name length
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
    PROCESSENTRY32 pEntry;
    HANDLE hProcess;

    pEntry.dwSize = sizeof(pEntry);
    BOOL hRes = Process32First(hSnapShot,& pEntry);
    char chStr[maxSize];

    while (hRes)
    {
        for (vector<string>::iterator it = _curProcToControl.begin(); it != _curProcToControl.end(); ++it)
        {
            wcstombs(chStr, pEntry.szExeFile, maxSize * sizeof(WCHAR));
            if (strcmp(chStr, (*it).c_str()) == 0)
            {

                hProcess = OpenProcess(PROCESS_TERMINATE, 0,
                    (DWORD)pEntry.th32ProcessID);
                if (hProcess != NULL)
                {
                    TerminateProcess(hProcess, 9);
                    CloseHandle(hProcess);
                    //MessageBox(NULL, L"That program is not allowed in current mode!", L"Process control warning", MB_ICONWARNING);
                }
            }
        }
        hRes = Process32Next(hSnapShot,& pEntry);
    }


    CloseHandle(hSnapShot);
}

/// <summary>
/// Rewrites the mode file. Fills file with name _modsFilename with information 
/// about modes: 
/// 1) In the first line: %number of modes%
/// 2) Next 5 lines describes first mode and so on:
///                     %name%
///                     %descriptions%
///                     %programs separated with "|"%
///                     %urls separated with "|"%
///                     %progress (number)%
/// </summary>
/// <returns> 0 if ok </returns>
int ProcController::rewriteModeFile()
{
    std::ofstream out;
    out.open(_modsFilename.c_str(), std::ofstream::out);

    _ASSERT(out.good());

    out << _allModes.size() << std::endl;

    for (std::vector<Mode>::iterator& mode = _allModes.begin(); mode != _allModes.end(); ++mode)
    {
        out << mode->name << std::endl;
        out << mode->description << std::endl;
        for (std::vector<string>::iterator& process = mode->processes.begin();
            process != mode->processes.end(); 
            ++process)
        {
            out << *process << "|";
        }
        out << std::endl;
        for (std::vector<string>::iterator& url = mode->urls.begin(); url != mode->urls.end(); ++url)
        {
            out << *url << "|";
        }
        out << std::endl;
        out << mode->progress << std::endl;
    }

    out.close();

    return 0;
}

int ProcController::getModsNames(vector<string>&  names) const
{
    names.clear();
    names.resize(_allModes.size());

    for (int i = 0; i < _allModes.size(); ++i)
        names[i] = _allModes[i].name;

    return 0;
}

int ProcController::getModsDscriprions(vector<string>& descriptions) const
{
    descriptions.clear();
    descriptions.resize(_allModes.size());

    for (int i = 0; i < _allModes.size(); ++i)
        descriptions[i] = _allModes[i].description;
    return 0;
}

int ProcController::getModsProgress(vector<short>& progress) const
{
    progress.clear();
    progress.resize(_allModes.size());

    for (int i = 0; i < _allModes.size(); ++i)
        progress[i] = _allModes[i].progress;
    return 0;
}

int ProcController::getModeInfo(string& name, string& description, 
    vector<string>& procToControl, vector<string>& urlToControl, 
    short& progress) const
{
    int curMode = 0;
    for (curMode = 0; curMode < _allModes.size(); ++curMode)
    {
        if (_allModes[curMode].name == name)
            break;
    }

    if (curMode >= _allModes.size())
        return 1;

    description = _allModes[curMode].description;
    procToControl = _allModes[curMode].processes;
    urlToControl = _allModes[curMode].urls;
    progress = _allModes[curMode].progress;
    
    return 0;
}

int ProcController::getModeProgress(string& name)
{
    for (int i = 0; i < _allModes.size(); ++i) {
        if (_allModes[i].name == name)
            return _allModes[i].progress;
    }
    return -2;
}

int ProcController::editMode(string& oldName, string& newName, 
    string& newDescr, vector<string>& newProcs, 
    vector<string>& newUrls, short& newProgress)
{
    if (newProcs.size() == 0 && newUrls.size() == 0)
        return EMPTY_LISTS_ERROR;

    if (newProgress > 100)
        return BAD_PROGRESS_ERROR;

    if (newName.length() == 0)
        return BAD_NAME_ERROR;

    int curMode = -1;
    int count = 0;
    for (int i = 0; i < _allModes.size(); ++i)
    {
        if (_allModes[i].name == oldName)
            curMode = i;
        if (_allModes[i].name == newName && oldName != newName)
            return MODE_ALREADY_EXISTS_ERROR;
    }
    if (curMode == -1)
        return NO_MODE_TO_EDIT;

    _allModes[curMode].name = newName;
    _allModes[curMode].description = newDescr;
    _allModes[curMode].processes = newProcs;
    _allModes[curMode].urls = newUrls;
    _allModes[curMode].progress = newProgress;

    this->rewriteModeFile();

    return 0;
}

int ProcController::deleteMode(string name)
{
    for (vector<Mode>::iterator mode = _allModes.begin(); mode != _allModes.end(); ++mode)
    {
        if (mode->name == name)
        {
            _allModes.erase(mode);
            this->rewriteModeFile();
            return 0;
        }
    }

    return 1;
}

int ProcController::addNewMode(string& name, string& description,
    vector<string>& procToControl, vector<string>& urlToControl, short& progress)
{
    for (vector<Mode>::iterator mode = _allModes.begin(); mode != _allModes.end(); ++mode)
    {
        if (mode->name == name)
            return MODE_ALREADY_EXISTS_ERROR;
    }

    if (procToControl.size() == 0 && urlToControl.size() == 0)
        return EMPTY_LISTS_ERROR;

    if (progress > 100)
        return BAD_PROGRESS_ERROR;

    if (name.length() == 0)
        return BAD_NAME_ERROR;

    Mode newMode = { name, description, procToControl, urlToControl, progress };
    _allModes.push_back(newMode);

    rewriteModeFile();

    return 0;
}

int ProcController::setSessionMode(string& name)
{
    _activeModeName = name;

    std::stringstream ss;
    std::string item;

    for (vector<Mode>::iterator mode = _allModes.begin(); mode != _allModes.end(); ++mode)
    {
        if (mode->name == name)
        {
            _curProcToControl = mode->processes;
            _curUrlToControl = mode->urls;
            _curProgress = mode->progress;

            // getting only .exe names [%name%.exe]
            for (int i = 0; i < _curProcToControl.size(); ++i)
            {
                _curProcToControl[i].erase(0, 1);
                _curProcToControl[i].erase(_curProcToControl[i].length() - 1, 1);

                ss.str(_curProcToControl[i]);
                ss.clear();
                while (std::getline(ss, item, '/')) {
                    _curProcToControl[i] = item;
                }
            }
            return 0;
        }
    }
    return 1;
}

int ProcController::control()
{
    killControlledProcesses();
    hookBrowsers();
    return 0;
}

string ProcController::getActiveModeName()
{
    return _activeModeName;
}

int ProcController::editProgress(string& name, int newProgress)
{
    if (newProgress < 0)
        return 1;

    for (vector<Mode>::iterator mode = _allModes.begin(); mode != _allModes.end(); ++mode)
    {
        if (mode->name == name)
        {
            mode->progress = newProgress;
            this->rewriteModeFile();
            return 0;
        }
    }
}

int ProcController::hookBrowserProcess(unsigned long pId)
{
    _logger.log("INFO: Injecting dll into process : %lu", pId);
    // injecting dll into process with id - pId
    if (_dllInjector.inject(pId, _dll86name)) {
        _logger.log("ERROR: Cannot inject dll.");
        return 1;
    }
    //// Create a pipe to send data
    //_logger.log("Creating a named pipe. %i", 10);
    //HANDLE pipe = CreateNamedPipeA(
    //    "\\\\.\\pipe\\my_restricted_urls_pipe", // name of the pipe
    //    PIPE_ACCESS_OUTBOUND, // 1-way pipe -- send only
    //    PIPE_TYPE_BYTE, // send data as a byte stream
    //    1, // only allow 1 instance of this pipe
    //    0, // no outbound buffer
    //    0, // no inbound buffer
    //    0, // use default wait time
    //    NULL // use default security attributes
    //    );

    //if (pipe == NULL || pipe == INVALID_HANDLE_VALUE) {
    //    _logger.log("ERROR: Failed to create outbound pipe instance. Error code: [%u]\n", GetLastError());
    //    return 1;
    //}

    //_logger.log("INFO: Waiting for browser process connects to the pipe.");
    //// This call blocks until a client process connects to the pipe
    //BOOL result = ConnectNamedPipe(pipe, NULL);
    //if (!result) {
    //    _logger.log("ERROR: Failed to make connection on named pipe. Error code: [%u]\n", GetLastError());
    //    CloseHandle(pipe); // close the pipe
    //    return 1;
    //}

    //
    //DWORD numBytesWritten = 0;

    //_logger.log("INFO: Sending data to injected dll.");
    //for (int i = 0; i < _curUrlToControl.size(); ++i) {
    //    // This call blocks until a client process reads all the data
    //    result = WriteFile(
    //        pipe,                           // handle to our outbound pipe
    //        _curUrlToControl[i].c_str() ,                         // data to send
    //        _curUrlToControl[i].length() * sizeof(char),  // length of data to send (bytes)
    //        &numBytesWritten,               // will store actual amount of data sent
    //        NULL                            // not using overlapped IO
    //        );

    //    if (!result) {
    //        _logger.log("ERROR: Failed to send data. Error code: [%u]\n", GetLastError());
    //    }
    //}
    //_logger.log("INFO: Restricted urls sended.");
    // Close the pipe (automatically disconnects client too)
    //CloseHandle(pipe);
    return 0;
}

void ProcController::hookBrowsers()
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
        hookBrowserProcess(*it);
    }  
}