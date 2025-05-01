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

/* Production rules */
%%
    statement_list: statement
    | statement_list statement
    ;
    statement: conditional_statement
    | loops
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
    | IF LPAREN expression RPAREN LBRACE statement_list RBRACE ELSE LBRACE statement_list RBRACE
    ;
    unmatched_if_statement: IF LPAREN expression RPAREN LBRACE statement_list RBRACE
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
    ;
    while_loop: WHILE LPAREN expression RPAREN LBRACE statement_list RBRACE
    ;
    do_while_loop: DO LBRACE statement_list RBRACE WHILE LPAREN expression RPAREN SEMI
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