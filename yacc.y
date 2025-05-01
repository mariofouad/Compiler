%{
    #include <stdio.h>
    #include <stdlib.h>
    FILE *yyin;
    int yylex(void);
    void yyerror(char *);
%}

%union {
    int i;
    float f;
    char* id;
}

%token <i> INT_LITERAL;
%token <f> FLOAT_LITERAL;
%token <id> ID;

%token IF THEN ELSE WHILE DO INT FLOAT RETURN
%token ASSIGN EQ NEQ LE GE LT GT
%token PLUS MINUS MUL DIV
%token LPAREN RPAREN LBRACE RBRACE SEMI COMMA
%token SWITCH CASE DEFAULT COLON FOR

%token CHAR DOUBLE BOOL STRING LONG SHORT UNSIGNED SIGNED CONST

%%  

program       : function_list
              ;

block         : LBRACE block_items RBRACE
              ;

block_items   : /* empty */
              | block_items declaration
              | block_items statement
              ;

declaration   : type var_list SEMI
              ;

type          : INT
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

var_list      : ID
              | var_list COMMA ID
              ;

statement
    : assignment SEMI
    | conditional_statement
    | loops
    | simple_statement
    | block
    ;

assignment    : ID ASSIGN expression
              ;

expression    : INT_LITERAL
              | FLOAT_LITERAL
              | ID
              | expression PLUS expression
              | expression MINUS expression
              | expression MUL expression
              | expression DIV expression
              ;

statement_list : statement
               | statement_list statement
               ;

conditional_statement : if_statement
                      | switch_statement
                      ;

loops : for_loop 
      | while_loop
      | do_while_loop
      ;

if_statement : matched_if_statement
             | unmatched_if_statement
             ;
    
matched_if_statement : IF LPAREN expression RPAREN block
                     | IF LPAREN expression RPAREN simple_statement ELSE simple_statement
                     | IF LPAREN expression RPAREN block ELSE block
                     ;
    
unmatched_if_statement : IF LPAREN expression RPAREN block
                       | IF LPAREN expression RPAREN simple_statement
                       ;

switch_statement : SWITCH LPAREN expression RPAREN LBRACE switch_case_list RBRACE
                 ;
    
switch_case_list : switch_case
                 | switch_case_list switch_case
                 ;
                
switch_case : CASE constant COLON statement_list
            | DEFAULT COLON statement_list
            ;

for_loop : FOR LPAREN expression SEMI expression SEMI expression RPAREN block
         | FOR LPAREN expression SEMI expression SEMI expression RPAREN simple_statement
         ;
    
while_loop : WHILE LPAREN expression RPAREN block
           | WHILE LPAREN expression RPAREN simple_statement
           ;

do_while_loop : DO block WHILE LPAREN expression RPAREN SEMI
              ;

constant : INT_LITERAL
         | FLOAT_LITERAL
         | ID
         ;

simple_statement: expression SEMI
                ;
                
function_list : /* empty */
              | function_list function_definition
              ;

function_definition
    : type ID LPAREN parameter_list RPAREN block
    ;

parameter_list
    : /* empty */
    | parameter_declaration
    | parameter_list COMMA parameter_declaration
    ;

parameter_declaration
    : type ID
    ;

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
        return 0;
    }

    return 1;
}