#ifndef CALL_H
#define CALL_H

#ifdef __cplusplus
extern "C" {
#endif

    typedef union {
	int i;
	float f;
	void *ptr;
    } UParam;

#ifdef WIN32 
int __cdecl Call(void* Funcion, UParam* ParamVector, int ParamCount);
#elif defined(LINUX)
int Call(void* Funcion, UParam* ParamVector, int ParamCount);
#else
  #error "Sistema no definido"
#endif

#ifdef __cplusplus
};
#endif

#endif

