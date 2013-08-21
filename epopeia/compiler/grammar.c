/*
 * Script grammar implementation
 * Epopeia Scripting Demo System
 *      by enlar/rgba
 *
 * Based on a groupwork for the faculty by 
 *		Jose Manuel Olaizola
 *		Mari Joxe Azurtza
 *              Eneko Lacunza (Enlar)
 *
 */
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "../epopeia.h"
#include "../epopeia_internal.h"
#include "../demo.h"
#include "lex.h"
#include "sintax.h"
#include "grammar.h"
#include "st.h"
#include "method.h"
#include "type_table.h"
#include "errors.h"
#include "../inter/command_list.h"
#include "../inter/command_list_builder.h"
#include "expression.h"
#include "function.h"
#include "function_table.h"
#define DEBUG_VAR 1
#define DEBUG_MODULE "grammar"
#include "../debug.h"

typedef unsigned long DWORD;


void DemoScript(void);
void Header(void);
void ModnameEnd(int number_processed);
void Body(void);
void CommandSequence(void);
void Command(void);
void CommandIdentifierEnd(token Ident);
TPos CommandUntilEvery(void);
TExpr* Expression(void);
TExpr* ExpressionEnd(TExpr* E1);
TExpr* Factor(void);
TExpr* FactorNoSign(void);
DWORD Integer(void);
float Real(void);
TArgList* ArgList(void);
void ArgListEnd(TArgList* ParamList);
TExpr* Term(void);
TExpr* TermEnd(TExpr* E1);
TExpr* IdentifierEnd(token *IdentToken);
TPos TimingPoint(void);

static int grammar_timing_por_tiempo = 0;
static int inside_until_statement = 0;


static int IsTimeTiming(char *SongFileName)
{
    char* OggExt[] = { ".ogg", ".mp3", ".wav", ".raw", ".mp2", NULL };
    int extension_mismatch = 0;
    int i;
    i = 0;
    do {
        char* tmpptr = SongFileName + strlen(SongFileName) - 4;
        extension_mismatch = strcasecmp(tmpptr, OggExt[i]);
        i++;
    } while(OggExt[i] != NULL && extension_mismatch);
    return !extension_mismatch;
}

static TPos PosStart;
static TPos PosStop;
static long Depth;
static int  PosValid = 0;
static char *InstanceName;
static int  InstanceNameNumber = 0;

//-------------------------------------------------------------------
void DemoScript(void)
{
    PosValid = 0;
    Header();
    Body();
}
//-------------------------------------------------------------------
void Header(void)
{
    token tok;

    if(is_lookahead(DEMONAME))
    {
        match(DEMONAME, NULL);
        match(EQUAL, NULL);
        match(STRING, &tok);
        DemoSetName(tok.lexem);
        match(SEMICOLON, NULL);
        Header();        
    }
    else if(is_lookahead(STARTMESSAGE))
    {
        match(STARTMESSAGE, NULL);
        match(EQUAL, NULL);
        match(STRING, &tok);
        DemoSetStartMessage(tok.lexem);
        match(SEMICOLON, NULL);
        Header();        
    }
    else if(is_lookahead(ENDMESSAGE))
    {
        match(ENDMESSAGE, NULL);
        match(EQUAL, NULL);
        match(STRING, &tok);
        DemoSetEndMessage(tok.lexem);
        match(SEMICOLON, NULL);
        Header();        
    }
    else if(is_lookahead(MODNAME))
    {
        match(MODNAME, NULL);
        match(EQUAL, NULL);
	match(STRING, &tok);
	if(Epopeia_GetSongNumber() == 1)
	{
	    DemoSetModFile(tok.lexem);
	    grammar_timing_por_tiempo = IsTimeTiming(tok.lexem);
	}
        ModnameEnd(1);
        match(SEMICOLON, NULL);
        Header();
    }
    else if(is_lookahead(COOLKEYS))
    {
        int b;
        match(COOLKEYS, NULL);
        match(EQUAL, NULL);
        b = Integer();
        g_Epopeia.MoveKeys = b;
        match(SEMICOLON, NULL);
        Header();
    }
    else if(is_lookahead(WARNINGLEVEL))
    {
        int b;
        match(WARNINGLEVEL, NULL);
        match(EQUAL, NULL);
        b = Integer();
        g_Epopeia.WarningLevel = b;
        match(SEMICOLON, NULL);
        Header();
    }
    else if(is_lookahead(FORCEMODTIMING))
    {
        int b;
        match(FORCEMODTIMING, NULL);
        match(EQUAL, NULL);
	b = Integer();
        if(b)
	    grammar_timing_por_tiempo = 0;
        match(SEMICOLON, NULL);
        Header();
    }
    else
    {
//        printf("Sintax error: no header!\n");
//        error_sintax(lookahead_row());
    }
}

//-------------------------------------------------------------------
void ModnameEnd(int number_processed)
{
    if(is_lookahead(COMA))
    {
	token tok;
        match(COMA, NULL);
	match(STRING, &tok);
	if(Epopeia_GetSongNumber() == number_processed+1)
	{
	    DemoSetModFile(tok.lexem);
	    grammar_timing_por_tiempo = IsTimeTiming(tok.lexem);
	}
	ModnameEnd(number_processed+1);
    }
}

//-------------------------------------------------------------------
void Body(void)
{
    CommandSequence();
//    match(END, NULL);
    DEBUG_msg("Body end");
}
//-------------------------------------------------------------------
void CommandSequence(void)
{
    if( is_lookahead(AT) || is_lookahead(SET) || is_lookahead(SETORDER) || is_lookahead(SETTIME) ||
        is_lookahead(TOKEN_TYPENAME) || is_lookahead(TOKEN_IDENTIFIER) ||
        is_lookahead(BEGIN) || is_lookahead(UNTIL) || is_lookahead(FULLSTOP))
    {
        Command();
        CommandSequence();
    } // else that's ok; empty command sequence is allowed
}
//-------------------------------------------------------------------
void Command(void)
{

//    DEBUG_msg_str("Have token: ", lex_token_typename(lookahead.type));
//    DEBUG_msg_str("Have token type: ", lookahead.lexem);
    if(is_lookahead(AT))
    {
        TPos pos;
        match(AT, NULL);
        pos = TimingPoint();
        match(SEMICOLON, NULL);
        CommandListBuilder_SetPos(pos.Order, pos.Row);
    }
    else if(is_lookahead(BEGIN))
    {
        match(BEGIN, NULL);
        CommandListBuilder_InsertMusicPlay();
    }
    else if(is_lookahead(SET))
    {

        match(SET, NULL);
	match(OPEN_PAREN, NULL);

	float t;
	int negativo;
        int row;
	negativo = 0;
        if(is_lookahead(TOKEN_OP_TERM) && lookahead.lexem[0] == '-')
        {
            match(TOKEN_OP_TERM, NULL);
            negativo = 1;
        }
        t = Real();
        row  =(int)(t*1000);
        if(negativo)
            row = -row;
	PosStart.Order = 0;
	PosStart.Row   = row;
	match(COMA, NULL);

	negativo = 0;
        if(is_lookahead(TOKEN_OP_TERM) && lookahead.lexem[0] == '-')
        {
            match(TOKEN_OP_TERM, NULL);
            negativo = 1;
        }
        t = Real();
        row  =(int)(t*1000);
        if(negativo)
            row = -row;
	PosStop.Order = 0;
	PosStop.Row   = row;
	match(COMA, NULL);

        Depth = Integer();

	match(CLOSE_PAREN, NULL);
        match(SEMICOLON, NULL);
        printf("Effect will start on %d; ends on %d; layer is %ld\n", PosStart.Row, PosStop.Row, Depth);
	CommandListBuilder_SetPos(PosStart.Order, PosStart.Row);
        PosValid = 1;
    }
    else if(is_lookahead(SETORDER) || is_lookahead(SETTIME))
    {
        TPos pos;
	DEBUG_msg("--->aki<---");
        if(is_lookahead(SETORDER))
	    match(SETORDER, NULL);
        else
	    match(SETTIME, NULL);
        pos = TimingPoint();
        match(SEMICOLON, NULL);
        CommandListBuilder_InsertSetOrder(pos.Order, pos.Row);
        DEBUG_msg("--->aki2<---");
    DEBUG_msg_str("Have token: ", lex_token_typename(lookahead.type));
    DEBUG_msg_str("Have token type: ", lookahead.lexem);
    DEBUG_msg_dec("Have token type: ", lookahead.row);
    }
    else if(is_lookahead(TOKEN_TYPENAME))
    {
        token Type, Ident;
        TArgList* ParamList;
        TTypeId Id;
        int error;
        
        match(TOKEN_TYPENAME, &Type);

        Id = Type_GetId(Type.lexem);

	if(PosValid)
	{
	    // FIXME: this is wrong
            InstanceName = malloc(10);
            sprintf(InstanceName, "%d", InstanceNameNumber);
	    Ident.lexem = InstanceName;
            InstanceNameNumber++;
	}
	else
	{
	    match(TOKEN_IDENTIFIER, &Ident);
	    if(ST_Exists(Ident.lexem))
		error_sintax("El identificador ya existe.", &Ident);
	}
	match(OPEN_PAREN, NULL);
        ParamList = ArgList();
        error = Type_CheckParams(Id, "New", ParamList);
        if(0 != error)
        {
            switch(error)
            {
                char msg[200];
            case -1:
                sprintf(msg, "Faltan parametros en la llamada a %s", Ident.lexem);
                error_semantic(msg, &Ident);
                break;
            case -2:
                sprintf(msg, "Sobran parametros en la llamada a %s", Ident.lexem);
                error_semantic(msg, &Ident);
                break;
            default:
                error_semantic("Error en los parametros.", &Ident);
            }
        }

        ST_Register(Ident.lexem, Id, NULL, ParamList);
        // Creamos el comando de creacion del objeto
        CommandListBuilder_InsertNew(Ident.lexem);
        match(CLOSE_PAREN, NULL);
	match(SEMICOLON, NULL);

	if(PosValid)
	{
	    // Introducimos los comandos resumidos en el SET
	    // El Start(Depth)
	    CommandListBuilder_SetPos(PosStart.Order, PosStart.Row);
            CommandListBuilder_InsertStart(Ident.lexem, Depth);
	    // El Stop
	    CommandListBuilder_SetPos(PosStop.Order, PosStop.Row);
            CommandListBuilder_InsertStop(Ident.lexem);
	    CommandListBuilder_SetPos(PosStart.Order, PosStart.Row);

            PosValid = 0;
	}
    }
    else if(is_lookahead(TOKEN_IDENTIFIER))
    {
        token Ident;
        
        DEBUG_msg("Entrando en identificador");
        match(TOKEN_IDENTIFIER, &Ident);
        DEBUG_msg_str("El identificador es ", Ident.lexem);
        CommandIdentifierEnd(Ident);
    }
    else if(is_lookahead(FULLSTOP))
    {
	// FIXME: check that PosValid is TRUE
        token Ident;
        Ident.lexem = InstanceName;
        CommandIdentifierEnd(Ident);
    }
    else if(is_lookahead(UNTIL))
    {
        TPos pos_end, frequency;
        TList* old_command_list, *command_list;
        TList* new_list;
        
        match(UNTIL, NULL);
        pos_end   = TimingPoint();
        frequency = CommandUntilEvery();
        match(DO, NULL);
        inside_until_statement = 1;
        new_list = List_New(Pos_Compare);
        DEBUG_msg_hex("New list is ", (unsigned)new_list);
        old_command_list = CommandList_SetCommandList(new_list);
        CommandSequence();
        command_list = CommandList_SetCommandList(old_command_list);
        DEBUG_msg_hex("New list is (2) ", (unsigned)command_list);
        inside_until_statement = 0;
        match(END, NULL);
        CommandListBuilder_InsertRepetitiveCommands(CommandListBuilder_GetPos(), pos_end, frequency, command_list);
    }

}
//-------------------------------------------------------------------
void CommandIdentifierEnd(token Ident)
{
    DEBUG_msg("CommandIdentifierEnd");
    if(is_lookahead(FULLSTOP))
    {
        token Method;
        TTypeId Id;
        TArgList* ParamList;
        int isDelete;
        int error;

        if(!ST_Exists(Ident.lexem))
        {
            char temp[400];
            sprintf(temp, "El identificador/objeto '%s' no existe.", Ident.lexem);
            error_sintax(temp, &Ident);
            error_sintax("(Es posible que sea un plugin que no se ha encontrado)", &Ident);
            // Salimos porque el ST_Type de abajo sino levantaria
            // una excepcion
            // En realidad deberiamos hacer antes del return algo asi como
            // match all until sentence end ";"
            return;
        }
        DEBUG_msg_str("Identificador es ", Ident.lexem);
        DEBUG_msg("match fullstop...");
        match(FULLSTOP, NULL);
        DEBUG_msg("match indent...");

        match(TOKEN_IDENTIFIER, &Method);

        Id = ST_Type(Ident.lexem);
        if(!Type_HasMethod(Id, Method.lexem))
            error_semantic("La clase del objeto no tiene el metodo especificado.", &Method);

        match(OPEN_PAREN, NULL);

        ParamList = ArgList();
        error = Type_CheckParams(Id, Method.lexem, ParamList);
        if(0 != error)
        {
            switch(error)
            {
                char msg[200];
            case -1:
                sprintf(msg, "Faltan parametros en la llamada a %s", Method.lexem);
                error_semantic(msg, &Method);
                break;
            case -2:
                sprintf(msg, "Sobran parametros en la llamada a %s", Method.lexem);
                error_semantic(msg, &Method);
                break;
            default:
                error_semantic("Error en los parametros.", &Method);
            }
        }

        match(CLOSE_PAREN, NULL);

        if(!error)
        {
            isDelete = !strcmp(Method.lexem, "Delete");

            CommandListBuilder_InsertCall(Ident.lexem, Method.lexem, ParamList, isDelete);

            // Manejamos funciones especiales de los plugins
            if(0 == stricmp(Method.lexem, "Start"))
            {
                char* Identifier;
                long  Depth;
                TExprResult* R;

                Identifier = strdup(Ident.lexem);
                R = Expr_Solve((TExpr*)List_First(ParamList));
                if(R->Type == Type_IntegerId)
                {
                    Depth = R->ValueInteger;
                    CommandListBuilder_InsertStart(Identifier, Depth);
                }
                else
                {
                    error_semantic("El primer parametro del Start debe ser una expresion con resultado entero.", &Method);
//                    printf("El tipo es %s\n", R->Type->Name);
                }
            }
            else if(0 == stricmp(Method.lexem, "Stop"))
            {
                CommandListBuilder_InsertStop(strdup(Ident.lexem));
            }
        }
        match(SEMICOLON, NULL);
    }
    else if(is_lookahead(EQUAL))
    {
        TExpr* e;
        token tokEqual;
        
        match(EQUAL, &tokEqual);
        e = Expression();
        if(!ST_Exists(Ident.lexem))
        {
            // Crear variable con el tipo de la expresion
            ST_Register(Ident.lexem, e->Type, NULL, NULL);
        }
        
        if(e->Type != ST_Type(Ident.lexem))
        {
            char ctemp[200];
            // cast automatico de integer <-> float
            if(ST_Type(Ident.lexem) == Type_IntegerId && e->Type == Type_RealId)
	    {
		e = Expr_NewCastInteger(e);
                sprintf(ctemp, "El resultado de la expresion que se asigna no es un entero.");
                warning_semantic(&tokEqual, ctemp);
	    }
	    else if(e->Type == Type_IntegerId && ST_Type(Ident.lexem) == Type_RealId)
            {
                e = Expr_NewCastReal(e);
                sprintf(ctemp, "El resultado de la expresion que se asigna no es un numero real.");
                warning_semantic(&tokEqual, ctemp);
	    }
            else
            {
                char msg[200];
                sprintf(msg, "Error en la asignacion: la expresion con resultado del tipo %s debia ser %s\n",
                        e->Type->Name, ST_Type(Ident.lexem)->Name);
                error_semantic_full(msg, tokEqual.row, tokEqual.col);
                match_until(SEMICOLON);
                return;
            }
        }

        CommandListBuilder_InsertAssign(Ident.lexem, e);
        match(SEMICOLON, NULL);
    }
    else
    {
        error_sintax("Se esperaba '.' o '='", &Ident);
        match_until(SEMICOLON);
    }
}
//-------------------------------------------------------------------
TPos CommandUntilEvery(void)
{
    TPos pos;
    pos.Order = 0;
    pos.Row   = 0;

    if(is_lookahead(EVERY))
    {
        match(EVERY, NULL);
        pos = TimingPoint();
    }
    return pos;
}

//-------------------------------------------------------------------
TArgList* ArgList(void)
{
    TArgList* ArgList = List_New(AlwaysLesser);

    if(is_lookahead(TOKEN_IDENTIFIER) || is_lookahead(TOKEN_NUMBER)
       || is_lookahead(STRING) || is_lookahead(TOKEN_OP_TERM)
       || is_lookahead(OPEN_PAREN))
    {
        TExpr* E1;
        E1 = Expression();
        List_Insert(ArgList, E1);
        ArgListEnd(ArgList);
    }
    
    return ArgList;
}
//-------------------------------------------------------------------
void ArgListEnd(TArgList* ArgList)
{
    if(is_lookahead(COMA))
    {
        TExpr* E1;
        match(COMA, NULL);
        E1 = Expression();
        List_Insert(ArgList, E1);
        ArgListEnd(ArgList);
    }
}
//-------------------------------------------------------------------
TExpr* Expression(void)
{
    TExpr* E1;
    E1 = Term();
    return ExpressionEnd(E1);
}
//-------------------------------------------------------------------
TExpr* ExpressionEnd(TExpr* E1)
{
    if(is_lookahead(TOKEN_OP_TERM))
    {
        token tok;
        TExpr* E2;
        
        match(TOKEN_OP_TERM, &tok);

        E2 = Term();
        if(E1->Type != E2->Type)
        {
            char ctmp[200];
            DEBUG_msg_hex("Tipo 1 es ", (unsigned)E1->Type);
            DEBUG_msg_hex("Tipo 2 es ", (unsigned)E2->Type);
	    if(E1->Type == Type_IntegerId && E2->Type == Type_RealId)
	    {
		E1 = Expr_NewCastReal(E1);
                warning_semantic(&tok, "El primer operando no es un numero real");
	    }
	    else if(E2->Type == Type_IntegerId && E1->Type == Type_RealId)
	    {
		E2 = Expr_NewCastReal(E2);
                warning_semantic(&tok, "El segundo operando no es un numero real");
	    }
	    else
	    {
		sprintf(ctmp, "Tipos no coinciden: primer operando es %s pero el segundo es %s\n", E1->Type->Name, E2->Type->Name);
		error_semantic(ctmp, &tok);
	    }
        }

	if(0 == strcmp(tok.lexem, "+"))
            E2 = Expr_NewAdd(E1, E2);
        else if(0 == strcmp(tok.lexem, "-"))
            E2 = Expr_NewSubstract(E1, E2);
        else
        {
            error_semantic("Operacion no reconocida", &tok);
            return NULL;
        }
        return ExpressionEnd(E2);
    }
    return E1;
}
//-------------------------------------------------------------------
TExpr* Term(void)
{
    TExpr* E1;
    E1 = Factor();
    return TermEnd(E1);
}
//-------------------------------------------------------------------
TExpr* TermEnd(TExpr* E1)
{
    if(is_lookahead(TOKEN_OP_FACTOR))
    {
        token tok;
        TExpr* E2;

        match(TOKEN_OP_FACTOR, &tok);

        E2 = Factor();
	if(E1->Type != E2->Type)
	{
	    if(E1->Type == Type_IntegerId && E2->Type == Type_RealId)
	    {
                warning_semantic(&tok, "El primer operando no es un numero real");
		E1 = Expr_NewCastReal(E1);
	    }
	    else if(E2->Type == Type_IntegerId && E1->Type == Type_RealId)
	    {
                warning_semantic(&tok, "El segundo operando no es un numero real");
		E2 = Expr_NewCastReal(E2);
	    }
            else
		error_semantic("El tipo no coincide con el primer operando", &tok);
        }
        if(0 == strcmp(tok.lexem, "*"))
            E2 = Expr_NewMultiply(E1, E2);
        else if(0 == strcmp(tok.lexem, "/"))
            E2 = Expr_NewDivide(E1, E2);
        else if(0 == strcmp(tok.lexem, "%"))
            E2 = Expr_NewModule(E1, E2);
        else
        {
            error_semantic("Operacion no reconocida", &tok);
            return NULL;
        }
        return TermEnd(E2);
    }
    return E1;
}
//-------------------------------------------------------------------
TExpr* Factor(void)
{
    TExpr* E1;

    if(is_lookahead(TOKEN_OP_TERM))
    {
        token tok;
        int sign = 1;
        match(TOKEN_OP_TERM, &tok);

        if(tok.lexem[0] == '-')
            sign = -1;

        E1 = FactorNoSign();
        return Expr_NewNegative(E1);
    }
    else if(is_lookahead(TOKEN_IDENTIFIER) ||
            is_lookahead(TOKEN_NUMBER)     ||
            is_lookahead(STRING)           ||
            is_lookahead(OPEN_PAREN))
    {
        return FactorNoSign();
    }
    else
    {
        error_sintax("Se esperaba un termino (identificador o constante)\n", NULL);
        return NULL;
    }
}
//-------------------------------------------------------------------
TExpr* FactorNoSign(void)
{
    TExpr* E1;
    
    if(is_lookahead(TOKEN_IDENTIFIER))
    {
        token tok;
        match(TOKEN_IDENTIFIER, &tok);
        E1 = IdentifierEnd(&tok);
    }
    else if(is_lookahead(TOKEN_NUMBER))
    {
        token tok;
        match(TOKEN_NUMBER, &tok);
        if(tok.num_type == TYPE_INT)
            E1 = Expr_NewInteger(tok.value);
        else
            E1 = Expr_NewReal(tok.value);
    }
    else if(is_lookahead(STRING))
    {
        token tok;
        match(STRING, &tok);
        E1 = Expr_NewString(tok.lexem);
    }
    else if(is_lookahead(OPEN_PAREN))
    {
        match(OPEN_PAREN, NULL);
        E1 = Expression();
        match(CLOSE_PAREN, NULL);
    }
    else
    {
        error_sintax("Se esperaba un termino (identificador o constante)\n", NULL);
        return NULL;
    }
    return E1;
}
//-------------------------------------------------------------------
TExpr* IdentifierEnd(token *IdentToken)
{
    TExpr* E1;
    
    if(is_lookahead(OPEN_PAREN))
    {
        TArgList* Args;
        TFunctionId Id;
        int error;
        
        match(OPEN_PAREN, NULL);
        Args = ArgList();
        match(CLOSE_PAREN, NULL);
        Id = FunctionTable_GetId(IdentToken->lexem);
        if(NULL == Id)
            error_semantic("La funcion no se ha definido", IdentToken);

        error = Function_CheckParams(Id, Args);
        if(0 != error)
        {
            switch(error)
            {
                char msg[200];
            case -1:
                sprintf(msg, "Faltan parametros en la llamada a %s", IdentToken->lexem);
                error_semantic(msg, IdentToken);
                break;
            case -2:
                sprintf(msg, "Sobran parametros en la llamada a %s", IdentToken->lexem);                
		DEBUG_msg_dec("Numero de parametros: ",List_Length(Args));
                error_semantic(msg, IdentToken);
                break;
            default:
                error_semantic("Error en los parametros.", IdentToken);
            }
        }
        E1 = Expr_NewFunctionCall(IdentToken->lexem, Args);
        DEBUG_msg_hex("Function return type is ", (unsigned)E1->Type);
    }
    else if(is_lookahead(FULLSTOP))
    {
        TArgList* Args;
        token TokenMethod;
        TTypeId Id;
        int error;
        
        match(FULLSTOP, NULL);
        match(TOKEN_IDENTIFIER, &TokenMethod);
        match(OPEN_PAREN, NULL);
        Args = ArgList();
        match(CLOSE_PAREN, NULL);

        if(!ST_Exists(IdentToken->lexem))
        {
            error_semantic("El objeto no se ha declarado.\n", IdentToken);
            return NULL;
        }
        Id = ST_Type(IdentToken->lexem);
        if(!Type_HasMethod(Id, TokenMethod.lexem))
            error_semantic("La clase del objeto no tiene el metodo especificado.", &TokenMethod);

        error = Type_CheckParams(Id, TokenMethod.lexem, Args);
        if(0 != error)
        {
            switch(error)
            {
                char msg[200];
            case -1:
                sprintf(msg, "Faltan parametros en la llamada a %s", TokenMethod.lexem);
                error_semantic(msg, &TokenMethod);
                break;
            case -2:
                sprintf(msg, "Sobran parametros en la llamada a %s", TokenMethod.lexem);
                error_semantic(msg, &TokenMethod);
                break;
            default:
                error_semantic("Error en los parametros.", &TokenMethod);
            }
        }
        
        E1 = Expr_NewMethodCall(IdentToken->lexem, TokenMethod.lexem, Args);
    }
    else
    {
        if(!ST_Exists(IdentToken->lexem))
        {
            error_semantic("El identificador no se ha declarado.\n", IdentToken);
            return NULL;
        }
        else
            E1 = Expr_NewVariable(IdentToken->lexem);
    }
    return E1;
}

//-------------------------------------------------------------------
DWORD Integer(void)
{
    token tok;
    match(TOKEN_NUMBER, &tok);
    if(tok.num_type != TYPE_INT)
    {
        error_sintax("se esperaba un entero", &tok);
        return 0;
    }
    return (DWORD)tok.value;
}
//-------------------------------------------------------------------
float Real(void)
{
    token tok;
    match(TOKEN_NUMBER, &tok);
    if(tok.num_type != TYPE_REAL)
    {
        error_sintax("se esperaba un real", &tok);
        return 0;
    }
    return (float)tok.value;
}
//-------------------------------------------------------------------
TPos TimingPoint(void)
{
    int order, row;
    TPos pos;
    match(OPEN_PAREN, NULL);
    if(grammar_timing_por_tiempo)
    {
        float t;
        int negativo = 0;
        
        if(is_lookahead(TOKEN_OP_TERM) && lookahead.lexem[0] == '-')
        {
            match(TOKEN_OP_TERM, NULL);
            negativo = 1;
        }
        t = Real();
        order=0;
        row  =(int)(t*1000);
        if(negativo)
            row = -row;
    }
    else
    {
        order = Integer();
        match(COMA, NULL);
        row = Integer();
    }
    match(CLOSE_PAREN, NULL);
    pos.Order = order;
    pos.Row   = row;
    return pos;
}


