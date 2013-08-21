/*
 * Finite Lexer Automat module
 * Epopeia Scripting Demo System
 *      by Enlar/RGBA
 *
 * Based on a groupwork for the faculty by 
 *		Jose Manuel Olaizola
 *		Mari Joxe Azurtza
 *      Eneko Lacunza (Enlar)
 *
 */
#ifndef AUTOMAT_H
#define AUTOMAT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#ifdef LINUX
  #include <ctype.h>
#endif

#define BLANK       0
#define LETTER      1
#define DIGIT       2
#define UNDERSCORE  3
#define COLON       4
#define FULLSTOP    5
#define E           6
#define PLUS        7
#define MINUS       8
#define MUL         9
#define DIV        10
#define PERCENT    11
#define COMP       12 // <, >
#define EQUAL      13
#define SIGNS      14 // ;, , , (, )
#define QUOTE      15
#define FINAL      16 // final state?

#define STATES      20
#define ALPHABET    16

static int group(char c);

static const int transitions[STATES][ALPHABET+1] = {
/* t | h | d | _ | : | . | E | + | - | * | / | % |<> | = |,;)| " | Final state? */
/*---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+*/
 { 0 , 7 , 1 ,-1 , 13,16 , 7 , 8 , 8 , 9 , 17, 19, 10, 12, 12, 15, 0 }, /* 0*/
 {-1 ,-1 , 1 ,-1 ,-1 , 2 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 , 1 }, /* 1*/
 {-1 ,-1 , 3 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 , 0 }, /* 2*/
 {-1 ,-1 , 3 ,-1 ,-1 ,-1 , 4 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 , 1 }, /* 3*/
 {-1 ,-1 , 6 ,-1 ,-1 ,-1 ,-1 , 5 , 5 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 , 0 }, /* 4*/
 {-1 ,-1 , 6 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 , 0 }, /* 5*/
 {-1 ,-1 , 6 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 , 1 }, /* 6*/
 {-1 , 7 , 7 , 7 ,-1 ,-1 , 7 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 , 1 }, /* 7*/
 {-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 , 14,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 , 1 }, /* 8*/
 {-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 , 1 }, /* 9*/ // Multiply operator
 {-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 , 11,-1 ,-1 , 1 }, /*10*/
 {-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 , 1 }, /*11*/
 {-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 , 11,-1 ,-1 , 1 }, /*12*/
 {-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 , 12,-1 ,-1 , 1 }, /*13*/
 {-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 , 1 }, /*14*/ // single line comment (ada or C).
 { 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 16, 0 }, /*15*/ // "string" reading
 {-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 , 1 }, /*16*/ // "string" or "." readed
 {-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 , 18, 14,-1 ,-1 ,-1 ,-1 ,-1 , 1 }, /*17*/ // Division operator
 {-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 , 1 }, /*18*/ // C-style comment (el final se busca "a mano")
 {-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 ,-1 , 1 }, /*19*/ // % operator

};

//-------------------------------------------------------------------
int
is_final(int state)
{
    return transitions[state][FINAL];
}

//-------------------------------------------------------------------
int
transition(int state, char c)
{
    int i;

    i= group (c);
    return transitions[state][i];
}

//-------------------------------------------------------------------
static int
group(char c)
{
    c=tolower(c);

    if(islower(c))
    {
        if(c=='e')
            return E;
        else
            return LETTER;
    }
    if(isdigit(c))
        return DIGIT;

    switch(c)
    {
        case '_':
            return UNDERSCORE;
        case ':':
            return COLON;
        case '.':
            return FULLSTOP;
        case '+':
            return PLUS;
        case '-':
            return MINUS;
        case ' ':
        case '\t':
        case '\n':
            return BLANK;
        case '*':
            return MUL;
        case '/':
            return DIV;
        case '%':
            return PERCENT;
        case '>':
        case '<':
            return COMP;
        case '=':
            return EQUAL;
        case ',':
        case ';':
        case '(':
        case ')':
            return SIGNS;
        case '\"':
             return QUOTE;
        default:
//            printf("Caracter desconocido \"%c\" -> lo tomo como blanko.\n", c);
            break;
    }
    return BLANK;
}
#ifdef __cplusplus
};
#endif

#endif /* AUTOMAT_H */
