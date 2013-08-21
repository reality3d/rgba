#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <stdio.h>

//#define DISABLE_DEBUG

#ifdef __cplusplus
extern "C" {
#endif


#ifndef DEBUG_MODULE
    #pragma msg "DEBUG_MODULE not defined!"
#endif /* DEBUG_MODULE */

#ifdef DISABLE_DEBUG
    #define DEBUG_msg(MSG)
    #define DEBUG_msg_dec(MSG, D)
    #define DEBUG_msg_hex(MSG, D)
    #define DEBUG_msg_real(MSG, D)
    #define DEBUG_msg_str(MSG, D)
#else
    #include <stdio.h>
    #define DEBUG_msg(MSG) {           \
        if(DEBUG_VAR & 0x0001) {       \
            DEBUG_printMsg(DEBUG_MODULE, __FILE__, __LINE__, MSG);\
        }                                                         \
    }
    #define DEBUG_msg_dec(MSG, D) {   \
        if(DEBUG_VAR & 0x0001) {      \
            DEBUG_printMsgDec(DEBUG_MODULE, __FILE__, __LINE__, MSG, D);\
        }                                                         \
    }
    #define DEBUG_msg_hex(MSG, D) {   \
        if(DEBUG_VAR & 0x0001) {      \
            DEBUG_printMsgHex(DEBUG_MODULE, __FILE__, __LINE__, MSG, D);\
        }                                                         \
    }

    #define DEBUG_msg_real(MSG, D) {   \
        if(DEBUG_VAR & 0x0001) {      \
            DEBUG_printMsgReal(DEBUG_MODULE, __FILE__, __LINE__, MSG, D);\
        }                                                         \
    }
    #define DEBUG_msg_str(MSG, D) {   \
        if(DEBUG_VAR & 0x0001) {      \
            DEBUG_printMsgString(DEBUG_MODULE, __FILE__, __LINE__, MSG, D);\
        }                                                         \
    }


#endif /* DISABLE_DEBUG */

#ifndef PUBLIC_RELEASE
extern void DEBUG_enableHGC(void);
extern void DEBUG_disableHGC(void);
#endif

extern void DEBUG_printMsg(const char *debug_module, char *file, int line, const char *MSG);
extern void DEBUG_printMsgDec(const char *debug_module, char *file, long line, const char *MSG, long D);
extern void DEBUG_printMsgHex(const char *debug_module, char *file, long line, const char *MSG, long H);
extern void DEBUG_printMsgReal(const char *debug_module, char *file, long line, const char *MSG, double f);
extern void DEBUG_printMsgString(const char *debug_module, char *file, long line, const char *MSG, const char* str);

#ifdef __cplusplus
};
#endif


#endif /* _DEBUG_H_ */
