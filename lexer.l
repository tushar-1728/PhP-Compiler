%{
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "header.h"
#include "y.tab.h"

int id_install(char *type, char *name, int yylineno, int scope);
void key_install(char *name, int yylineno);
void multicomment();
int temp;
%}

%option yylineno

%%

"<?php" {
    fprintf(stdout, "%s", yytext);
    return START;
}

"?>" {
    fprintf(stdout, "%s", yytext);
    return END;
}

"$"[a-zA-Z_]+[0-9a-zA-Z_]*   { // regex to match identifier
    fprintf(stdout, "%s", yytext);
    yylval.sIndex = id_install("int\0", yytext, yylineno, scope); // create symbol table entry
    return VARIABLE; // return token
}

0   {
    fprintf(stdout, "%s", yytext);
    yylval.iValue = atoi(yytext);
    return INTEGER; // int const token return hua
}

[1-9][0-9]* { // regex to match int const
    fprintf(stdout, "%s", yytext);
    yylval.iValue = atoi(yytext);
    return INTEGER; // int const token return hua
}

[-<>=+*/;.] {
    fprintf(stdout, "%s", yytext);
    return *yytext;
}

"(" {
    ++brack_count;
    fprintf(stdout, "%s", yytext);
    return *yytext;
}

")" {
    --brack_count;
    fprintf(stdout, "%s", yytext);
    return *yytext;
}

"{" {
    ++scope;
    ++paren_count;
    fprintf(stdout, "%s", yytext);
    return *yytext;
}

"}" {
    --scope;
    --paren_count;
    fprintf(stdout, "%s", yytext);
    return *yytext;
}

[\t]*"//".* {;}
"/*" {
    multicomment(); // function to detect multiline comment
}

">="            { /* tokens for keywors and operators */
    fprintf(stdout, "%s", yytext);
    return GE;
}
"<="            {
    fprintf(stdout, "%s", yytext);
    return LE;
}
"=="            {
    fprintf(stdout, "%s", yytext);
    return EQ;
}
"!="            {
    fprintf(stdout, "%s", yytext);
    return NE;
}
"while"         {
    fprintf(stdout, "%s", yytext);
    key_install(yytext, yylineno);
    return WHILE;
}
"if"            {
    fprintf(stdout, "%s", yytext);
    key_install(yytext, yylineno);
    return IF;
}
"else"          {
    fprintf(stdout, "%s", yytext);
    key_install(yytext, yylineno);
    return ELSE;
}
"print"         {
    fprintf(stdout, "%s", yytext);
    key_install(yytext, yylineno);
    return PRINT;
}

[ \t\n]+        {
    fprintf(stdout, "%s", yytext);
};       /* ignore whitespace */

.               yyerror("unknown character");
%%
int yywrap(void) {
    return 1;
}

/* Function to insert entries for identifiers in the symbol table. */
int id_install(char *type, char *name, int yylineno, int scope){
    for(int i = 0; i < sym_size; ++i){
        if(strcmp(sym_table[i].name, name) == 0)
            return i;
    }
    strcpy(sym_table[sym_size].type, type);
    strcpy(sym_table[sym_size].name, name);
    sym_table[sym_size].value = 0;
    sym_table[sym_size].line_no = yylineno;
    sym_table[sym_size].scope = scope;
    sym_table[sym_size].storage_req = sizeof(int);
    ++sym_size;
    return sym_size-1;
}


void multicomment()
{
	char c, c1, c2;
	while ((c = input()) != '*' && c != 0);
	c1=input();
	if(c=='*' && c1=='/')
	{
		c=0;
	}
	if (c != 0)
		putchar(c1);
}

/* Function to insert entries of keyword in the symbol table. */
void key_install(char *name, int yylineno){
    strcpy(key_table[key_size].name, name);
    key_table[key_size].line_no = yylineno;
    ++key_size;
}