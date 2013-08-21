#ifndef CRITICAL_H
#define CRITICAL_H

//#define ASSERT(str)  if(!(str)) { CriticalExit("Assertion failed!", __FILE__, __LINE__); }
#include <assert.h>
#define ASSERT(str)  assert(str)

//void CriticalExit(const char* Msg, const char* FileName, long LineNumber);

#endif
