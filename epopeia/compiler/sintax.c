/*
 * Sintax analizer helper functions module body
 * Epopeia Scripting Demo System
 *      by Enlar/RGBA
 *
 * Based on a groupwork for the faculty by 
 *		Jose Manuel Olaizola
 *		Mari Joxe Azurtza
 *      Eneko Lacunza (Enlar)
 *
 */

#include <stdio.h>
#include <malloc.h>
#define DEBUG_VAR 0
#define DEBUG_MODULE "sintax"
#include "../debug.h"

#include "lex.h"
#include "st.h"
#include "errors.h"

//-------------------------------------------------------------------
void
match(token_type type, token *tok)
{
    char Tmp[80];
    if(tok!=NULL)
        *tok=lookahead;

    if(lookahead.type != type && type != TOKEN_ANY)
    {
        sprintf(Tmp, "\tSe esperaba \"%s\" pero se ha encontrado \"%s\".\n",
               lex_token_typename(type), lookahead.lexem);
        error_sintax(Tmp, &lookahead);
    }
    DEBUG_msg_str("match: ", lookahead.lexem);
    lex_next_token();

/*
    if(lookahead.type != END) // Debe haber un bug en la carga de PAK, pero...
        lex_next_token();
    else
    DEBUG_msg("BUUUUUUUUUGGGGG!");
*/
}

//-------------------------------------------------------------------
void match_until(token_type type)
{
    token tok;
    do {
        match(TOKEN_ANY, &tok);
    } while(tok.type != type && tok.type != TOKEN_EOF);
}

//-------------------------------------------------------------------
int
is_lookahead(token_type type)
{
    return (lookahead.type == type);
}

//-------------------------------------------------------------------
int
lookahead_col(void)
{
    return lookahead.col;
}

//-------------------------------------------------------------------
int
lookahead_row(void)
{
    return lookahead.row;
}

