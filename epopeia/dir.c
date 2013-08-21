#ifdef WIN32
#include <windows.h>

typedef struct {
    HANDLE handle;
    WIN32_FIND_DATA file_data;
    BOOL file_data_used;
} DIRECTORY;

//-----------------------------------------------------------------------------
void DIRECTORY_Close(DIRECTORY *dir)
{
    FindClose(dir->handle);
    dir->handle = INVALID_HANDLE_VALUE;
    free(dir);
}

//-----------------------------------------------------------------------------
const char *DIRECTORY_GetNextFilename(DIRECTORY *dir)
{
    if(!dir->file_data_used)
    {
        dir->file_data_used = TRUE;
        return dir->file_data.cFileName;
    }
    if(!FindNextFile(dir->handle, &dir->file_data))
        return NULL;

    return dir->file_data.cFileName;
}

//-----------------------------------------------------------------------------
DIRECTORY *DIRECTORY_Open(const char *path)
{
    DIRECTORY *dir;

    dir = malloc(sizeof(DIRECTORY));
    if(dir == NULL)
        return NULL;

    dir->handle = FindFirstFile(path, &dir->file_data);
    if(dir->handle == INVALID_HANDLE_VALUE)
    {
        free(dir);
        return NULL;
    }
    dir->file_data_used = FALSE;
    return dir;
}
#endif // WIN32
