#ifndef MEMMAPFILE_H_INCLUDED__
#define MEMMAPFILE_H_INCLUDED__

#ifdef MEMMAPFILE_EXPORTS
#define MEMMAPFILE_API __declspec(dllexport) 
#else
#define MEMMAPFILE_API __declspec(dllimport)
#endif

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include "constants.h"

namespace my_shared_mem {
    class MemMappedFile {
        HANDLE hMapFile;
        size_t size;
        void *view;
        int curPosition;
        int writePosition;
        char *buffer;


    public:
        MemMappedFile();
        ~MemMappedFile();
        bool create(const char *name, size_t size);
        bool openExisting(const char *name, size_t size, int flags = FILE_MAP_ALL_ACCESS);
        char* readLine(char *dest);
        bool readDecimal(int *dst);
        bool readDecimal(unsigned long *dst);
        bool writeLine(const char *str);
        bool writeDecimal(int num);
        bool writeDecimal(unsigned long num);
        bool close();
        void reset();
    };
}


#endif