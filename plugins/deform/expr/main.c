#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "expr.h"


int main (void)
{
    char text[256];
    TExpr *expr;

    TURB_Init (0);

    while (1)
    {
        //scanf ("%s", text);
        gets (text);
        printf ("Evaluating: \"%s\"\n", text);

        VL_Init ( );
        VL_AddVar ("t");
        expr = EXPR_New (text);

        VL_Set ("t", 1.0);
        printf ("eval: %f\n", EXPR_Eval (expr));
    }
}
