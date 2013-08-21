#ifdef WIN32
#include <windows.h>

typedef struct {
    HANDLE handle;
    WIN32_FIND_DATA file_data;
    BOOL file_data_used;
} DIRECTORY;
#endif

/**
 * Close the directory file listing handle.
 * \param dir Directory listing handle to close.
 */
void        DIRECTORY_Close(DIRECTORY *dir);

/**
 * Get the next filename in the directory.
 * \param dir Directory listing handle to get the next name from.
 * \result A filename string valid until next call to this function
 *         with the same dir handle, or a DIRECTORY_Close call with the
 *         same dir handle. NULL if error (including end of listing)
 */
const char *DIRECTORY_GetNextFilename(DIRECTORY *dir);

/**
 * Open a directory to list filenames inside.
 * \param path The directory path, relative or absolute.
 * \result A directory listing handle, or NULL if there was an error.
 */
DIRECTORY * DIRECTORY_Open(const char *path);
