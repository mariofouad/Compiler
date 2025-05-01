/* Definitions */
%{
    #include <stdio.h>
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

%right EQ
%left COMMA

// start symbol of the grammar
%start program

/* Production rules */

%%
program:    program declaration
            | /*empty*/
            ;
declaration : type ID SEMI
            | type ID ASSIGN expression SEMI
            | const_decl
            ;
const_decl       : CONST type ID ASSIGN expression SEMI
            ;
type        : INT 
            | FLOAT
            | CHAR
            | STR
            ;
primary_expression: ID
            | STRING_LITERAL
            | CHAR_LITERAL
            | FLOAT_LITERAL
            | INT_LITERAL
            | BOOL_LITERAL
            | LPAREN expression RPAREN
            ;
logical_or_expression: 
            logical_and_expression
            | logical_or_expression OR logical_and_expression
            ;
logical_and_expression: comparison_expression
            | logical_and_expression AND comparison_expression
            ;
comparison_expression : mathematical_expression
            | comparison_expression EQ EQ mathematical_expression   // ==
            | comparison_expression NEQ mathematical_expression    // !=
            | comparison_expression LT mathematical_expression    // <
            | comparison_expression GT mathematical_expression    // >
            | comparison_expression LE mathematical_expression    // <=
            | comparison_expression GE mathematical_expression    // >=
            ;
mathematical_expression: 
            term
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