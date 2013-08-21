#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>
#include <math.h>
#include "expr.h"
#include "calc.h"

//#define EXPR_VERBOSE

//
// Fast "variable list" hack ;)
//
#define MAX_VARIABLES 16
static TVariable var[MAX_VARIABLES];

void VL_Init (void)
{
    int i;
    for (i=0; i<MAX_VARIABLES; i++)
    {
        var[i].id    = NULL;
        var[i].value = 0.0;
    }
}

void VL_AddVar (char *id)
{
    int i;

    //printf ("VL_AddVar: %s\n", id);

    for (i=0; i<MAX_VARIABLES; i++)
    {
        // Si ya existe la variables, no hacemos nada
        if (var[i].id && (!strcmp (id, var[i].id)))
            return;
    }

    i = 0;
    while (var[i].id)
        i ++;

    assert (i != MAX_VARIABLES);
    var[i].id    = strdup (id);
    var[i].value = 0.0;
}

void VL_Set (char *id, double value)
{
    int i;

    for (i=0; i<MAX_VARIABLES; i++)
    {
        if (var[i].id && (!strcmp (id, var[i].id)))
        {
            var[i].value = value;
            return;
        }
    }

    printf ("VL_Set:: Variable %s not used!\n", id);
    exit (1);
}

static float frand (void)
{
    return (rand ( ) & 0xFF) / 255.0f;
}


double VL_Get (char *id)
{
    int i;

    if (!strcmp (id, "rnd"))
        return frand ( );

    for (i=0; i<MAX_VARIABLES; i++)
    {
        if (var[i].id && (!strcmp (id, var[i].id)))
            return var[i].value;
    }

    printf ("VL_Set:: Variable %s not used!\n", id);
    exit (1);
    return 0.0;
}


// -------------------- EXPRESSION EVALUATOR CODE --------------------

TExpr *EXPR_Number (double value)
{
    TExpr *expr = (TExpr *) malloc (sizeof (TExpr));
    assert (expr != NULL);
    memset (expr, 0, sizeof (TExpr));

    expr->op    = OP_CONSTANT;
    expr->value = value;

#ifdef EXPR_VERBOSE
    printf ("EXPR_Number: %f\n", value);
#endif

    return expr;
}

TExpr *EXPR_Identifier (char *lexem_id)
{
    TExpr *expr = (TExpr *) malloc (sizeof (TExpr));
    assert (expr != NULL);
    memset (expr, 0, sizeof (TExpr));

    VL_AddVar (lexem_id);

    expr->op = OP_IDENTIFIER;
    expr->id = strdup (lexem_id);

#ifdef EXPR_VERBOSE
    printf ("EXPR_Id: %s\n", lexem_id);
#endif

    return expr;
}

TExpr *EXPR_BinaryOp (TOperator op, TExpr *expr1, TExpr *expr2)
{
    TExpr *expr = (TExpr *) malloc (sizeof (TExpr));
    assert (expr != NULL);
    memset (expr, 0, sizeof (TExpr));

    expr->op    = op;
    expr->left  = expr1;
    expr->right = expr2;

#ifdef EXPR_VERBOSE
    {
        char *opstr[5] =
        {
            "Add", "Sub", "Mul", "Div", "Pow"
        };

        printf ("EXPR_Op2: %i (%s)\n", op, opstr[op-3]);
    }
#endif


    return expr;
}

TExpr *EXPR_UnaryOp (TOperator op, TExpr *expr1)
{
    TExpr *expr = (TExpr *) malloc (sizeof (TExpr));
    assert (expr != NULL);
    memset (expr, 0, sizeof (TExpr));

    expr->op   = op;
    expr->left = expr1;

#ifdef EXPR_VERBOSE
    printf ("EXPR_Op1: %i\n", op);
#endif

    return expr;
}

TExpr *EXPR_Function (TFunction *f, TExpr *expr1)
{
    TExpr *expr = (TExpr *) malloc (sizeof (TExpr));
    assert (expr != NULL);
    memset (expr, 0, sizeof (TExpr));

    expr->op   = OP_FUNCTION;
    expr->func = f;
    expr->left = expr1;

    return expr;
}

TExpr *EXPR_Function2  (TFunction2 *f, TExpr *expr1, TExpr *expr2)
{
    TExpr *expr = (TExpr *) malloc (sizeof (TExpr));
    assert (expr != NULL);
    memset (expr, 0, sizeof (TExpr));

    expr->op    = OP_FUNCTION2;
    expr->func2 = f;
    expr->left  = expr1;
    expr->right = expr2;

    return expr;
}

TExpr *EXPR_Function3 (TFunction3 *f, TExpr *expr1, TExpr *expr2, TExpr *expr3)
{
    TExpr *expr = (TExpr *) malloc (sizeof (TExpr));
    assert (expr != NULL);
    memset (expr, 0, sizeof (TExpr));

    expr->op     = OP_FUNCTION3;
    expr->func3  = f;
    expr->left   = expr1;
    expr->right  = expr2;
    expr->right2 = expr3;

    return expr;
}

TExpr *EXPR_Function4 (TFunction4 *f, TExpr *expr1, TExpr *expr2, TExpr *expr3, TExpr *expr4)
{
    TExpr *expr = (TExpr *) malloc (sizeof (TExpr));
    assert (expr != NULL);
    memset (expr, 0, sizeof (TExpr));

    expr->op     = OP_FUNCTION4;
    expr->func4  = f;
    expr->left   = expr1;
    expr->right  = expr2;
    expr->right2 = expr3;
    expr->right3 = expr4;

    return expr;
}

TExpr *EXPR_Function5 (TFunction5 *f, TExpr *expr1, TExpr *expr2, TExpr *expr3, TExpr *expr4, TExpr *expr5)
{
    TExpr *expr = (TExpr *) malloc (sizeof (TExpr));
    assert (expr != NULL);
    memset (expr, 0, sizeof (TExpr));

    expr->op     = OP_FUNCTION5;
    expr->func5  = f;
    expr->left   = expr1;
    expr->right  = expr2;
    expr->right2 = expr3;
    expr->right3 = expr4;
    expr->right4 = expr5;

    return expr;
}


void EXPR_Delete (TExpr *expr)
{
    if (expr->left)
        EXPR_Delete (expr->left);

    if (expr->right)
        EXPR_Delete (expr->right);

    if (expr->id)
        free (expr->id);

    free (expr);
}

double EXPR_Eval (TExpr *expr)
{
    switch (expr->op)
    {
    case OP_CONSTANT: return expr->value;
    case OP_IDENTIFIER: return VL_Get (expr->id);
    case OP_ADD: return (EXPR_Eval (expr->left) + EXPR_Eval (expr->right));
    case OP_SUB: return (EXPR_Eval (expr->left) - EXPR_Eval (expr->right));
    case OP_MUL: return (EXPR_Eval (expr->left) * EXPR_Eval (expr->right));
    case OP_DIV: return (EXPR_Eval (expr->left) / EXPR_Eval (expr->right));
    case OP_NEG: return (- EXPR_Eval (expr->left));

    case OP_FUNCTION:  return expr->func  (EXPR_Eval (expr->left));
    case OP_FUNCTION2: return expr->func2 (EXPR_Eval (expr->left), EXPR_Eval (expr->right));

    case OP_FUNCTION3: return expr->func3 (EXPR_Eval (expr->left), EXPR_Eval (expr->right), EXPR_Eval (expr->right2));
    case OP_FUNCTION4: return expr->func4 (EXPR_Eval (expr->left), EXPR_Eval (expr->right), EXPR_Eval (expr->right2), EXPR_Eval (expr->right3));
    case OP_FUNCTION5: return expr->func5 (EXPR_Eval (expr->left), EXPR_Eval (expr->right), EXPR_Eval (expr->right2), EXPR_Eval (expr->right3), EXPR_Eval (expr->right4));

    default:
        printf ("EXPR_Eval fail!!\n");
        exit (1);
        break;
    }

    return 0.0;
}

#include "parser.h"
extern FILE *yyin;

typedef struct
{
    char *data;
    int   index;
    int   size;
} TString;

TString *string_new (char *text)
{
    TString *s = (TString *) malloc (sizeof (TString));
    assert (s != NULL);

    s->data = strdup (text);
    s->size = strlen (text) + 1;
    s->data[s->size-1] = EOF;
    s->index = 0;

    return s;
}

int myferror (void *string)
{
    TString *s = (TString *) string;

    return 0;
}

int mygetc (void *string)
{
    TString *s = (TString *) string;
    int      ch;

    if (s->index >= s->size)
        return EOF;

    ch = s->data[s->index];
    s->index ++;

    return ch;
}

int myfread (char *buffer, int count, int size, void *string)
{
    int maxbytes = count * size;
    int bytes    = 0;
    TString *s = (TString *) string;

    while ((bytes < maxbytes) && (s->index < s->size))
    {
        *(buffer ++) = s->data[s->index ++];
        bytes ++;
    }

    return bytes;
}


TExpr *EXPR_New (char *expression)
{
    TString *str;
    str = string_new (expression);
    yyin = (FILE *) str;
    yyrestart (yyin);

    if (yyparse ( ))
    {
        printf ("EXPR_New(\"%s\"):: parse error!\n", expression);
        exit (1);
    }
    return parser_expression;
}
