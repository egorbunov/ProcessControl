#include "../headers/Logger.h"

Logger::Logger(const char *filename, bool isNewFile, bool addPidPrefix, bool isMuted) {
    addPidPrefix_ = addPidPrefix;
    isMuted_ = isMuted;
    if (!isMuted_) {
        filename_ = new char[strlen(filename) + 1];
        strcpy_s(filename_, (strlen(filename) + 1) * sizeof(char), filename);

        errno_t err;
        if (isNewFile)
            err = fopen_s(&pfile_, filename, "wt");
        else
            err = fopen_s(&pfile_, filename, "a+");
        if (err != 0) {
            throw "Cannot open file";
        }
        fclose(pfile_);
    }
}

void Logger::_addPrefixAndPrint(const char* prefix, const char* format, va_list arguments) {
    if (fopen_s(&pfile_, filename_, "a+") != 0) {
        throw "Cannot log to file (cannot open log file)";
    }

    const int addLen = Logger::MAX_PREFIX_LEN + common_consts::UL_MAX_DIGIT_NUMBER;
    int len = (int)strlen(format) + addLen;

    // creating prefix
    char* newFormat = new char[len];

    char wholePrefix[addLen] = "";
    if (addPidPrefix_) {
        strcat_s(wholePrefix, "[");
        char strpid[common_consts::UL_MAX_DIGIT_NUMBER];
        _ultoa_s(GetCurrentProcessId(), strpid, common_consts::DECIMAL_NOTATION);
        strcat_s(wholePrefix, strpid);
        strcat_s(wholePrefix, "] ");
    }
    strcat_s(wholePrefix, prefix);
    strcpy_s(newFormat, len, wholePrefix);
    strcat_s(newFormat, len, format);

    // printing
    vfprintf(pfile_, newFormat, arguments);
    fprintf(pfile_, "\n");

    delete[] newFormat;
    fclose(pfile_);
}

void Logger::log(const char *format, ...) {
    if (!isMuted_) {
        va_list arguments;
        va_start(arguments, format);
        _addPrefixAndPrint("", format, arguments);
        va_end(arguments);
    }
}

void Logger::info(const char *format, ...) {
    if (!isMuted_) {
        va_list arguments;
        va_start(arguments, format);
        _addPrefixAndPrint("INFO: ", format, arguments);
        va_end(arguments);
    }
}

void Logger::error(const char *format, ...) {
    if (!isMuted_) {
        va_list arguments;
        va_start(arguments, format);
        _addPrefixAndPrint("ERROR: ", format, arguments);
        va_end(arguments);
    }
}

Logger::~Logger() {
    delete filename_;
}