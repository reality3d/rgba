/**********************************************************************/
/*                                                                    */
/*  Expression Evaluator  -  Lexical Scanner                          */
/*                                                                    */
/**********************************************************************/
%{
#include <stdio.h>
#include <ctype.h>
#include "calc.h"
#include "parser.h"
#include "../turb.h"

YYSTYPE yylval;

void   str2lower (char *string);
double my_min (double a, double b);
double my_max (double a, double b);
double my_rand (void);


double my_turb1d (double x, double oct, double mode);
double my_turb2d (double x, double y, double oct, double mode);
double my_turb3d (double x, double y, double z, double oct, double mode);

%}

%option noyywrap

digit       [0-9]
letter      [a-zA-Z_]
number      {digit}+\.?|{digit}*\.{digit}+
identifier  {letter}({letter}|{digit})*
wspace      [ \t]+

/**********************************************************************/
%%

{wspace}   { /* Skip blanks */ }
{number}   { sscanf (yytext, "%lf", &yylval.value); return NUMBER; }

   /* CONSTANTS */
"pi"     { yylval.value = 3.14159265358979; return NUMBER; }
"e"      { yylval.value = exp (1.0); return NUMBER; }


   /* FUNCTIONS (1 PARAMETER) */
"sqrt"   { yylval.f = &sqrt;  return UNARY_FUNCTION; }
"exp"    { yylval.f = &exp;   return UNARY_FUNCTION; }
"log"    { yylval.f = &log;   return UNARY_FUNCTION; }
"ln"     { yylval.f = &log;   return UNARY_FUNCTION; }
"log10"  { yylval.f = &log10; return UNARY_FUNCTION; }
"sin"    { yylval.f = &sin;   return UNARY_FUNCTION; }
"asin"   { yylval.f = &asin;  return UNARY_FUNCTION; }
"cos"    { yylval.f = &cos;   return UNARY_FUNCTION; }
"acos"   { yylval.f = &acos;  return UNARY_FUNCTION; }
"tan"    { yylval.f = &tan;   return UNARY_FUNCTION; }
"atan"   { yylval.f = &atan;  return UNARY_FUNCTION; }
"abs"    { yylval.f = &fabs;  return UNARY_FUNCTION; }

   /* FUNCTIONS (2 PARAMETER) */
"**"     { yylval.f2 = &pow;    return BINARY_FUNCTION; }
"^"      { yylval.f2 = &pow;    return BINARY_FUNCTION; }
"pow"    { yylval.f2 = &pow;    return BINARY_FUNCTION; }
"fmod"   { yylval.f2 = &fmod;   return BINARY_FUNCTION; }
"atan2"  { yylval.f2 = &atan2;  return BINARY_FUNCTION; }
"min"    { yylval.f2 = &my_min; return BINARY_FUNCTION; }
"max"    { yylval.f2 = &my_max; return BINARY_FUNCTION; }

   /* FUNCTIONS (3 PARAMETER) */
"turb1d"  { yylval.f3 = &my_turb1d; return FUNCTION3D; }

   /* FUNCTIONS (4 PARAMETER) */
"turb2d"  { yylval.f4 = &my_turb2d; return FUNCTION4D; }

  /* FUNCTIONS (5 PARAMETER) */
"turb3d"  { yylval.f5 = &my_turb3d; return FUNCTION5D; }


"exit"   { return EXIT; }
{identifier} { yylval.name = strdup (yytext); str2lower (yylval.name); return IDENTIFIER; }
\n|.     { return yytext[0]; }

%%

/**********************************************************************/

// Convierte una cadena a minusculas
void str2lower (char *string)
{
    char ch;

    while (ch = *string)
        *(string ++) = (char) tolower ((int) ch);
}

double my_min (double a, double b)
{
    return (a < b) ? a : b;
}

double my_max (double a, double b)
{
    return (a > b) ? a : b;
}

// [0..1]
double my_rand (void)
{
    static unsigned int myseed = 0;
    myseed = 0x269EC3 + myseed * 0x343FD;
    return (double) ((myseed >> 16) & 0x7FFF) / 32767.0;
}


double my_turb1d (double x, double oct, double mode)
{
    return TURB_Turbulence1d (x, (long) oct, (long) mode);
}

double my_turb2d (double x, double y, double oct, double mode)
{
    return TURB_Turbulence2d (x, y, (long) oct, (long) mode);
}

double my_turb3d (double x, double y, double z, double oct, double mode)
{
    return TURB_Turbulence3d (x, y, z, (long) oct, (long) mode);
}
