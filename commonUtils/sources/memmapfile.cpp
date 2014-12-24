#include "../headers/MemMapFile.h"

my_shared_mem::MemMappedFile::MemMappedFile() {
    hMapFile = NULL;
    curPosition = -1;
    writePosition = 0;
    size = 0;
    view = NULL;
    buffer = NULL;
}

/**
 * @fn  bool my_shared_mem::MemMappedFile::create(const char *name, size_t size)
 *
 * @brief   Creates shared file for read and write access with specified name. Every call of that
 *          function must be provided with close() call.
 *
 * @param   name    Name of the shared file.
 * @param   size    File max. size.
 *
 * @return  true if all is okay, and false else.
 */
bool my_shared_mem::MemMappedFile::create(const char *name, size_t size) {
    buffer = new char[size];

    this->size = size;
    hMapFile = CreateFileMappingA(
        INVALID_HANDLE_VALUE,    // use paging file
        NULL,                    // default security
        PAGE_READWRITE,          // read/write access
        0,                       // maximum object size (high-order DWORD)
        (DWORD) size,                    // maximum object size (low-order DWORD)
        name);           // name of mapping object

    if (hMapFile == NULL)
        return false;

    view = MapViewOfFile(hMapFile,   // handle to map object
                         FILE_MAP_ALL_ACCESS, // read/write permission
                         0,
                         0,
                         size);

    if (view != NULL)
        curPosition = 0;
    else
        return false;

    return true;
}

/**
 * @fn  bool my_shared_mem::MemMappedFile::openExisting(const char *name, size_t size, int flags)
 *
 * @brief   Opens the existing shared file. Every call of that function must be provided with
 *          close() call.
 *
 * @param   name    Name of the file to open.
 * @param   size    Size of the file.
 * @param   flags   The flags.
 *
 * @return  true is success and false else.
 */
bool my_shared_mem::MemMappedFile::openExisting(const char *name, size_t size, int flags) {
    buffer = new char[size];

    hMapFile = OpenFileMappingA(
        flags,   // read/write access
        FALSE,                 // do not inherit the name
        name);               // name of mapping object

    if (hMapFile == NULL)
        return false;

    void *pBuf = MapViewOfFile(hMapFile, // handle to map object
                               flags,
                               0,
                               0,
                               size);

    if (pBuf == NULL)
        return false;

    if (sprintf_s(buffer, size, "%s", pBuf) < 0)
        return false;

    curPosition = 0;

    return true;
}

/**
 * @fn  char* my_shared_mem::MemMappedFile::readLine(char *dest)
 *
 * @brief   Reads the line from shared file. Shared file must be opened before using that
 *          function;
 *          Every call will increase file iterator. To seek to file start use reset() method;
 *
 *
 * @param [in,out]  dest    buffer to write to.
 *
 * @return  NULL if error occured, valid pointer if else (dest points to the same mem. location)
 */
char* my_shared_mem::MemMappedFile::readLine(char *dest) {
    if (buffer == NULL)
        return NULL;

    int i = 0;
    while (curPosition < (int)strlen(buffer) && buffer[curPosition] != '\n') {
        if (dest == NULL)
            return NULL;
        dest[i++] = buffer[curPosition++];
    }
    ++curPosition;
    dest[i] = '\0';
    return dest;
}

bool my_shared_mem::MemMappedFile::writeLine(const char *str) {
    if (writePosition != 0)
        buffer[writePosition++] = '\n';
    for (int i = 0; i < (int)strlen(str); ++i) {
        buffer[writePosition++] = str[i];
    }
    buffer[writePosition] = '\0';
    CopyMemory((PVOID)view, buffer, (strlen(buffer) * sizeof(char)));
    return true;
}

bool my_shared_mem::MemMappedFile::close() {
    this->reset();
    if (view != NULL)
        UnmapViewOfFile(view);
    if (hMapFile != NULL)
        CloseHandle(hMapFile);
    if (buffer != NULL) {
        delete[] buffer;
        buffer = NULL;
    }
    return true;
}

my_shared_mem::MemMappedFile::~MemMappedFile() {
    if (buffer != NULL) {
        delete[] buffer;
    }
}

void my_shared_mem::MemMappedFile::reset() {
    writePosition = 0;
    if (buffer == NULL)
        curPosition = -1;
    else
        curPosition = 0;
}

bool my_shared_mem::MemMappedFile::readDecimal(int *dst) {
    char num[12];
    if (this->readLine(num) == NULL)
        return false;
    *dst = atoi(num);
    return true;
}

bool my_shared_mem::MemMappedFile::readDecimal(unsigned long *dst) {
    char num[15];
    if (this->readLine(num) == NULL)
        return false;
    *dst = (unsigned long) atoll(num);
    return true;
}

bool my_shared_mem::MemMappedFile::writeDecimal(int num) {
    char strnum[15];
    _itoa_s(num, strnum, 10);
    return writeLine(strnum);
}

bool my_shared_mem::MemMappedFile::writeDecimal(unsigned long num) {
    char strnum[15];
    _ultoa_s(num, strnum, 10);
    return writeLine(strnum);
}

