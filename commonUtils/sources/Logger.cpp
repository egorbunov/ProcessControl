#include "../headers/Logger.h"

Logger::Logger(const char *filename, bool isNewFile) {
    filename_ = new char[strlen(filename) + 1];
    strcpy_s(filename_, (strlen(filename) + 1) * sizeof(char), filename);

    errno_t err;
    if (isNewFile)
        err = fopen_s(&pfile_, filename, "wt");
    else
        err = fopen_s(&pfile_, filename, "wt+");
    if (err != 0) {
        throw "Cannot open file";
    }
    fclose(pfile_);
}

void Logger::log(const char *format, ...) {
    if (fopen_s(&pfile_, filename_, "a+") != 0) {
        throw "Cannot log to file (cannot open log file)";
    }

    // adding prefix
    const int addLen = 25; // additional length for prefix
    const int ulongDigitNum = 15; // just enough
    int len = (int) strlen(format) + addLen;
    char* newFormat = new char[strlen(format) + addLen];
    char prefix[addLen] = "[";
    char strpid[ulongDigitNum];
    _ultoa_s(GetCurrentProcessId(), strpid, 10);
    strcat_s(prefix, strpid);
    strcat_s(prefix, "] ");
    strcpy_s(newFormat, strlen(format) + addLen, prefix);
    strcat_s(newFormat, strlen(format) + addLen, format);

    // printing
    va_list arguments;
    va_start(arguments, format);
    vfprintf(pfile_, newFormat, arguments);
    va_end(arguments);
    fprintf(pfile_, "\n");

    delete[] newFormat;
    fclose(pfile_);
}

Logger::~Logger() {
    delete filename_;
}