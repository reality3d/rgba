#ifndef VBO_H
#define VBO_H

#include "3dworld.h"

#define UPDATE_VERTICES 1
#define UPDATE_NORMALS	2

void UploadBuffers(object_type *);
void UpdateBuffers(object_type *,unsigned char);
void DestroyBuffers(object_type *);

#endif