typedef enum { typeCon, typeId, typeOpr } nodeEnum; /*denotes the type of node in abstract sysntax tree*/

/* constants */
typedef struct {                     /*int constant node*/
        int ivalue;                  /* value of int constant */
} conNodeType;

/* identifiers */
typedef struct {                /*identifier node*/
    int i;                      /* index to symbol table array */
} idNodeType;

/* operators */
typedef struct {
    int oper;                   /* operator */
    int nops;                   /* number of operands */
    struct nodeTypeTag *op[1];	/* operands, extended at runtime */
} oprNodeType;

typedef struct nodeTypeTag {
    nodeEnum type;              /* type of node */

    union {
        conNodeType con;        /* constants */
        idNodeType id;          /* identifiers */
        oprNodeType opr;        /* internal node with an operators */
    };
} nodeType;

typedef struct {                /*structure of each entry in symbol table*/
    char type[15];              /*typeof variable*/
    char name[256];             /*variable name*/
    int value;                  /*variable value*/
    int line_no;                /*line of their declaration*/
    int scope;                  /*scope of variable*/
    size_t storage_req;         /*storage required by variable*/
} identifier;

typedef struct {                /*structure of each entry in keyword table*/
    char name[15];              /*keyword name*/
    int line_no;                /*line of its occurence*/
} keyword;

typedef struct node{
	char value[256];
	struct node *next;
}node_t;

typedef struct head{
	node_t *node;
	int size;
}head_t;

extern int sym_size;                       /*denotes no of entry in sym table*/
extern identifier sym_table[100];          /*symbol table*/

extern int key_size;                       /*denotes no of entry in key table*/
extern keyword key_table[100];             /*keyword table*/

extern int scope;

extern int paren_count;
extern int brack_count;

int yyerror(const char *s);
nodeType *opr(int oper, int nops, ...);
nodeType *id(int i);
nodeType *con(int value);
void freeNode(nodeType *p);
void print_sym_table();
void print_key_table();
void print_ast();
void print_imt_code();
int opti_icg1(nodeType *p);
int unopti_icg(nodeType *p);
int eval(nodeType *p);
int d_graph(nodeType *p);
int assembly(nodeType *p);
int yylex(void);