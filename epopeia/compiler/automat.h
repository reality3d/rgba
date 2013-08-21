/*
 * Finite Lexer Automat module header
 * Epopeia Scripting Demo System
 *      by Enlar/RGBA
 *
 * Based on a groupwork for the faculty by 
 *		Jose Manuel Olaizola
 *		Mari Joxe Azurtza
 *      Eneko Lacunza (Enlar)
 *
 */

#ifndef _AUTOMAT_H_
#define _AUTOMAT_H_

#define COMMENT_STATE 14
#define MULTILINE_COMMENT_STATE 18

extern    int is_final(int state);
extern    int transition(int state, char c);


#endif /* _AUTOMAT_H_ */
