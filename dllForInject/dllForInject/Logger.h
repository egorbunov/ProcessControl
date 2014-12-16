#ifdef LOGGER_EXPORTS
#define LOGGERDLL_API __declspec(dllexport) 
#else
#define LOGGERDLL_API __declspec(dllimport)
#endif

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