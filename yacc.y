%{
    #include <stdio.h>
    #include <stdlib.h>
    #define MAX 1000

    // === SYMBOL TABLE ===
typedef struct Symbol {
    char *name;
    char *type;
    int isConstant;
    struct Symbol *next;
} Symbol;

Symbol* symbolTable = NULL;
char* currentType = NULL;

void insertSymbol(char* name, char* type, int isConst) {
    Symbol* sym = (Symbol*)malloc(sizeof(Symbol));
    sym->name = strdup(name);
    sym->type = strdup(type);
    sym->isConstant = isConst;
    sym->next = symbolTable;
    symbolTable = sym;
}

int lookup(char* name) {
    Symbol* sym = symbolTable;
    while (sym != NULL) {
        if (strcmp(sym->name, name) == 0)
            return 1;
        sym = sym->next;
    }
    return 0;
}

// === INTERMEDIATE CODE ===
typedef struct {
    char* op;
    char* arg1;
    char* arg2;
    char* result;
} Quadruple;

Quadruple quads[MAX];
int quadIndex = 0;
int tempCount = 0;
int labelCount = 0;

char* newLabel() {
    char* name = malloc(10);
    sprintf(name, "L%d", labelCount++);
    return name;
}


char* newTemp() {
    char temp[10];
    sprintf(temp, "t%d", tempCount++);
    return strdup(temp);
}

void emit(char* op, char* arg1, char* arg2, char* result) {
    quads[quadIndex].op = strdup(op);
    quads[quadIndex].arg1 = strdup(arg1);
    quads[quadIndex].arg2 = strdup(arg2);
    quads[quadIndex].result = strdup(result);
    quadIndex++;
}

void printQuads() {
    printf("\nGenerated Quadruples:\n");
    for (int i = 0; i < quadCount; i++) {
        printf("%d: (%s, %s, %s, %s)\n", i, quads[i].op, quads[i].arg1, quads[i].arg2, quads[i].result);
    }
}


    // === ERROR HANDLING ===
    extern int yylineno;
    extern char* yytext;
    extern FILE *yyin;
    int yylex(void);
    void yyerror(char* s) {
    fprintf(stderr, "Syntax Error: %s at line %d near '%s'\n", s, yylineno, yytext);
}%}

%union{
    int i;
    float f;
    char c;
    char* id;
}

%token <i> INT
%token <f> FLOAT
%token <id> ID
%token <id> STRING
%token <i> INT_LITERAL
%token <f> FLOAT_LITERAL
%token <c> CHAR_LITERAL
%token <id> STRING_LITERAL
%token <i> BOOL_LITERAL

%token IF ELSE WHILE DO RETURN BREAK MOD NOT VOID CONTINUE
%token ASSIGN EQ NEQ LE GE LT GT
%token PLUS MINUS MUL DIV
%token LPAREN RPAREN LBRACE RBRACE SEMI COMMA
%token SWITCH CASE DEFAULT COLON FOR AND OR
%token CHAR CONST DOUBLE BOOL SHORT LONG UNSIGNED SIGNED

%right ASSIGN
%left COMMA

%start program

%%

program
  : external_list
  ;

external_list
  : /* empty */
  | external_list external
  ;

external
  : function_definition
  | declaration
  ;

declaration
  : type var_list SEMI
  | const_decl
  ;

const_decl : CONST type var_list SEMI ;

var_list: init_declarator
  | var_list COMMA init_declarator
  ;

init_declarator: ID
  | ID ASSIGN expression
  ;

type
  : INT | FLOAT | CHAR | DOUBLE | BOOL | STRING | LONG | SHORT | UNSIGNED | SIGNED
  ;

function_definition
    : type ID LPAREN parameter_list RPAREN block
    | VOID ID LPAREN parameter_list RPAREN block
    ;

argument_list
    : /* empty */
    | argument_sequence
    ;

argument_sequence
    : expression
    | argument_sequence COMMA expression
    ;

parameter_list
    : /* empty */
    | VOID
    | parameter_declaration
    | parameter_list COMMA parameter_declaration
    ;

parameter_declaration
    : type ID
    ;

block : LBRACE block_items RBRACE ;

block_items
  : /* empty */
  | block_items block_item
  ;

block_item
  : declaration
  | statement
  ;

statement
    : expression SEMI
    | conditional_statement
    | loops
    | block
    | CONTINUE SEMI
    | BREAK SEMI
    | RETURN expression SEMI
    | RETURN SEMI
    ;

expression
    : logical_or_expression
    | expression ASSIGN expression
    ;

logical_or_expression
    : logical_and_expression
    | logical_or_expression OR logical_and_expression
    ;

logical_and_expression
    : comparison_expression
    | logical_and_expression AND comparison_expression
    ;

comparison_expression
    : mathematical_expression
    | comparison_expression EQ mathematical_expression
    | comparison_expression NEQ mathematical_expression
    | comparison_expression LT mathematical_expression
    | comparison_expression GT mathematical_expression
    | comparison_expression LE mathematical_expression
    | comparison_expression GE mathematical_expression
    ;

mathematical_expression
    : term
    | mathematical_expression PLUS term
    | mathematical_expression MINUS term
    ;

term
    : term MUL factor
    | term DIV factor
    | term MOD factor
    | factor
    ;

factor
    : primary_expression
    | NOT factor
    | LPAREN expression RPAREN
    ;

primary_expression
    : ID
    | STRING_LITERAL
    | CHAR_LITERAL
    | FLOAT_LITERAL
    | INT_LITERAL
    | BOOL_LITERAL
    | ID LPAREN argument_list RPAREN

    ;

conditional_statement
    : IF LPAREN expression RPAREN block
    | IF LPAREN expression RPAREN block ELSE block
    | switch_statement
    ;

switch_statement : SWITCH LPAREN expression RPAREN LBRACE switch_case_list RBRACE ;

switch_case_list : /* empty */ | switch_case_list switch_case ;

switch_case : CASE constant COLON statement
            | DEFAULT COLON statement
            ;

constant : INT_LITERAL | FLOAT_LITERAL | ID ;

loops
    : for_loop
    | while_loop
    | do_while_loop
    ;

for_loop : FOR LPAREN expression SEMI expression SEMI expression RPAREN block
         | FOR LPAREN INT expression SEMI expression SEMI expression RPAREN block  
         ;

while_loop : WHILE LPAREN expression RPAREN block ;

do_while_loop : DO block WHILE LPAREN expression RPAREN SEMI ;

%%

void yyerror(char *s) {
    fprintf(stderr, "%s\n", s);
    exit(0);
}

int main(int argc, char **argv) {
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            perror("Error opening file");
            return 1;
        }
    }

    if (yyparse() == 0){
        printQuads();
        return 0;
    }

    return 1;
}