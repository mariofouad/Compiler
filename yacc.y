/* Definitions */
%{
    #include <stdio.h>
    int yylex(void);
    void yyerror(char *);
%}

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