%{
    #include <stdio.h>
    #include <stdlib.h>
    extern FILE *yyin;
    int yylex(void);
    void yyerror(char *);
%}

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

%token IF THEN ELSE WHILE DO RETURN BREAK MOD NOT VOID
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
    | RETURN expression SEMI
    | RETURN SEMI
    ;

expression
    : logical_or_expression
    | expression ASSIGN expression
    | declaration
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
    | factor
    ;

factor
    : primary_expression
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

for_loop : FOR LPAREN expression SEMI expression SEMI expression RPAREN statement ;

while_loop : WHILE LPAREN expression RPAREN statement ;

do_while_loop : DO statement WHILE LPAREN expression RPAREN SEMI ;

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