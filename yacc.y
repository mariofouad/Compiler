/* Definitions */
%{
    #include <stdio.h>
    int yylex(void);
    void yyerror(char *);
%}

%union {
    int i;
    float f;
    char* id;
}

%token <i> INT_NUM;
%token <f> FLOAT_NUM;
%token <id> ID;

%token IF THEN ELSE WHILE DO INT FLOAT RETURN
%token ASSIGN EQ NEQ LE GE LT GT
%token PLUS MINUS MUL DIV
%token LPAREN RPAREN LBRACE RBRACE SEMI COMMA
%token SWITCH CASE DEFAULT COLON FOR

%union{
    int i;
    float f;
    char* id;
}

%token <i> INT
%token <f> FLOAT
%token <id> ID

%token INT_LITERAL FLOAT_LITERAL CHAR_LITERAL STRING_LITERAL
%token LBRACE RBRACE ASSIGN SEMI COMMA
%token CHAR DOUBLE BOOL STRING LONG SHORT UNSIGNED SIGNED CONST
%token PLUS MINUS MUL DIV EQ

/* Production rules */
%%  
    
    statement_list: statement
    | statement_list statement
    ;
    statement: conditional_statement
    | loops
    | simple_statement
    ;
    conditional_statement: if_statement
    | switch_statement
    ;
    loops: for_loop 
    | while_loop
    | do_while_loop
    ;
    if_statement: matched_if_statement
    | unmatched_if_statement
    ;
    matched_if_statement: IF LPAREN expression RPAREN LBRACE statement_list RBRACE
    | IF LPAREN expression RPAREN simple_statement ELSE simple_statement
    | IF LPAREN expression RPAREN LBRACE statement_list RBRACE ELSE LBRACE statement_list RBRACE
    ;
    unmatched_if_statement: IF LPAREN expression RPAREN LBRACE statement_list RBRACE
    | IF LPAREN expression RPAREN simple_statement
    ;
    switch_statement: SWITCH LPAREN expression RPAREN LBRACE switch_case_list RBRACE
    ;
    switch_case_list: switch_case
    | switch_case_list switch_case
    ;
    switch_case: CASE constant COLON statement_list
    | DEFAULT COLON statement_list
    ;
    for_loop: FOR LPAREN expression SEMI expression SEMI expression RPAREN LBRACE statement_list RBRACE
    | FOR LPAREN expression SEMI expression SEMI expression RPAREN simple_statement
    ;
    while_loop: WHILE LPAREN expression RPAREN LBRACE statement_list RBRACE
    | WHILE LPAREN expression RPAREN simple_statement
    ;
    do_while_loop: DO LBRACE statement_list RBRACE WHILE LPAREN expression RPAREN SEMI
    ;
    constant: INT_NUM | FLOAT_NUM | ID;
    simple_statement: expression SEMI
    ;
    
type
    : INT
    | FLOAT
    | CHAR
    | DOUBLE
    | BOOL
    | STRING
    | LONG
    | SHORT
    | UNSIGNED
    | SIGNED
    | CONST type
    ;

block : LBRACE block_items RBRACE
      ;

block_items : /* nothing */
            | block_items declaration
            | block_items statement
            ;

declaration : type var_list SEMI
            ;

var_list : ID
         | var_list COMMA ID
         ;

statement : assignment SEMI
          ;

assignment : ID ASSIGN expression

expression
    : INT_LITERAL
    | FLOAT_LITERAL
    | CHAR_LITERAL
    | STRING_LITERAL
    | ID
    | expression PLUS expression
    | expression MINUS expression
    | expression MUL expression
    | expression DIV expression
    | expression EQ expression
    ;

%%


/* Subroutines */
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
        return 0;
    }

    return 1;
}