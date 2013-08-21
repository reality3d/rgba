#ifndef __IMG_PNG_H__
#define __IMG_PNG_H__

#include "image.h"


image_t PNG_Read (char *file_name);


// RETURN:
//  Last error 'string'
char *PNG_GetError (void);


#endif // __IMG_PNG_H__
