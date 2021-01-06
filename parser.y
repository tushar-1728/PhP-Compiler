%{
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "header.h"
#define YYERROR_VERBOSE

/* prototypes */

extern int yylineno;
extern int imt_var_count0;
extern int imt_var_count1;
head_t *head1;
head_t *head2;
FILE *fp_temp1;
FILE *fp_temp2;
FILE *fp_temp3;
FILE *fp_temp4;
int temprorary = 0;
int error_flag;
int scope;
int brack_count;                    /*keeps count of brackets ()*/
int paren_count;                    /*keeps count of parenthesis {}*/
int sym_size;                       /* denotes no of entry in sym table */
int key_size;                       /*denotes no of entry in key table*/
keyword key_table[100];             /*keyword table*/
identifier sym_table[100];          /*symbol table*/

%}

%union {
    int iValue;                         /* integer value */
    int sIndex;                         /* symbol table index */
    nodeType *nPtr;                     /* node pointer */
};

%token <iValue> INTEGER
%token <sIndex> VARIABLE
%token WHILE IF PRINT START END
%nonassoc IFX
%nonassoc ELSE

%left GE LE EQ NE '>' '<'
%left '+' '-'
%left '*' '/'
%nonassoc UMINUS

%type <nPtr> stmt expr stmt_list

%%

program:
        START function END               { ; }
        ;

function:
          function stmt                     { eval($2); d_graph($2); unopti_icg($2); temprorary = opti_icg1($2); assembly($2); freeNode($2); }
        | /* NULL */
        ;

stmt:
        ';'                                 { $$ = opr(';', 2, NULL, NULL); }
        | expr ';'                          { $$ = $1; }
        | PRINT expr ';'                    { $$ = opr(PRINT, 1, $2); }
        | VARIABLE '=' expr ';'             { $$ = opr('=', 2, id($1), $3); }
        | WHILE '(' expr ')' stmt           { $$ = opr(WHILE, 2, $3, $5); }
        | IF '(' expr ')' stmt %prec IFX    { $$ = opr(IF, 2, $3, $5); }
        | IF '(' expr ')' stmt ELSE stmt    { $$ = opr(IF, 3, $3, $5, $7); }
        | '{' stmt_list '}'                 { $$ = $2; }
        ;

stmt_list:
          stmt                              { $$ = $1; }
        | stmt_list stmt                    { $$ = opr(';', 2, $1, $2); }
        ;

expr:
          INTEGER                           { $$ = con($1);}
        | VARIABLE                          { $$ = id($1); }
        | '-' expr %prec UMINUS             { $$ = opr(UMINUS, 1, $2); }
        | expr '+' expr                     { $$ = opr('+', 2, $1, $3); }
        | expr '-' expr                     { $$ = opr('-', 2, $1, $3); }
        | expr '*' expr                     { $$ = opr('*', 2, $1, $3); }
        | expr '/' expr                     { $$ = opr('/', 2, $1, $3); }
        | expr '<' expr                     { $$ = opr('<', 2, $1, $3); }
        | expr '>' expr                     { $$ = opr('>', 2, $1, $3); }
        | expr GE expr                      { $$ = opr(GE, 2, $1, $3); }
        | expr LE expr                      { $$ = opr(LE, 2, $1, $3); }
        | expr NE expr                      { $$ = opr(NE, 2, $1, $3); }
        | expr EQ expr                      { $$ = opr(EQ, 2, $1, $3); }
        | '(' expr ')'                      { $$ = $2; }
        ;

%%

nodeType *con(int value) {
    nodeType *p;

    /* allocate node */
    if ((p = malloc(sizeof(nodeType))) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeCon;
    p->con.ivalue = value;

    return p;
}

nodeType *id(int i) {
    nodeType *p;

    /* allocate node */
    if ((p = malloc(sizeof(nodeType))) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeId;
    p->id.i = i;

    return p;
}

nodeType *opr(int oper, int nops, ...) {
    va_list ap;
    nodeType *p;
    int i;

    /* allocate node, extending op array */
    if ((p = malloc(sizeof(nodeType) + (nops-1) * sizeof(nodeType *))) == NULL)
        yyerror("out of memory");

    /* copy information */
    p->type = typeOpr;
    p->opr.oper = oper;
    p->opr.nops = nops;
    va_start(ap, nops);
    for (i = 0; i < nops; i++)
        p->opr.op[i] = va_arg(ap, nodeType*);
    va_end(ap);
    return p;
}

void freeNode(nodeType *p) {
    int i;

    if (!p) return;
    if (p->type == typeOpr) {
        for (i = 0; i < p->opr.nops; i++)
            freeNode(p->opr.op[i]);
    }
    free (p);
}

int yyerror(const char *s) {
    // fprintf(stdout, "apple\n");
    if(error_flag == 0){
        error_flag = 1;
        printf("\n-----------------------------------------------------------------------------------------------------\n\n");
        if(paren_count != 0) {
            fprintf(stdout, "parenthesis missing.\n\n");
        }
        else if(brack_count != 0) {
            fprintf(stdout, "bracket missing.\n\n");
        }
        else{
            fprintf(stdout, "\n%s at line number %d.\n\n", s, yylineno);
        }
    }
}

void print_sym_table(){
    if(sym_size > 0){
        fprintf(stdout, "--------------------------------------------Symbol Table---------------------------------------------\n");
        fprintf(stdout, "%16s %16s %16s %16s %16s %16s\n", "type", "name", "value", "line no", "scope", "storage_req");
    }
    for(int i = 0; i < sym_size; ++i){
        fprintf(stdout, "%16s %16s %16d %16d %16d %16zu\n", sym_table[i].type, sym_table[i].name, sym_table[i].value, sym_table[i].line_no, sym_table[i].scope, sym_table[i].storage_req);
    }
    printf("\n");
}

void print_key_table(){
    if(key_size > 0){
        fprintf(stdout, "--------------------------------------------Keyword Table--------------------------------------------\n");
        fprintf(stdout, "%16s %16s\n","name", "line no");
    }
    for(int i = 0; i < key_size; ++i){
            fprintf(stdout, "%16s %16d\n", key_table[i].name, key_table[i].line_no);
    }
    printf("\n");
}

void print_ast(){
    FILE *fp = fopen("ast.txt", "r");
    printf("-----------------------------------------Abstract Syntax Tree-----------------------------------------");
    char s = fgetc(fp);
    while(s != EOF){
        printf("%c", s);
        s = fgetc(fp);
    }
    printf("\n");
    fclose(fp);
    remove("ast.txt");
}

void print_imt_code(){
    fclose(fp_temp1);
    FILE *fp = fopen("unopti_icg.txt", "r");
    printf("------------------------------------------Intermediate Code-------------------------------------------\n\n");
    char s = fgetc(fp);
    while(s != EOF){
        printf("%c", s);
        s = fgetc(fp);
    }
    fclose(fp);
    printf("\n");
    printf("\t----------------------------------------\n");
    printf("\t| No. of temprorary variables used: %d. |\n", imt_var_count0);
    printf("\t----------------------------------------\n");
    remove("unopti_icg.txt");
}

void print_opti_imt_code(){
    fclose(fp_temp2);
    fclose(fp_temp3);
    FILE *fp = fopen("opti_icg1.txt", "r");
    printf("-------------------------------------Optimized Intermediate Code--------------------------------------\n\n");
    printf("------------------------------------------------Pass 1------------------------------------------------\n");
    printf("---------------------------------Reduction in number of Live registers--------------------------------\n\n");
    char s = fgetc(fp);
    while(s != EOF){
        printf("%c", s);
        s = fgetc(fp);
    }
    fclose(fp);
    printf("\n");
    printf("\t----------------------------------------\n");
    printf("\t| No. of temprorary variables used: %d. |\n", temprorary);
    printf("\t----------------------------------------\n\n");
    remove("opti_icg1.txt");
}

int eval(nodeType *p) {
    if (!p) return 0;
    switch(p->type) {
    case typeCon:       return p->con.ivalue;
    case typeId:        if(sym_table[p->id.i].scope <= scope)
                            return sym_table[p->id.i].value;
                        else {
                            yyerror("uninitialized variable");
                            return 0;
                        }
    case typeOpr:
        switch(p->opr.oper) {
        case WHILE:     while(eval(p->opr.op[0])) eval(p->opr.op[1]); return 0;
        case IF:        if (eval(p->opr.op[0]))
                            eval(p->opr.op[1]);
                        else if (p->opr.nops > 2)
                            eval(p->opr.op[2]);
                        return 0;
        case PRINT:     /*printf("%d\n", eval(p->opr.op[0]));*/ return eval(p->opr.op[0]);
        case ';':       eval(p->opr.op[0]); return eval(p->opr.op[1]);
        case '=':       return sym_table[p->opr.op[0]->id.i].value = eval(p->opr.op[1]);
        case UMINUS:    return -eval(p->opr.op[0]);
        case '+':       return eval(p->opr.op[0]) + eval(p->opr.op[1]);
        case '-':       return eval(p->opr.op[0]) - eval(p->opr.op[1]);
        case '*':       return eval(p->opr.op[0]) * eval(p->opr.op[1]);
        case '/':       return eval(p->opr.op[0]) / eval(p->opr.op[1]);
        case '<':       return eval(p->opr.op[0]) < eval(p->opr.op[1]);
        case '>':       return eval(p->opr.op[0]) > eval(p->opr.op[1]);
        case GE:        return eval(p->opr.op[0]) >= eval(p->opr.op[1]);
        case LE:        return eval(p->opr.op[0]) <= eval(p->opr.op[1]);
        case NE:        return eval(p->opr.op[0]) != eval(p->opr.op[1]);
        case EQ:        return eval(p->opr.op[0]) == eval(p->opr.op[1]);
        }
    }
    return 0;
}

head_t* init(){
	head_t *head = (head_t*)malloc(sizeof(head_t));
	head->node = NULL;
	head->size = 0;
	return head;
}

void push(head_t *head, char *value){
	node_t *node = (node_t*)malloc(sizeof(node_t));
	strcpy(node->value, value);
	node->next = head->node;
	head->node = node;
	++(head->size);
}

char* pop(head_t *head){
	char *str = (char*)malloc(sizeof(char) * 256);
	node_t *temp = head->node;
	head->node = head->node->next;
	--(head->size);
	strcpy(str, temp->value);
	free(temp);
	return str;
}

int main(void) {
    fp_temp1 = fopen("unopti_icg.txt", "w");
    fp_temp2 = fopen("opti_icg1.txt", "w");
    fp_temp3 = fopen("opti_icg2.txt", "w");
    fp_temp4 = fopen("assembly.txt", "w");
    head1 = init();
    head2 = init();
    printf("\nCode after stripping out comments:\n");
    printf("\n-----------------------------------------------------------------------------------------------------\n");
    yyparse();
    printf("\n-----------------------------------------------------------------------------------------------------\n");
    if(error_flag) {
        printf("\nParsing failed.\n\n");
        // exit(0);
    }
    else {
        printf("\nParsing successful.\n\n");
    }
    print_sym_table();
    print_key_table();
    print_ast();
    print_imt_code();
    print_opti_imt_code();
    fclose(fp_temp4);
    free(head1);
    free(head2);
    return 0;
}
