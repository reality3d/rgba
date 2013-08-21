///////////////////////////////////////////////////////////////////////
//
// JPEG IO Manager
//
///////////////////////////////////////////////////////////////////////
#ifndef __JPEG_IO__
#define __JPEG_IO__

#ifdef LINUX
#include <jpeglib.h>
#else
#include "jpeglib.h"
#endif

typedef struct
{
    struct  jpeg_source_mgr  pub;	     // public fields
    void                    *infile;         // IO handle
    JOCTET                  *buffer;	     // start of buffer
    boolean                  start_of_file;  // have we gotten any data yet?
} JPEGIO_MANAGER;

GLOBAL (void) JPEGIO_Init (j_decompress_ptr cinfo, void *infile);


#endif // __JPEG_IO__
