#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "lex.h"
#define DEBUG_VAR 1
#define DEBUG_MODULE "errors"
#include "../debug.h"
#include "../epopeia_internal.h"

int error_count = 0;
int warning_count = 0;

FILE *output_fp = NULL;
extern int console_output;

void error_set_output_file(const char* output_filename)
{
    //output_fp = fopen(output_filename, "wt");
    if(output_fp == NULL)
        output_fp = stdout;
}

void msg_print(const char* type, int row, int col, const char* msg)
{
    if(output_fp == NULL)
        output_fp = stdout;
    if(error_count + warning_count == 0)
       if(console_output) printf("\n");
/*    if(console_output)*/
        printf("%s (%03d:%03d): %s\n", type, row, col, msg);
}

void error_semantic(char *msg, token *tok)
{
    msg_print("Error semantico", tok->row, tok->col, msg);
    error_count++;
}

void error_semantic_full(char *msg, int row, int col)
{
    msg_print("Error semantico", row, col, msg);
    error_count++;
}

void error_sintax(char *msg, token *tok)
{
    msg_print("Error sintactico", tok->row, tok->col, msg);
    error_count++;
}

void error_runtime(char* msg)
{
    assert(msg != NULL);
    DEBUG_msg_str("EPOPEIA RUNTIME ERROR (AYEEE): ", msg);
    msg_print("EPOPEIA RUNTIME ERROR (AYEEE)", -1, -1, msg);
    exit(1); // por si hay alguna duda 8)
}

int error_get_count(void)
{
	return error_count;
}

void warning_runtime(char* msg)
{
    assert(msg != NULL);
    DEBUG_msg_str("Epopeia runtime warning: ", msg);
    msg_print("Epopeia runtime warning", -1, -1, msg);
}


void warning_semantic(token *tok, const char *msg)
{
    switch(g_Epopeia.WarningLevel)
    {
    case 0:
	break;
    case 1:
	msg_print("Warning", tok->row, tok->col, msg);
	warning_count++;
	break;
    case 2:
    default:
	msg_print("Warning", tok->row, tok->col, msg);
	error_count++;
    }
}

