#ifndef __PARSER_H__
#define __PARSER_H__

#include <math.h>
#include <string.h>
#include "expr.h"


typedef union
{
    double   value;
    char    *name;

    TFunction  *f;
    TFunction2 *f2;
    TFunction3 *f3;
    TFunction4 *f4;
    TFunction5 *f5;

    TExpr   *expr;
} calc_type;
#define YYSTYPE calc_type

void yyerror(char *errstr);
int  yylex ( );

TExpr *parser_expression;

#endif // __PARSER_H__
