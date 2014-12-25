#ifndef LOGGER_H_INCLUDED__
#define LOGGER_H_INCLUDED__

#ifdef LOGGER_EXPORTS
#define LOGGERDLL_API __declspec(dllexport) 
#else
#define LOGGERDLL_API __declspec(dllimport)
#endif

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <Windows.h>
#include "constants.h"

class Logger {
private:
    static const int MAX_PREFIX_LEN = 15;
    FILE *pfile_;
    char *filename_;
    bool isMuted_;
    bool addPidPrefix_;

    void _addPrefixAndPrint(const char* prefix, const char* format, va_list arguments);
public:
    Logger(const char *filename, 
           bool isNewFile = false, 
           bool addPidPrefix = true,
           bool isMuted = false);
    ~Logger();
    void log(const char *format, ...);
    void info(const char *format, ...);
    void error(const char *format, ...);
};

#endif