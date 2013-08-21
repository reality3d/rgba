#ifndef __EXPRESSION_H__
#define __EXPRESSION_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "../turb.h"

typedef double (TFunction) (double x);
typedef double (TFunction2) (double x, double y);
typedef double (TFunction3) (double x, double y, double z);
typedef double (TFunction4) (double x, double y, double z, double t);
typedef double (TFunction5) (double x, double y, double z, double t, double w);

typedef enum
{
    OP_NOP,         // No Operation
    OP_CONSTANT,    // Real constant value
    OP_IDENTIFIER,

    OP_ADD,         // expr + expr
    OP_SUB,         // expr - expr
    OP_MUL,         // expr * expr
    OP_DIV,         // expr / expr
    OP_POW,         // expr ^ expr
    OP_NEG,         // - expr
    OP_FUNCTION,    // Function
    OP_FUNCTION2,   // Function (2 parameters)
    OP_FUNCTION3,   // Function (3 parameters)
    OP_FUNCTION4,   // Function (4 parameters)
    OP_FUNCTION5    // Function (5 parameters)
} TOperator;

typedef struct TExpr_t
{
    TOperator  op;
    double     value;
    char      *id;

    TFunction  *func;
    TFunction2 *func2;
    TFunction3 *func3;
    TFunction4 *func4;
    TFunction5 *func5;

    struct TExpr_t *left;
    struct TExpr_t *right;

    struct TExpr_t *right2;
    struct TExpr_t *right3;
    struct TExpr_t *right4;
} TExpr;

typedef struct
{
    char   *id;
    double  value;
} TVariable;

// Lista de variables?
void   VL_Init   (void);
void   VL_AddVar (char *id);
void   VL_Set    (char *id, double value);
double VL_Get    (char *id);

TExpr *EXPR_Number     (double value);
TExpr *EXPR_Identifier (char *lexem);
TExpr *EXPR_BinaryOp   (TOperator op, TExpr *expr1, TExpr *expr2);
TExpr *EXPR_UnaryOp    (TOperator op, TExpr *expr);
TExpr *EXPR_Function   (TFunction  *f, TExpr *expr);
TExpr *EXPR_Function2  (TFunction2 *f, TExpr *expr1, TExpr *expr2);
TExpr *EXPR_Function3  (TFunction3 *f, TExpr *expr1, TExpr *expr2, TExpr *expr3);
TExpr *EXPR_Function4  (TFunction4 *f, TExpr *expr1, TExpr *expr2, TExpr *expr3, TExpr *expr4);
TExpr *EXPR_Function5  (TFunction5 *f, TExpr *expr1, TExpr *expr2, TExpr *expr3, TExpr *expr4, TExpr *expr5);
void   EXPR_Delete     (TExpr *expr);
double EXPR_Eval       (TExpr *expr);


TExpr *EXPR_New (char *expression);

#ifdef __cplusplus
};
#endif


#endif // __EXPRESSION_H__
