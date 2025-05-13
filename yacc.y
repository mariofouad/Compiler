%{
    #include <stdio.h>
    #include <string.h>  // for strdup, strcmp
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
char* getType(char* name) {
    Symbol* sym = symbolTable;
    while (sym != NULL) {
        if (strcmp(sym->name, name) == 0)
            return sym->type;
        sym = sym->next;
    }
    return NULL;  // not found
}

char* resolveType(char* type1, char* type2) {
    if (type1 == NULL || type2 == NULL) {
    printf("Warning: NULL type encountered in resolveType\n");
    return "unknown";
}

    if (strcmp(type1, "float") == 0 || strcmp(type2, "float") == 0)
        return "float";
    if (strcmp(type1, "int") == 0 && strcmp(type2, "int") == 0)
        return "int";
    if (strcmp(type1, "char") == 0 && strcmp(type2, "char") == 0)
        return "char";

    return "unknown"; // add more logic if needed
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
    for (int i = 0; i < quadIndex; i++) {
        printf("%d: (%s, %s, %s, %s)\n", i, quads[i].op, quads[i].arg1, quads[i].arg2, quads[i].result);
    }
}


    // === ERROR HANDLING ===
    extern int yylineno;
    extern char* yytext;
    extern FILE *yyin;
    int yylex(void);
    void yyerror(char* s) {
    fprintf(stderr, "Syntax Error: %s \n", s);
}%}

%union{
    int i;
    float f;
    char c;
    char* id;
    struct {
        char* name;  // name of the result  (e.g., temp variable name)
        char* type;  // type of the result (e.g., "int", "float")
    } exprInfo;     // for full expression information
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

%type <exprInfo> expression
%type <exprInfo> logical_or_expression
%type <exprInfo> logical_and_expression
%type <exprInfo> comparison_expression
%type <exprInfo> mathematical_expression
%type <exprInfo> term
%type <exprInfo> factor
%type <exprInfo> primary_expression
%type <exprInfo> statement




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
  | statement
  ;

declaration
  : type var_list SEMI
  | const_decl
  ;

const_decl : CONST type var_list SEMI ;

var_list: init_declarator
  | var_list COMMA init_declarator
  ;

init_declarator: ID   //need to insert into symbol table
  | ID ASSIGN expression 
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
    : expression SEMI {
        $$ = $1;
    }
    | conditional_statement
    | loops
    | block
    | CONTINUE SEMI
    | BREAK SEMI
    | RETURN expression SEMI
    | RETURN SEMI
    ;

expression
    : logical_or_expression {
        $$ = $1;
    }
    | expression ASSIGN expression {
        // need to check if the variable is declared and if the types match

        char* lhs_type = getType($1.name);

        emit("=", $3.name, "", $1.name);
        $$.name = $1.name;
        $$.type = lhs_type;
    }
    ;

logical_or_expression
    : logical_and_expression {
        $$ = $1;
    }
    | logical_or_expression OR logical_and_expression {
        char* temp = newTemp();
        emit("||", $1.name, $3.name, temp);
        $$.name = temp;
        $$.type = strdup("bool");  // result of logical OR is always bool
    }
    ;

logical_and_expression
    : comparison_expression {
        $$ = $1;
    }
    | logical_and_expression AND comparison_expression {
        char* temp = newTemp();
        emit("&&", $1.name, $3.name, temp);
        $$.name = temp;
        $$.type = strdup("bool");  
    }
    ;

comparison_expression
    : mathematical_expression {
        $$ = $1;
    }
    | comparison_expression EQ mathematical_expression {
        char* temp = newTemp();
        emit("==", $1.name, $3.name, temp);
        $$.name = temp;
        $$.type = strdup("bool");
    }
    | comparison_expression NEQ mathematical_expression {
        char* temp = newTemp();
        emit("!=", $1.name, $3.name, temp);
        $$.name = temp;
        $$.type = strdup("bool");
    }
    | comparison_expression LT mathematical_expression {
        char* temp = newTemp();
        emit("<", $1.name, $3.name, temp);
        $$.name = temp;
        $$.type = strdup("bool");
    }
    | comparison_expression GT mathematical_expression {
        char* temp = newTemp();
        emit(">", $1.name, $3.name, temp);
        $$.name = temp;
        $$.type = strdup("bool");
    }
    | comparison_expression LE mathematical_expression {
        char* temp = newTemp();
        emit("<=", $1.name, $3.name, temp);
        $$.name = temp;
        $$.type = strdup("bool");
    }
    | comparison_expression GE mathematical_expression {
        char* temp = newTemp();
        emit(">=", $1.name, $3.name, temp);
        $$.name = temp;
        $$.type = strdup("bool");
    }
    ;

mathematical_expression
    : term {
        $$ = $1;
    }
    | mathematical_expression PLUS term {
        char* temp = newTemp();
        emit("+", $1.name, $3.name, temp);
        $$.name = temp;
        $$.type = resolveType($1.type, $3.type);  
    }
    | mathematical_expression MINUS term {
        char* temp = newTemp();
        emit("-", $1.name, $3.name, temp);
        $$.name = temp;
        $$.type = resolveType($1.type, $3.type);
    }



term
    : term MUL factor {
        char* temp = newTemp();
        emit("*", $1.name, $3.name, temp);
        $$.name = temp;
        $$.type = resolveType($1.type, $3.type);
    }
    | term DIV factor {
        char* temp = newTemp();
        emit("/", $1.name, $3.name, temp);
        $$.name = temp;
        $$.type = resolveType($1.type, $3.type);
    }
    | term MOD factor {
        char* temp = newTemp();
        emit("%", $1.name, $3.name, temp);
        $$.name = temp;
        $$.type = resolveType($1.type, $3.type);
    }
    | factor {$$=$1;}
    ;

factor
    : primary_expression {$$=$1;}
    | NOT factor {
        char* temp = newTemp();
        emit("!", $2.name, "", temp);
        $$.name = temp;
        $$.type = $2.type; // here type remains the same
    }
    | LPAREN expression RPAREN {
        $$ = $2;
    }
    ;

primary_expression
    : ID {
    // need to check if the variable is declared 
    $$.name = $1;
    $$.type = getType($1);
    }
    | STRING_LITERAL {
    char* temp = newTemp();
    emit("=", $1, "", temp);
    $$.name = temp;
    $$.type = strdup("string");
    }
    | CHAR_LITERAL {
    char val[4]; sprintf(val, "'%c'", $1);
    $$.name = strdup(val);
    $$.type = strdup("char");
    }
    | FLOAT_LITERAL {
    char val[20]; sprintf(val, "%f", $1);
    $$.name = strdup(val);
    $$.type = strdup("float");
    }
    | INT_LITERAL {
    char val[20]; sprintf(val, "%d", $1);
    $$.name = strdup(val);
    $$.type = strdup("int");
    }
    | BOOL_LITERAL {
    char val[10]; sprintf(val, "%d", $1);
    $$.name = strdup(val);
    $$.type = strdup("bool");
    }
    | ID LPAREN argument_list RPAREN {
    // need to check if the function is declared
    char* temp = newTemp();
    emit("CALL", $1, "", temp);
    $$.name = temp;
    $$.type = getType($1); // assuming function return type is stored in symbol table 
    }

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