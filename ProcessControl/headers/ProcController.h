#ifndef PROC_CONTROLLER_H_INCLUDED__
#define PROC_CONTROLLER_H_INCLUDED__

#include <vector>
#include <string>
#include <windows.h>
#include <process.h>
#include <Tlhelp32.h>
#include <winbase.h>
#include <string.h>
#include <set>
#include <cstdlib>
#include <fstream>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <sstream>

#include "../../commonUtils/headers/memmapfile.h"
#include "../../commonUtils/headers/Logger.h"
#include "dllinjector.h"
#include "mode.h"


using std::vector;
using std::string;
using std::set;

#define ERROR_MODE_ALREADY_EXISTS 1
#define ERROR_TOO_BIG_DESCRIPTION 2
#define ERROR_EMPTY_LISTS 3
#define ERROR_BAD_PROGRESS 4
#define ERROR_BAD_NAME 5
#define ERROR_NO_MODE_TO_EDIT 6
#define ERROR_MODE_FILE_CORRUPTED 7


class ProcController
{
private:
    Logger _logger;

    /**
     * @enum    ProcessType
     *
     * @brief   Tags for describing ids of processes.
     *          Accordingly to that enum values in pair with pid,
     *          program decides how to deal with process - hook it or close it, for ex.
     */
    enum ProcessType
    {
        BROWSER,
        TASK_MANAGER
    };

    static const string CHROME_HOOK_DLL_86;
    static const string CHROME_HOOK_DLL_64;

    DllInjector _dllInjector;

    /** @brief   Mode, which is currently chosen (if not null) */
    Mode *_currentMode;

    /** @brief   name of file, where all mods are stored (info about allowed sites, programs, etc...) */
    const string _modsFilename;

    vector<Mode> _allModes;

    set<string> _controlledBrowsers;

    set<DWORD> _hookedPids;

    my_shared_mem::MemMappedFile _mmfileSizes;
    my_shared_mem::MemMappedFile _mmfileUrls;

    int loadMods();

    /**
    * @fn  int ProcController::_rewriteModeFile()
    *
    * @brief   Rewrites the mode file. Fills file with name _modsFilename with information about
    *          modes: 1) In the first line: %number of modes% 2) Next 5 lines describes first mode
    *          and so on:
    *                              %name% %descriptions% %programs separated with "|"% %urls
    *                              separated with "|"% %progress (number)%.
    *
    * @author  Egor
    * @date    23.12.2014
    *
    * @return  0 if ok.
    */
    int _rewriteModeFile();

    /**
    * @fn  void ProcController::_killProcessByName(string& processName)
    *
    * @brief   Kills process by process name.
    *
    * @param [in,out]  processName process name.
    */
    void _killProcessByName(string &filename);

    /**
    * @fn  void ProcController::_killControlledProcesses()
    *
    * @brief   Kills all controlled by current mode processes.
    */
    void _killControlledProcesses();

    int _hookBrowserProcess(unsigned long pId);

    void _hookBrowsers();

    void _hookTaskmanagers();

    int _createSessionSharedFiles();

    void _destroySessionSharedFiles();
public:
    int getMode(string name, Mode &mode);

    int getModes(vector<Mode> &modes) const;

    /**
    * @fn  int ProcController::getModsProgress(vector<short>& progress) const
    *
    * @brief   Fills given array with progresses of already created mods.
    *
    * @param [in,out]  progress    The progresses vector to fill.
    *
    * @return  The mods progress.
    */
    int getModeProgress(string name) const;

    int addNewMode(Mode mode);

    int editMode(string oldName, Mode mode);

    int editProgress(string name, int progress);

    int deleteMode(string name);

    int setSessionMode(string &name);

    int control();

    void init();

    void uninit();

    string getActiveModeName();

    ProcController();
    ~ProcController();
};

#endif