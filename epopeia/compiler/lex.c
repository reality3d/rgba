/*
 * Lexer module body
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

#include <packer/packer.h>
#define fclose pak_close
#define fopen  pak_open
#define fwrite pak_write
#define fread  pak_read
#define ftell  pak_ftell
#define fseek  pak_fseek
#undef feof
#define feof   pak_feof
#define fgetc  pak_fgetc
#define FILE   PFILE


#include <string.h>
#include <stdlib.h>
//#include <io.h>
#include <errno.h>
#define DEBUG_VAR 0
#define DEBUG_MODULE "lex"
#include "../debug.h"

#include "../epopeia.h"
#include "lex.h"
#include "automat.h"
#include "st.h" /* TYPE_REAL / TYPE_INT */
#include "errors.h"

#ifdef LINUX
#define stricmp strcasecmp
#endif

#ifdef WIN32
#define strcasecmp stricmp
#endif

#define LEXEM_MAX  256


//-------------------------------------------------------------------
// Global variables
//-------------------------------------------------------------------

    token lookahead;

//-------------------------------------------------------------------
// Funtions callable from other modules
//-------------------------------------------------------------------

 int  lex_init(char *filename);
void  lex_next_token(void);
void  lex_finish(void);
char *lex_token_typename(token_type m);

//-------------------------------------------------------------------
// Local Definitions
//-------------------------------------------------------------------

static char *token_typenames[TOKEN_LAST] =
{
    "-?-FIRST-?-",
    "ANY TOKEN",
    "RESERVED WORD",
    "COMPARISON OPERATION",
    "TERM OPERATION (+, -)",
    "FACTOR OPERATION (*, /, %)",
    "BOOLEAN OPERATION",
    "NUMBER",
    "PUNCTUATION SIGN",
    "IDENTIFIER",
    "-?-RESERVED WORD START MARK-?-",
        "BEGIN", "END", "AT", "CoolKeys", "SetOrder", "SetTime", "WarningLevel", "ForceModTiming",
        "DemoName", "StartMessage", "EndMessage", "ModName", "Until", "Do", "Every", "Set",
    "-?-RESERVED WORD END MARK-?-",
    ")", "(", ",", ";", ":", "=", "."
    "COMMENT",
    "MULTILINE COMMENT",
    "STRING",
    "TYPE NAME",
    "END OF FILE",
    "-?-LAST-?-"
};

    static FILE *fp;
    static int save = -1;
    static int row, col;

    static int   next_char(void);
    static void  save_char(char c);
    static void  lexical_error(int state);
    static void  new_token(token *t, int state, char *lexem);
    static int   lex_reserved_word_type(char *lexem);
    static char *without_blankspaces(char *s);
    static void  find_line_end(void);
    static void  find_multiline_comment_end(void);


//-------------------------------------------------------------------
char *
lex_token_typename(token_type m)
{
    if(m>TOKEN_FIRST && m<TOKEN_LAST)
    {
        return token_typenames[m];
    }
    else
    {
        printf("\nError! No existe el tipo de token (%d)\n", m);
        exit(1);
    }
}

//-------------------------------------------------------------------
int
lex_init(char *filename)
{
    fp = fopen(filename, "rt");
    if(fp==NULL) {
        printf("\nError: no se puede abrir fichero \"%s\"\n", filename);
        return -1;
    }

    save=-1;
    row = 1;
    col = 1;
    lookahead.lexem=NULL;
    lookahead.type =-1;
    lookahead.value=-1;
    lookahead.num_type=-1;
    lookahead.row  =0;
    lookahead.col  =0;

//    printf("\nLEX using file: %s\n", filename);

    return 0;
}

//-------------------------------------------------------------------
void
lex_next_token(void)
{
    char c;
    int i;
    int state=0;
    int new_state;
    char lexem[LEXEM_MAX];
    char *lex=lexem;

    lookahead.row = row;
    lookahead.col = col;

    DEBUG_msg("lex_next_token started");
    i = next_char();

    while(!feof(fp) && i>=0) {
        DEBUG_msg_dec("Feof ", fp->eof);
        c=(char)i;
        new_state=transition(state, c);
        DEBUG_msg("Got transition");
        if(new_state==-1) {     // there's no transition
            save_char(c);
            DEBUG_msg("There isn't transition");
            if(is_final(state)) {
                DEBUG_msg("transition is final");
                *lex='\0';
                if(state==COMMENT_STATE)
                {
                    find_line_end();
                    lex_next_token();
                }
                else if(state == MULTILINE_COMMENT_STATE)
                {
                    find_multiline_comment_end();
                    lex_next_token();
                }
                else
                    new_token(&lookahead, state, lexem);
                return;
            } else {
                DEBUG_msg("Transition isn't final");
                lexical_error(state);
                return;
            }
        } else {    // There's a transition, char acepted
            DEBUG_msg("There is transition");
            *lex=c;
            lex++;
            i=next_char();
            state=new_state;
//            printf("%d irakurrita\n", i);
        }
    }
    DEBUG_msg("Out of loop");
    DEBUG_msg_dec("Feof ", fp->eof);
    if(feof(fp)) {
        lookahead.type  = TOKEN_EOF;
        lookahead.lexem = "<fitxategi bukaera>";
        lookahead.value = -1;
        lookahead.num_type=-1;
    } else {
        //        printf("LEX: File read error! (%d)\n", errno);
        printf("LEX: File read error!\n");
        exit(1);
    }
}

//-------------------------------------------------------------------
void
lex_finish(void)
{
    fclose(fp);
}

//-------------------------------------------------------------------
static int
lex_reserved_word_type(char *lexem)
{
    int i;
    int found = 0;

    for(i=TOKEN_MARK_RW_BEGIN+1; i<TOKEN_MARK_RW_END && !found; i++)
    {
        found = !stricmp(lexem, token_typenames[i]);
    }

    if(found)
        return i-1;
    else
    {
        if(Type_GetId(lexem) != NULL)
        {
            DEBUG_msg("Reserved word is type\n");
            return TOKEN_TYPENAME;
        }
        DEBUG_msg("Reserved word is identifier\n");
        return TOKEN_IDENTIFIER;
    }

}

//-------------------------------------------------------------------
// Local functions
//-------------------------------------------------------------------

// This could be implemented with one or two buffers for more speed
static int
next_char(void)
{
    int i;
    static int previous = -1;

    if(save!=-1) {
        i=save;
        save=-1;
        DEBUG_msg_dec("Returning ", i);
        return i;
    }
    if(previous!=-1) {
        if(((char)previous)=='\n') {
            row++;
            col=1;
        } else {
            col++;
        }
    }
    i = fgetc(fp);
    DEBUG_msg_dec("Got char ", i);
    DEBUG_msg_dec("Size ", fp->size);
    DEBUG_msg_dec("Pos ", fp->position);
    DEBUG_msg_dec("Returning ", i);
    previous = i;
    return i;
}

//-------------------------------------------------------------------
static void
save_char(char c)
{
    save = c;
}

//-------------------------------------------------------------------
void
new_token(token *t, int state, char *lexem)
{
    char *ctemp;

//    printf("new token: %s\n", lexem);
    DEBUG_msg("without blankspaces is going to start");
    DEBUG_msg_str("leex: ", lexem);
    DEBUG_msg("without blankspaces is going to start");
    lexem    = without_blankspaces(lexem);
    DEBUG_msg("without blankspaces is over");
    t->lexem = strdup(lexem);
    if(t->lexem==NULL)
    {
        printf("Error: not enough memory! (LEX/new_token)\n");
        exit(1);
    }
    DEBUG_msg("Token: ")
    DEBUG_msg(lexem);
    switch(state) {
        case 1:
            t->type     = TOKEN_NUMBER;
            t->num_type = TYPE_INT;
            t->value    = (double)atoi(lexem);
            break;
        case 3:
        case 6:
            t->type     = TOKEN_NUMBER;
            t->num_type = TYPE_REAL;
            t->value    = atof(lexem);
            break;
        case 7:
            t->type = lex_reserved_word_type(lexem);
            break;
        case 8:
            t->type = TOKEN_OP_TERM;
            break;
        case 9:
            t->type = TOKEN_OP_FACTOR;
            break;
        case 10:
            t->type = TOKEN_OP_COMPARISON;
            break;
        case 11:
            t->type = TOKEN_OP_COMPARISON;
            break;
        case 12:
            switch(*lexem) {
                case ';':
                    t->type = SEMICOLON;
                    break;
                case ',':
                    t->type = COMA;
                    break;
                case '(':
                    t->type = OPEN_PAREN;
                    break;
                case ')':
                    t->type = CLOSE_PAREN;
                    break;
                case '=': 
                    t->type = EQUAL;
                    break;
            }
            break;
        case 13:
            t->type = COLON;
            break;
        case 16:
            if(0 == strcmp(t->lexem, "."))
            {
                t->type = FULLSTOP;
                break;
            }
                   
            t->type = STRING;
            ctemp = strdup(t->lexem + 1);  // dupe without initial '\"'
            if(ctemp==NULL) {
                printf("Error: not enough memory! (LEX/new_token)\n");
                exit(1);
            }
            free(t->lexem);
            t->lexem = ctemp;
            t->lexem[strlen(t->lexem)-1] = '\0'; // Get rid of final '\"'
            break;
        case 17:
            t->type = TOKEN_OP_FACTOR;
            break;
        case 19:
            t->type = TOKEN_OP_FACTOR;
            break;
        default:
            printf("Error: can't create token for state %d! (LEX/new_token)\n", state);
            exit(1);
    }
}

//-------------------------------------------------------------------
void
lexical_error(int state)
{
    printf("Error lexico (linea %d, columna %d):\n", row, col);
    switch(state) {
        case 0:
            printf("\tSe leyeron solo espacios\n");
            break;
        case 2:
            printf("\tNo se han encontrado digitos despues del punto.\n");
            printf("\tSugerencia: si quieres un entero, quita el punto.\n");
            printf("\t            si quieres un numero real, escribe un 0 despues del punto.\n");
            break;
        case 4:
        case 5:
            printf("\tNo se ha encontrado exponente despues de la 'e'.\n");
            printf("\tSugerencia: quiza te falte un espacio despues de un numero.\n");
            break;
        default:
            printf("Error: no conozco este estado no-final (%d)\n", state);
            break;
    }
    error_count++;
    lex_next_token();
}

//-------------------------------------------------------------------
char *
without_blankspaces(char *s)
{
    while(*s==' ' || *s=='\n' || *s=='\t')
    {
        s++;
    }

    return s;
}

//-------------------------------------------------------------------
void
find_line_end(void)
{
    while(!feof(fp) && next_char()!='\n');
}

//-------------------------------------------------------------------
void
find_multiline_comment_end(void)
{
    int c;
    while(!feof(fp))
    {
        c = next_char();
        while(c == '*')
        {
            c = next_char();
            if(c == '/')
                return;
        }

    }
}
