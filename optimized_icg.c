#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "header.h"
#include "y.tab.h"

void push(head_t *head2, char *value);
char* pop(head_t *head2);

static int lbl;
extern FILE *fp_temp2;
extern FILE *fp_temp3;
char temp0[256];
extern head_t *head2;
int imt_var_count1;
int equality_flag;
int temp1_var;
int temp2_var;
char temp1[256];
char temp2[256];
char string1[256];
char string2[256];

int opti_icg1(nodeType *p) {
	int lbl1, lbl2;

	if (!p) return 0;
	switch(p->type) {
	case typeCon:
		sprintf(temp0, "%d", p->con.ivalue);
		push(head2, temp0);
		break;
	case typeId:
		push(head2, sym_table[p->id.i].name);
		break;
	case typeOpr:
		switch(p->opr.oper) {
		case WHILE:
			fprintf(fp_temp2, "\tLabel\tNULL\tNULL\tL%03d\n", lbl1 = lbl);
			fprintf(fp_temp3, "Label NULL NULL L%03d\n", lbl1 = lbl);
			++lbl;
			opti_icg1(p->opr.op[0]);
			strcpy(string1, pop(head2));
			fprintf(fp_temp2, "\tifFalse\t%s\tNULL\tL%03d\n", string1, lbl2 = lbl);
			fprintf(fp_temp3, "ifFalse %s NULL L%03d\n", string1, lbl2 = lbl);
			++lbl;
			opti_icg1(p->opr.op[1]);
			fprintf(fp_temp2, "\tLabel\tNULL\tNULL\tL%03d\n", lbl2);
			fprintf(fp_temp3, "Label NULL NULL L%03d\n", lbl2);
			break;
		case IF:
			opti_icg1(p->opr.op[0]);
			if (p->opr.nops > 2) {
				/* if else */
				strcpy(string1, pop(head2));
				fprintf(fp_temp2, "\tifFalse\t%s\tNULL\tL%03d\n", string1, lbl1 = lbl);
				fprintf(fp_temp3, "ifFalse %s NULL L%03d\n", string1, lbl1 = lbl);
				++lbl;
				opti_icg1(p->opr.op[1]);
				fprintf(fp_temp2, "\tgoto\tNULL\tNULL\tL%03d\n", lbl2 = lbl);
				fprintf(fp_temp3, "goto NULL NULL L%03d\n", lbl2 = lbl);
				++lbl;
				fprintf(fp_temp2, "\tLabel\tNULL\tNULL\tL%03d\n", lbl1);
				fprintf(fp_temp3, "Label NULL NULL L%03d\n", lbl1);
				opti_icg1(p->opr.op[2]);
				fprintf(fp_temp2, "\tLabel\tNULL\tNULL\tL%03d\n", lbl2);
				fprintf(fp_temp3, "Label NULL NULL L%03d\n", lbl2);
			} else {
				/* if */
				strcpy(string1, pop(head2));
				fprintf(fp_temp2, "\tifFalse\t%s\tNULL\tL%03d\n", string1, lbl1 = lbl);
				fprintf(fp_temp3, "ifFalse %s NULL L%03d\n", string1, lbl1 = lbl);
				++lbl;
				opti_icg1(p->opr.op[1]);
				fprintf(fp_temp2, "\tLabel\tNULL\tNULL\tL%03d\n", lbl1);
				fprintf(fp_temp3, "Label NULL NULL L%03d\n", lbl1);
			}
			break;
		case PRINT:     
			opti_icg1(p->opr.op[0]);
			strcpy(string1, pop(head2));
			fprintf(fp_temp2, "\tprint\t%s\tNULL\tNULL\n", string1);
			fprintf(fp_temp3, "print %s NULL NULL\n", string1);
			break;
		case '=':       
			// fprintf(fp_temp2, "%d\n", imt_var_count1);
			opti_icg1(p->opr.op[1]);
			// fprintf(fp_temp2, "%d\n", imt_var_count1);
			strcpy(temp1, pop(head2));
			if(temp1[0] == 't'){
				--imt_var_count1;
				equality_flag = 1;
			}
			fprintf(fp_temp2, "\t=\t%s\tNULL\t%s\n", temp1, sym_table[p->opr.op[0]->id.i].name);
			fprintf(fp_temp3, "= %s NULL %s\n", temp1, sym_table[p->opr.op[0]->id.i].name);
			// fprintf(fp_temp2, "%d\n", imt_var_count1);
			break;
		case UMINUS:    
			opti_icg1(p->opr.op[0]);
			sprintf(temp0, "%c%d", 't', imt_var_count1);
			strcpy(string1, pop(head2));
			fprintf(fp_temp2, "\tUMINUS\t%s\tNULL\t%s\n", string1, temp0);
			fprintf(fp_temp3, "UMINUS %s NULL %s\n", string1, temp0);
			push(head2, temp0);
			++imt_var_count1;
			break;
		default:
			opti_icg1(p->opr.op[0]);
			opti_icg1(p->opr.op[1]);
			sprintf(temp0, "%c%d", 't', imt_var_count1);
			switch(p->opr.oper) {
			case '+':
				strcpy(temp1, pop(head2));
				strcpy(temp2, pop(head2));
				if((temp1_var = (temp1[0] == temp0[0])) || (temp2_var = (temp2[0] == temp0[0]))){
					if(temp0[0] == 't'){
						if(temp1_var){
							strcpy(temp0, temp1);
						}
						if(temp2_var){
							strcpy(temp0, temp2);
						}
					}
				}
				fprintf(fp_temp2, "\t+\t%s\t%s\t%s\n", temp2, temp1, temp0);
				fprintf(fp_temp3, "+ %s %s %s\n", temp2, temp1, temp0);
				push(head2, temp0);
				if(!(temp1_var || temp2_var)){
					++imt_var_count1;
				}
				break;
			case '-':
				strcpy(temp1, pop(head2));
				strcpy(temp2, pop(head2));
				if((temp1_var = (temp1[0] == temp0[0])) || (temp2_var = (temp2[0] == temp0[0]))){
					if(temp0[0] == 't'){
						if(temp1_var){
							strcpy(temp0, temp1);
						}
						if(temp2_var){
							strcpy(temp0, temp2);
						}
						// fprintf(fp_temp2, "apple %d %d\n", temp1_var, temp2_var);
					}
				}
				fprintf(fp_temp2, "\t-\t%s\t%s\t%s\n", temp2, temp1, temp0);
				fprintf(fp_temp3, "- %s %s %s\n", temp2, temp1, temp0);
				push(head2, temp0);
				if(!(temp1_var || temp2_var)){
					++imt_var_count1;
				}
				break; 
			case '*':
				strcpy(temp1, pop(head2));
				strcpy(temp2, pop(head2));
				if((temp1_var = (temp1[0] == temp0[0])) || (temp2_var = (temp2[0] == temp0[0]))){
					if(temp0[0] == 't'){
						if(temp1_var){
							strcpy(temp0, temp1);
						}
						if(temp2_var){
							strcpy(temp0, temp2);
						}
						// fprintf(fp_temp2, "apple %d %d\n", temp1_var, temp2_var);
					}
				}
				fprintf(fp_temp2, "\t*\t%s\t%s\t%s\n", temp2, temp1, temp0);
				fprintf(fp_temp3, "* %s %s %s\n", temp2, temp1, temp0);
				push(head2, temp0);
				if(!(temp1_var || temp2_var)){
					++imt_var_count1;
				}
				break;
			case '/':
				strcpy(temp1, pop(head2));
				strcpy(temp2, pop(head2));
				if((temp1_var = (temp1[0] == temp0[0])) || (temp2_var = (temp2[0] == temp0[0]))){
					if(temp0[0] == 't'){
						if(temp1_var){
							strcpy(temp0, temp1);
						}
						if(temp2_var){
							strcpy(temp0, temp2);
						}
						// fprintf(fp_temp2, "apple %d %d\n", temp1_var, temp2_var);
					}
				}
				fprintf(fp_temp2, "\t/\t%s\t%s\t%s\n", temp2, temp1, temp0);
				fprintf(fp_temp3, "/ %s %s %s\n", temp2, temp1, temp0);
				push(head2, temp0);
				if(!(temp1_var || temp2_var)){
					++imt_var_count1;
				}
				break;
			case '<':
				strcpy(string1, pop(head2));
				strcpy(string2, pop(head2));
				fprintf(fp_temp2, "\t<\t%s\t%s\t%s\n", string2, string1, temp0);
				fprintf(fp_temp3, "< %s %s %s\n", string2, string1, temp0);
				push(head2, temp0);
				++imt_var_count1;
				break;
			case '>':
				strcpy(string1, pop(head2));
				strcpy(string2, pop(head2));
				fprintf(fp_temp2, "\t>\t%s\t%s\t%s\n", string2, string1, temp0);
				fprintf(fp_temp3, "> %s %s %s\n", string2, string1, temp0);
				push(head2, temp0);
				++imt_var_count1;
				break;
			case GE:
				strcpy(string1, pop(head2));
				strcpy(string2, pop(head2));
				fprintf(fp_temp2, "\t>=\t%s\t%s\t%s\n", string2, string1, temp0);
				fprintf(fp_temp3, ">= %s %s %s\n", string2, string1, temp0);
				push(head2, temp0);
				++imt_var_count1;
				break;
			case LE:
				strcpy(string1, pop(head2));
				strcpy(string2, pop(head2));
				fprintf(fp_temp2, "\t<=\t%s\t%s\t%s\n", string2, string1, temp0);
				fprintf(fp_temp3, "<= %s %s %s\n", string2, string1, temp0);
				push(head2, temp0);
				++imt_var_count1;
				break;
			case NE:
				strcpy(string1, pop(head2));
				strcpy(string2, pop(head2));
				fprintf(fp_temp2, "\t!=\t%s\t%s\t%s\n", string2, string1, temp0);
				fprintf(fp_temp3, "!= %s %s %s\n", string2, string1, temp0);
				push(head2, temp0);
				++imt_var_count1;
				break;
			case EQ:
				strcpy(string1, pop(head2));
				strcpy(string2, pop(head2));
				fprintf(fp_temp2, "\t==\t%s\t%s\t%s\n", string2, string1, temp0);
				fprintf(fp_temp3, "== %s %s %s\n", string2, string1, temp0);
				push(head2, temp0);
				++imt_var_count1;
				break;
			}
		}
	}
	return imt_var_count1 + equality_flag;
}
