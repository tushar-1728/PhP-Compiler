#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "header.h"
#include "y.tab.h"

static int lbl;
extern FILE *fp_temp4;

int assembly(nodeType *p) {
    int lbl1, lbl2;

    if (!p) return 0;
    switch(p->type) {
    case typeCon:       
        fprintf(fp_temp4, "\tpush\t%d\n", p->con.ivalue); 
        break;
    case typeId:        
        fprintf(fp_temp4, "\tpush\t%s\n", sym_table[p->id.i].name); 
        break;
    case typeOpr:
        switch(p->opr.oper) {
        case WHILE:
            fprintf(fp_temp4, "L%03d:\n", lbl1 = lbl++);
            assembly(p->opr.op[0]);
            fprintf(fp_temp4, "\tjz\tL%03d\n", lbl2 = lbl++);
            assembly(p->opr.op[1]);
            fprintf(fp_temp4, "\tjmp\tL%03d\n", lbl1);
            fprintf(fp_temp4, "L%03d:\n", lbl2);
            break;
        case IF:
            assembly(p->opr.op[0]);
            if (p->opr.nops > 2) {
                /* if else */
                fprintf(fp_temp4, "\tjz\tL%03d\n", lbl1 = lbl++);
                assembly(p->opr.op[1]);
                fprintf(fp_temp4, "\tjmp\tL%03d\n", lbl2 = lbl++);
                fprintf(fp_temp4, "L%03d:\n", lbl1);
                assembly(p->opr.op[2]);
                fprintf(fp_temp4, "L%03d:\n", lbl2);
            } else {
                /* if */
                fprintf(fp_temp4, "\tjz\tL%03d\n", lbl1 = lbl++);
                assembly(p->opr.op[1]);
                fprintf(fp_temp4, "L%03d:\n", lbl1);
            }
            break;
        case PRINT:     
            assembly(p->opr.op[0]);
            fprintf(fp_temp4, "\tprint\n");
            break;
        case '=':       
            assembly(p->opr.op[1]);
            fprintf(fp_temp4, "\tpop\t%s\n", sym_table[p->opr.op[0]->id.i].name);
            break;
        case UMINUS:    
            assembly(p->opr.op[0]);
            fprintf(fp_temp4, "\tneg\n");
            break;
        default:
            assembly(p->opr.op[0]);
            assembly(p->opr.op[1]);
            switch(p->opr.oper) {
            case '+':   fprintf(fp_temp4, "\tadd\n"); break;
            case '-':   fprintf(fp_temp4, "\tsub\n"); break; 
            case '*':   fprintf(fp_temp4, "\tmul\n"); break;
            case '/':   fprintf(fp_temp4, "\tdiv\n"); break;
            case '<':   fprintf(fp_temp4, "\tcompLT\n"); break;
            case '>':   fprintf(fp_temp4, "\tcompGT\n"); break;
            case GE:    fprintf(fp_temp4, "\tcompGE\n"); break;
            case LE:    fprintf(fp_temp4, "\tcompLE\n"); break;
            case NE:    fprintf(fp_temp4, "\tcompNE\n"); break;
            case EQ:    fprintf(fp_temp4, "\tcompEQ\n"); break;
            }
        }
    }
    return 0;
}