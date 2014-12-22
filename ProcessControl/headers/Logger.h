#ifndef LOGGER_H_INCLUDED__
#define LOGGER_H_INCLUDED__

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

class Logger {
private:
    FILE *pfile_;
    char *filename_;
public:
    Logger(const char *filename, bool isNewFile = false);
    ~Logger();
    void log(const char *format, ...);
};

#endif