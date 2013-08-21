///////////////////////////////////////////////////////////////////////
//
// JPEG Image
//
///////////////////////////////////////////////////////////////////////
#ifndef __JPEG_IMAGE_H__
#define __JPEG_IMAGE_H__

#ifdef __cplusplus
extern "C"
{
#endif

#include "../image.h"


// RETURN:
//  0 - Error (Use PNG_GetError() for more info)
//  1 - OK
image_t JPEG_Read (char *filename);


// RETURN:
//  Last error 'string'
char *JPEG_GetError (void);


#ifdef __cplusplus
};
#endif

#endif // __JPEG_IMAGE_H__
