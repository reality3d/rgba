#include "../assert.h"
#include "constant.h"

int CompareConstantPtr(void* d1, void* d2)
{
    TConstant* c1 = (TConstant*)d1;
    TConstant* c2 = (TConstant*)d2;
    ASSERT(c1 != NULL);
    ASSERT(c2 != NULL);
    return strcmp(c1->Name, c2->Name);
}
