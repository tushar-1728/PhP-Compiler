#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "header.h"
#include "y.tab.h"

void push(head_t *head1, char *value);
char* pop(head_t *head1);

static int lbl;
extern FILE *fp_temp1;
char temp[256];
extern head_t *head1;
int imt_var_count0;

int unopti_icg(nodeType *p) {
    int lbl1, lbl2;

    if (!p) return 0;
    switch(p->type) {
    case typeCon:
		sprintf(temp, "%d", p->con.ivalue);
		push(head1, temp);
        break;
    case typeId:
		push(head1, sym_table[p->id.i].name);
        break;
    case typeOpr:
        switch(p->opr.oper) {
        case WHILE:
            fprintf(fp_temp1, "\tLabel\tNULL\tNULL\tL%03d\n", lbl1 = lbl++);
            unopti_icg(p->opr.op[0]);
            fprintf(fp_temp1, "\tifFalse\t%s\tNULL\tL%03d\n", pop(head1), lbl2 = lbl);
			++lbl;
            unopti_icg(p->opr.op[1]);
            fprintf(fp_temp1, "\tLabel\tNULL\tNULL\tL%03d\n", lbl2);
            break;
        case IF:
            unopti_icg(p->opr.op[0]);
            if (p->opr.nops > 2) {
                /* if else */
                fprintf(fp_temp1, "\tifFalse\t%s\tNULL\tL%03d\n", pop(head1), lbl1 = lbl);
				++lbl;
                unopti_icg(p->opr.op[1]);
                fprintf(fp_temp1, "\tgoto\tNULL\tNULL\tL%03d\n", lbl2 = lbl);
				++lbl;
                fprintf(fp_temp1, "\tLabel\tNULL\tNULL\tL%03d\n", lbl1);
                unopti_icg(p->opr.op[2]);
                fprintf(fp_temp1, "\tLabel\tNULL\tNULL\tL%03d\n", lbl2);
            } else {
                /* if */
                fprintf(fp_temp1, "\tifFalse\t%s\t\tL%03d\n", pop(head1), lbl1 = lbl);
				++lbl;
                unopti_icg(p->opr.op[1]);
                fprintf(fp_temp1, "\tLabel\tNULL\tNULL\tL%03d\n", lbl1);
            }
            break;
        case PRINT:     
            unopti_icg(p->opr.op[0]);
			fprintf(fp_temp1, "\tprint\t%s\tNULL\tNULL\n", pop(head1));
            break;
        case '=':       
            unopti_icg(p->opr.op[1]);
            fprintf(fp_temp1, "\t=\t%s\tNULL\t%s\n", pop(head1), sym_table[p->opr.op[0]->id.i].name);
			break;
        case UMINUS:    
            unopti_icg(p->opr.op[0]);
			sprintf(temp, "%c%d", 't', imt_var_count0);
			fprintf(fp_temp1, "\t-\t%s\tNULL\t%s\n", pop(head1), temp);
			push(head1, temp);
			++imt_var_count0;
            break;
        default:
            unopti_icg(p->opr.op[0]);
            unopti_icg(p->opr.op[1]);
			sprintf(temp, "%c%d", 't', imt_var_count0);
            switch(p->opr.oper) {
            case '+':
			   	fprintf(fp_temp1, "\t+\t%s\t%s\t%s\n", pop(head1), pop(head1), temp);
			   	push(head1, temp);
			   	++imt_var_count0;
			   	break;
            case '-':
				fprintf(fp_temp1, "\t-\t%s\t%s\t%s\n", pop(head1), pop(head1), temp);
			   	push(head1, temp);
			   	++imt_var_count0;
				break; 
            case '*':
				fprintf(fp_temp1, "\t*\t%s\t%s\t%s\n", pop(head1), pop(head1), temp);
				push(head1, temp);
				++imt_var_count0;
				break;
            case '/':
				fprintf(fp_temp1, "\t/\t%s\t%s\t%s\n", pop(head1), pop(head1), temp);
			   	push(head1, temp);
			   	++imt_var_count0;
				break;
            case '<':
				fprintf(fp_temp1, "\t<\t%s\t%s\t%s\n", pop(head1), pop(head1), temp);
			   	push(head1, temp);
			   	++imt_var_count0;
				break;
            case '>':
				fprintf(fp_temp1, "\t>\t%s\t%s\t%s\n", pop(head1), pop(head1), temp);
			   	push(head1, temp);
			   	++imt_var_count0;
				break;
            case GE:
				fprintf(fp_temp1, "\t>=\t%s\t%s\t%s\n", pop(head1), pop(head1), temp);
			   	push(head1, temp);
			   	++imt_var_count0;
				break;
            case LE:
				fprintf(fp_temp1, "\t<=\t%s\t%s\t%s\n", pop(head1), pop(head1), temp);
			   	push(head1, temp);
			   	++imt_var_count0;
				break;
            case NE:
				fprintf(fp_temp1, "\t!=\t%s\t%s\t%s\n", pop(head1), pop(head1), temp);
			   	push(head1, temp);
			   	++imt_var_count0;
				break;
            case EQ:
				fprintf(fp_temp1, "\t==\t%s\t%s\t%s\n", pop(head1), pop(head1), temp);
			   	push(head1, temp);
			   	++imt_var_count0;
				break;
            }
        }
    }
    return 0;
}
