///////////////////////////////////////////////////////////////////////
//
// RGBA PACKER
//
///////////////////////////////////////////////////////////////////////
#ifndef __PACKER_H__
#define __PACKER_H__

//
// STATIC OR DYNAMIC LIBRARY
//
#ifdef PACKER_DLL
 #ifdef PACKER_EXPORT
  #define PACKER_API __declspec(dllexport)
 #else
  #define PACKER_API __declspec(dllimport)
 #endif
#else
 #define PACKER_API
#endif


#define PAK_FREE_DATA     0  // pak_close free memory
#define PAK_NOT_FREE_DATA 1  // pak_close don't free memory
#define PAK_OPEN_BINARY   2  // open file as binary
#define PAK_OPEN_ASCII    4  // open file as ASCII

//
// Common C I/O file descriptor
//
typedef struct
{
    char     *file_name;   // File name
    void     *data;        // Pointer to data in memory
    unsigned int size;     // Size of allocated data
    unsigned int position; // Current I/O position
    unsigned int mode;     // PAK_FREE_DATA, PAK_NOT_FREE_DATA...
    int      eof;          // atemped to read beyond end of file
} PFILE;


//
// Global Packer API
//
PACKER_API int pak_init (const char *file);
PACKER_API int pak_kill (void);


//
// Common C I/O Functions
//
PACKER_API PFILE *pak_open  (const char *file, const char *mode);
PACKER_API int    pak_read  (void *buffer, int size, int count, PFILE *pf);
PACKER_API int    pak_write (const void *buffer, int size, int count, PFILE *pf);
PACKER_API int    pak_close (PFILE *pf);
PACKER_API int    pak_ftell (PFILE *pf);
PACKER_API int    pak_fseek (PFILE *pf, int offset, int origin);
PACKER_API int    pak_feof(PFILE* pf);
PACKER_API int    pak_fgetc(PFILE* pf);

//
// File browsing routines
//
PACKER_API int pak_list_reset(void);    // 0 no files/error, true ok
PACKER_API char*pak_list_current_filename(void); // Current filename
PACKER_API int pak_list_current_size(void);     // Current file's size
PACKER_API int pak_list_next(void);     // Next filename, false: no more, true ok


#endif // __PACKER_H__
