#ifndef ERRORS_H
#define ERRORS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "lex.h"
    
    extern int error_count;


extern void error_set_output_file(const char* output_filename);
extern void error_semantic(char *msg, token *tok);
extern void error_semantic_full(char *msg, int row, int col);
extern void error_sintax(char *msg, token *tok);
extern  int error_get_count(void);
extern void error_runtime(char* msg);
extern void warning_semantic(token *tok, const char *msg);
extern void warning_runtime(char* msg);
extern void msg_print(const char* type, int row, int col, const char* msg);


#ifdef __cplusplus
};
#endif


#endif // ERRORS_H
