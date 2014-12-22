#ifndef MEMMAPFILE_H_INCLUDED__
#define MEMMAPFILE_H_INCLUDED__

#include <cstdio>
#include <cstdlib>
#include <windows.h>

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
        bool readInt(int *dst);
        bool writeLine(const char *str);
        bool close();
        void reset();
    };
}


#endif