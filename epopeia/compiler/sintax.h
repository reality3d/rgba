/*
 * Sintax analizer helper functions module header
 * Epopeia Scripting Demo System
 *      by Enlar/RGBA
 *
 * Based on a groupwork for the faculty by 
 *		Jose Manuel Olaizola
 *		Mari Joxe Azurtza
 *      Eneko Lacunza (Enlar)
 *
 */

#ifndef _SINTAX_H_
#define _SINTAX_H

#ifdef __cplusplus
extern "C" {
#endif


//#include "list.h"
#include "st.h"

void match(token_type type, token *tok);
 // Checks that lookahead is of type "type", and if so copies lookahead
 // to "tok".
 // Otherwise an error message is printed to std out 

// match tokens until given token type or EOF is found
void match_until(token_type type);

int is_lookahead(token_type type);
 // Checks wheter lookahead is of token_type type

int lookahead_col(void);
int lookahead_row(void);

#ifdef __cplusplus
};
#endif

#endif // _SINTAX_H_ 
