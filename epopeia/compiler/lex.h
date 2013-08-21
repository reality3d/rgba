/*
 * Lexer module header
 * Epopeia Scripting Demo System
 *      by Enlar/RGBA
 *
 * Based on a groupwork for the faculty by 
 *		Jose Manuel Olaizola
 *		Mari Joxe Azurtza
 *      Eneko Lacunza (Enlar)
 *
 */

#ifndef _LEX_H_
#define _LEX_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {  TOKEN_FIRST = 0,
                TOKEN_ANY,
                TOKEN_RESERVED_WORD,
                TOKEN_OP_COMPARISON, 
                TOKEN_OP_TERM,  /* + - */
                TOKEN_OP_FACTOR, /* * / */
                TOKEN_OP_BOOL, /* AND, OR... */
                TOKEN_NUMBER,
                TOKEN_SIGNS,
                TOKEN_IDENTIFIER,
                TOKEN_MARK_RW_BEGIN,
                    BEGIN, END, AT, COOLKEYS, SETORDER, SETTIME, WARNINGLEVEL, FORCEMODTIMING,
                    DEMONAME, STARTMESSAGE, ENDMESSAGE, MODNAME, UNTIL, DO, EVERY, SET,
                TOKEN_MARK_RW_END,
                CLOSE_PAREN,
                OPEN_PAREN,
                COMA,
                SEMICOLON,
                COLON,
                EQUAL,
                FULLSTOP,
                COMMENT,
                MULTILINE_COMMENT,
                STRING,
                TOKEN_TYPENAME,
                TOKEN_EOF,
                TOKEN_LAST
} token_type;

typedef struct {
    token_type type;
    char *lexem;
    double value;
    int num_type;
    int row;
    int col;
} token;

        int   lex_init(char *filename);
        void  lex_next_token(void);
        void  lex_finish(void);
        char *lex_token_typename(token_type m);
//        int   lex_reserved_word_type(char *lexem);
        
        token lookahead;

#ifdef __cplusplus
};
#endif

        
#endif /* _LEX_H_ */
