#ifndef EPOPEIA_EXPR
#define EPOPEIA_EXPR

#include "type.h"
#include "arglist.h"

typedef enum {
    EXPR_OP_INVALID,
    EXPR_OP_ADD,
    EXPR_OP_SUBSTRACT,
    EXPR_OP_MULTIPLY,
    EXPR_OP_DIVIDE,
    EXPR_OP_MODULE,
    EXPR_OP_NEGATIVE,
    EXPR_OP_INTEGER,
    EXPR_OP_REAL,
    EXPR_OP_STRING,
    EXPR_OP_CAST_REAL,
    EXPR_OP_CAST_INTEGER,
    EXPR_OP_FUNCTION_CALL,
    EXPR_OP_METHOD_CALL,
    EXPR_OP_VARIABLE
} TExprOp;

typedef struct _TExpr
{
    TExprOp   OpType;
    TTypeId   Type;
    struct _TExpr*    First;
    struct _TExpr*    Second;
    char*     ObjectName;
    char*     FunctionName;
    TArgList* Args;
    int       ValueInteger;
    double    ValueReal;
    void*     ValueObject;
} TExpr;

typedef struct
{
    TTypeId Type;
    void*   ValueObject;
    double  ValueReal;
    int     ValueInteger;
} TExprResult;


// Construccion
TExpr* Expr_NewAdd         (TExpr* E1, TExpr* E2);
TExpr* Expr_NewSubstract   (TExpr* E1, TExpr* E2);
TExpr* Expr_NewMultiply    (TExpr* E1, TExpr* E2);
TExpr* Expr_NewDivide      (TExpr* E1, TExpr* E2);
TExpr* Expr_NewModule      (TExpr* E1, TExpr* E2);
TExpr* Expr_NewNegative    (TExpr* E1);
TExpr* Expr_NewInteger     (int i);
TExpr* Expr_NewString      (char* s);
TExpr* Expr_NewReal        (double r);
TExpr* Expr_NewVariable    (char* var);
TExpr* Expr_NewCastInteger (TExpr* E1);
TExpr* Expr_NewCastReal    (TExpr* E1);
TExpr* Expr_NewFunctionCall(char* FunctionName, TArgList* Args);
TExpr* Expr_NewMethodCall  (char* ObjectName, char* MethodName, TArgList* Args);

// Ejecucion
TExprResult* Expr_MethodCall(const char* ObjectName, const char* FunctionName, TArgList* ArgExprs);
TExprResult* Expr_Solve(TExpr* E);

// Destruccion
void Expr_Delete(TExpr* E);



#endif //EPOPEIA_EXPR
