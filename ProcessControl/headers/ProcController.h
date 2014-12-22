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
#include <iostream>

#include "../../commonUtils/headers/memmapfile.h"
#include "../../commonUtils/headers/Logger.h"
#include "dllinjector.h"


using std::vector;
using std::string;
using std::set;

#define MODE_ALREADY_EXISTS_ERROR 1
#define TOO_BIG_DESCRIPTION_ERROR 2
#define EMPTY_LISTS_ERROR 3
#define BAD_PROGRESS_ERROR 4
#define BAD_NAME_ERROR 5
#define NO_MODE_TO_EDIT 6


class ProcController
{
    Logger _logger;
    struct Mode
    {
        string name;
        string description;
        vector<string> processes;
        vector<string> urls;
        short progress;
    };

    static const string _dll86name;

    static const string _dll64name;

    DllInjector _dllInjector;

    /// <summary>
    /// number of created objects of that class, I let to use only one in one program.
    /// </summary>
    static int _controllersCount;

    /// <summary>
    /// name of file, where all mods are stored (info about allowed sites, programs, etc...)
    /// </summary>
    const string _modsFilename;

    /// <summary>
    ///  array with names of processes we want to control in current session
    /// </summary>
    vector<string> _curProcToControl;

    /// <summary>
    /// array with site names, which we want to control in current session
    /// </summary>
    vector<string> _curUrlToControl;

    int _curProgress;

    string _activeModeName;

    vector<Mode> _allModes;

    set<string> _controlledBrowsers;

    set<DWORD> _hookedPids;

    my_shared_mem::MemMappedFile _mmfileSizes;
    my_shared_mem::MemMappedFile _mmfileUrls;

    int loadMods();

    int rewriteModeFile();

    void killProcessByName(string &filename);

    void killControlledProcesses();

    int hookBrowserProcess(unsigned long pId);

    void hookBrowsers();

    int createSessionSharedFiles();
    void destroySessionSharedFiles();
public:

    /// <summary>
    /// Fills given array with names of already created mods.
    /// </summary>
    /// <param name="names">The names vector to fill</param>
    /// <returns></returns>
    int getModsNames(vector<string>& names) const;

    /// <summary>
    /// Fills given array with desciptions of already created mods.
    /// </summary>
    /// <param name="descriptions">The descriptions vector to fill</param>
    /// <returns></returns>
    int getModsDscriprions(vector<string>& descriptions) const;

    /// <summary>
    /// Fills given array with progresses of already created mods.
    /// </summary>
    /// <param name="progress">The progresses vector to fill</param>
    /// <returns></returns>
    int getModsProgress(vector<short>& progress) const;

    /// <summary>
    /// Fills given arrays and variables with information about mode by name
    /// </summary>
    /// <param name="name">Name of mode.</param>
    /// <param name="procToControl">Array for names of processes, which are controlled by mode.</param>
    /// <param name="urlToControl">The URL to control.</param>
    /// <param name="progress">number (from 0 to 100) of progress for current mode, or -1 if mode has no progress.</param>
    /// <returns>0 if it succeeds (if mode was found in list), 1 if it fails (no mode with given name).</returns>
    int getModeInfo(string& name, string& description, vector<string>& procToControl, vector<string>& urlToControl, short& progress) const;

    int getModeProgress(string& name);

    int addNewMode(string &name, string &description, vector<string> &procToControl, vector<string> &urlToControl, short &progress);

    int editMode(string &oldName, string &newName, string &newDescr, vector<string> &newProcs, vector<string> &newUrls, short &newProgress);

    int editProgress(string& name, int progress);

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