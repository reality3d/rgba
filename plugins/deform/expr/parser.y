%{
#include <malloc.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "expr.h"
#include "calc.h"
%}

%token NUMBER
%token UNARY_FUNCTION
%token BINARY_FUNCTION
%token FUNCTION3D, FUNCTION4D, FUNCTION5D
%token IDENTIFIER
%token POW
%token EXIT

%left '+' '-'
%right ELEMENTARY_FUNCTION
%right BINARY_FUNCTION
%left '*' '/'
%right '^'
%right UMINUS
%right UPLUS

%%
lines
    : expr { parser_expression = $1.expr; }
    ;

expr
    : expr '+' expr { $$.expr = EXPR_BinaryOp (OP_ADD, $1.expr, $3.expr); }
    | expr '-' expr { $$.expr = EXPR_BinaryOp (OP_SUB, $1.expr, $3.expr); }
    | expr '*' expr { $$.expr = EXPR_BinaryOp (OP_MUL, $1.expr, $3.expr); }
    | expr '/' expr { $$.expr = EXPR_BinaryOp (OP_DIV, $1.expr, $3.expr); }
    | '(' expr ')'  { $$.expr = $2.expr; }
    | '[' expr ']'  { $$.expr = $2.expr; }
    | '|' expr '|'  { $$.expr = EXPR_Function (fabs, $2.expr); }
    | '-' expr  %prec UMINUS { $$.expr = EXPR_UnaryOp (OP_NEG, $2.expr); }
    | '+' expr  %prec UPLUS  { $$.expr = $2.expr; }
    | UNARY_FUNCTION '(' expr ')'      { $$.expr = EXPR_Function (*$1.f,   $3.expr); }
    | expr BINARY_FUNCTION expr             { $$.expr = EXPR_Function2 (*$2.f2, $1.expr, $3.expr); }
    | BINARY_FUNCTION '(' expr ',' expr ')' { $$.expr = EXPR_Function2 (*$1.f2, $3.expr, $5.expr); }
    | FUNCTION3D '(' expr ',' expr ',' expr ')' { $$.expr = EXPR_Function3 (*$1.f3, $3.expr, $5.expr, $7.expr); }
    | FUNCTION4D '(' expr ',' expr ',' expr ',' expr ')' { $$.expr = EXPR_Function4 (*$1.f4, $3.expr, $5.expr, $7.expr, $9.expr); }
    | FUNCTION5D '(' expr ',' expr ',' expr ',' expr ',' expr ')' { $$.expr = EXPR_Function5 (*$1.f5, $3.expr, $5.expr, $7.expr, $9.expr, $11.expr); }
    | NUMBER { $$.expr = EXPR_Number ($1.value); }
    | IDENTIFIER { $$.expr = EXPR_Identifier ($1.name); }
    ;

%%

void yyerror (char* errstr)
{
    printf("Error: %s\n", errstr);
    //return 0;
}
