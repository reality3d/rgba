#ifndef EPOPEIA_CONSTANT
#define EPOPEIA_CONSTANT

#include "type.h"

typedef struct {
    char* Name;
    TTypeId Type;
    int       ValueInteger;
    double    ValueReal;
    void*     ValueObject;
} TConstant;

int CompareConstantPtr(void* d1, void* d2);

#endif // EPOPEIA_CONSTANT
