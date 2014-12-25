#ifndef CONSTANTS_H_INCLUDED__
#define CONSTANTS_H_INCLUDED__


namespace common_consts {
	const int DECIMAL_NOTATION = 10;
	const int UL_MAX_DIGIT_NUMBER = 15;
	const int UI_MAX_DIGIT_NUMBER = 10;
    const int MAX_PATH_LENGTH = 250;
    const int MAX_PROCESS_NAME_LENGTH = 150;
	const char SHARED_FILE_WITH_LOG_PATH[] = "Global\\ProcessControlSFLogPath";
    const char REL_LOGGING_PATH[] = "..\\logging";
    const char MAIN_LOG_FILENAME[] = "..\\logging\\main_log.txt";
    const char TASKMGR_HOOKER_LOG_FILENAME[] = "..\\logging\\taskmgr_hooker_log.txt";

#if _DEBUG
    const bool IS_NO_LOGGING = false;
#else
    const bool IS_NO_LOGGING = true;
#endif
}

namespace browser_hook_consts {
	const char SHARED_FILE_WITH_SIZES[] = "Global\\ProcessControlAppSizes";
	const char SHARED_FILE_WITH_URLS[] = "Global\\ProcessControlAppRestrictedURLS";
    const char LOG_FILE_SUFFIX[] = "\\log_browser";
}

namespace taskmgt_hook_consts {
	const char SHARED_FILE_WITH_PIDS[] = "Global\\TaskmgrHookerPIDSFile";
    const char LOG_FILE_SUFFIX[] = "\\log_taskmgr";
}

#endif