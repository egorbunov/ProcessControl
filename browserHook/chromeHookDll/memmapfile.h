#ifdef MEMMAPFILE_EXPORTS
#define MEMMAPFILE_API __declspec(dllexport) 
#else
#define MEMMAPFILE_API __declspec(dllimport)
#endif


#include <stdio.h>
#include <stdlib.h>
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
        bool writeLine(const char *str);
        bool readInt(int *dst);
        bool close();
        void reset();
    };
}