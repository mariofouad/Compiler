%{
    #include <stdio.h>
    #include <stdlib.h>
    FILE *yyin;
    int yylex(void);
    void yyerror(char *);
%}

%union{
    int i;
    float f;
    char c;
    char* id;
}

%token <i> INT      // FOR 'int' KEYWORD (type)
%token <f> FLOAT    //for 'float' keyword (type)
%token <id> ID      // for variable names (identifiiers)
%token <id> STRING  //for 'string' keyword (type)
%token <i> INT_LITERAL //for integer numbers
%token <f> FLOAT_LITERAL    //for float numbers
%token <c> CHAR_LITERAL     //for single character => NOTE THAT WE CAN ALSO USE i FOR CHARS
%token <id> STRING_LITERAL  // for string literals
%token <i> BOOL_LITERAL     // for true/false

%token IF THEN ELSE WHILE DO RETURN
%token ASSIGN EQ NEQ LE GE LT GT
%token PLUS MINUS MUL DIV
%token LPAREN RPAREN LBRACE RBRACE SEMI COMMA
%token SWITCH CASE DEFAULT COLON FOR
%token CHAR CONST STR OR AND
%token DOUBLE BOOL SHORT LONG UNSIGNED SIGNED

%right EQ
%left COMMA

// start symbol of the grammar
%start program


%%  

program:    program declaration
            | program function_list
            | /*empty*/
            ;

/* declaration : type ID SEMI
            | type expression SEMI
            | declaration
            | const_decl
            ; */

declaration: type declaration
            | ID declaration
            | COMMA declaration
            | expression declaration
            | SEMI
            | const_decl
            ;

const_decl  : CONST type const_decl
            | expression const_decl
            | COMMA const_decl
            | SEMI
            ;

primary_expression: ID
            | STRING_LITERAL
            | CHAR_LITERAL
            | FLOAT_LITERAL
            | INT_LITERAL
            | BOOL_LITERAL
            | LPAREN expression RPAREN
            ;
            
logical_or_expression: logical_and_expression
                     | logical_or_expression OR logical_and_expression
                     ;

logical_and_expression : comparison_expression
                       | logical_and_expression AND comparison_expression
                       ;

comparison_expression : mathematical_expression
            | comparison_expression EQ mathematical_expression   // ==
            | comparison_expression NEQ mathematical_expression    // !=
            | comparison_expression LT mathematical_expression    // <
            | comparison_expression GT mathematical_expression    // >
            | comparison_expression LE mathematical_expression    // <=
            | comparison_expression GE mathematical_expression    // >=
            ;

mathematical_expression: term
            | mathematical_expression PLUS term
            | mathematical_expression MINUS term
            ;

term:       term MUL factor
            | term DIV factor
            | factor
            ;
            
factor:     primary_expression
            | LPAREN mathematical_expression RPAREN //could add expression to allow anything in the paren (x==y) +1
            ;

expression: logical_or_expression
            | expression ASSIGN expression
            ;

block         : LBRACE block_items RBRACE
              ;

block_items   : /* empty */
              | block_items declaration
              | block_items statement
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

statement
    : assignment SEMI
    | conditional_statement
    | loops
    | simple_statement
    | block
    ;

assignment    : ID ASSIGN expression
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