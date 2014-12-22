#include "Logger.h"

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
    va_list arguments;
    va_start(arguments, format);
    vfprintf(pfile_, format, arguments);
    va_end(arguments);
    fprintf(pfile_, " | PROCESS: [%i]\n", GetCurrentProcessId());
    

    fclose(pfile_);
}

Logger::~Logger() {
    delete filename_;
}