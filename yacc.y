%{
    #include <stdio.h>
    #include <string.h>  // for strdup, strcmp
    #include <stdlib.h>
    #define MAX 1000
    #define MAX_STACK_SIZE 100  // Maximum number of labels to hold in the stack
    #define LABEL_SIZE 20       // Size for label names (e.g., "L1", "L2", ...)


    // === SYMBOL TABLE ===
typedef struct Symbol {
    char *name;
    char *type;
    int isConstant;
    struct Symbol *next;
} Symbol;

Symbol* symbolTable = NULL;
char* currentType = NULL;
int isConst = 0;

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
int for_start = -1;
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
// Linked List representation of Cases
typedef struct CaseLabel{
    char* value;
    char* label;
    struct CaseLabel* next;
} CaseLabel;
CaseLabel* newCaseLabel(char* value, char* label){
    CaseLabel* node = malloc(sizeof(CaseLabel));
    node->value = strdup(value);
    node->label = strdup(label);
    node->next = NULL;
    return node;
}
void emitCases(CaseLabel* list, char* switchTemp){
    char* endLabel = newLabel();
    CaseLabel* curr = list;
    char* defaultLabel = NULL;
    while(curr){
        if(strcmp(curr->value, "default") == 0){
            defaultLabel = curr->label;
        }else{
            emit("ifEqual", switchTemp, curr->value, curr->label);
        }
        curr = curr->next;
    }
    if(defaultLabel){
        emit("goto", "", "", defaultLabel);
    }else{
        emit("goto", "", "", endLabel);
    }

    emit("label", "", "", endLabel);
}
// Stack structure for labels
typedef struct {
    char labels[MAX_STACK_SIZE][LABEL_SIZE]; // Array to hold label strings
    int top;  // Keeps track of the top of the stack
} LabelStack;
// Push a new label onto the stack
// Initialize the stack
LabelStack labelStack; // << Global instance

// --- Stack Operations ---
void initStack() {
    labelStack.top = -1;
}

int isStackEmpty() {
    return labelStack.top == -1;
}

void push(const char *label) {
    if (labelStack.top < MAX_STACK_SIZE - 1) {
        labelStack.top++;
        strncpy(labelStack.labels[labelStack.top], label, LABEL_SIZE);
    } else {
        printf("Stack overflow!\n");
    }
}

char* pop() {
    if (!isStackEmpty()) {
        return labelStack.labels[labelStack.top--];
    } else {
        printf("Stack underflow!\n");
        return NULL;
    }
}

void moveTwoToEnd(int n, int x) {
    if (x < 0 || x >= n - 1) {
        printf("Invalid position\n");
        return;
    }

    Quadruple a = quads[x];
    Quadruple b = quads[x + 1];

    // Shift elements left
    for (int i = x + 2; i < n; i++) {
        quads[i - 2] = quads[i];
    }

    // Place the saved elements at the end
    quads[n - 2] = a;
    quads[n - 1] = b;
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
    struct CaseLabel* caseLabel;
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

%type <id> constant
%type <caseLabel> switch_case
%type <caseLabel> switch_case_list

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
  : type var_list SEMI  {isConst = 0;}
  | const_decl          {isConst = 0;}
  ;

const_decl : CONST type var_list SEMI {
    isConst = 1;
}
;

var_list: init_declarator
  | var_list COMMA init_declarator
  ;

init_declarator: ID   { insertSymbol($1, currentType, isConst); }
  | ID ASSIGN expression {
    insertSymbol($1, currentType, isConst);
    emit("=", $3.name, "", $1);
  }
  ;

type
    : INT       { currentType = "int"; }
    | FLOAT     { currentType = "float"; }
    | CHAR      { currentType = "char"; }
    | DOUBLE    { currentType = "double"; }
    | BOOL      { currentType = "bool"; }
    | STRING    { currentType = "string"; }
    | LONG      { currentType = "long"; }
    | SHORT     { currentType = "short"; }
    | UNSIGNED  { currentType = "unsigned"; }
    | SIGNED    { currentType = "signed"; }
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
    | MINUS factor // negative (-4)
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
    : IF LPAREN expression {
        char* elseLabel = newLabel();
        emit("ifFalseGoto", $3.name, "", elseLabel);
        push(elseLabel);
    }RPAREN block optional_else
    | switch_statement
    ;

optional_else
    : ELSE {
        // Get the else label and place it here
        // Generate label for the end of the entire if-else
        char* elseLabel = pop();
        char* endLabel = newLabel();
        emit("goto", "", "", endLabel);
        emit("label", "", "", elseLabel);
        push(endLabel);
    }block {
        char* endLabel = pop();
        emit("label", "", "", endLabel);
    }
    |{
        // Get the else label and place it here
        char* elseLabel = pop();
        emit("label", "", "", elseLabel);
    }
    ;

switch_statement : SWITCH LPAREN expression RPAREN LBRACE switch_case_list RBRACE {
    // if expression is true
    char* switchTemp = newTemp();
    emit("assign", $3.name, "", switchTemp);
    emitCases($6, switchTemp);           
};

switch_case_list : /* empty */ {$$ = NULL}| switch_case_list switch_case {
    CaseLabel* q = $1;
    if(!q) $$ = $2;
    else{
        while(q->next) q = q->next;
        q->next = $2;
        $$ = $1;
    }
};

switch_case : CASE constant COLON statement{
                char* label = newLabel();
                emit("label", "", "", label);
                //emit statement code;
                $$ = newCaseLabel($2, label);
}
            | DEFAULT COLON statement {
                char* label = newLabel();
                emit("label", "", "", label);
                // emit statement code
                $$ = newCaseLabel("default", label);
            }
            ;

constant : INT_LITERAL {
    char buffer[20];
    sprintf(buffer, "%d", $1);
    $$ = strdup(buffer);
}
| FLOAT_LITERAL {
    char buffer[20];
    sprintf(buffer, "%f", $1);
    $$ = strdup(buffer);
}
| ID {
    if(!lookup($1)){
        yyerror("Undeclared identifier used as a constant for switch\n");
    }
    $$ = strdup($1);
}
;

loops
    : for_loop
    | while_loop
    | do_while_loop
    ;

for_loop
    : FOR LPAREN expression SEMI {
        char* start = newLabel();
        char* end = newLabel();
        emit("label", "", "", start);
        push(start); push(end);
    }
    expression SEMI {
        char* end = pop();
        emit("ifFalseGoTo", $6.name, "", end);
        push(end);
        for_start = quadIndex;
        printf("Quad index is ");
        printf("Quad index is %d\n", for_start);
    }
    expression RPAREN block {
        moveTwoToEnd(quadIndex, for_start);
        char* end = pop();
        char* start = pop();
        emit("goto", "", "", start);
        emit("label", "", "", end);
    }
    | FOR LPAREN INT expression SEMI {
        char* start = newLabel();
        char* end = newLabel();
        emit("label", "", "", start);
        push(start); push(end);
    }
    expression SEMI {
        char* end = pop();
        emit("ifFalseGoTo", $7.name, "", end);
        push(end);
        for_start = quadIndex;
        printf("Quad index is ");
        printf("Quad index is %d\n", for_start);
    }
    expression RPAREN block {
        moveTwoToEnd(quadIndex, for_start);       
        char* end = pop();
        char* start = pop();
        emit("goto", "", "", start);
        emit("label", "", "", end);
    }
    ;

while_loop : WHILE LPAREN{
    char* start = newLabel();
    emit("label", "", "", start);
    push(start);
} expression{
    char* end = newLabel();
    push(end);
    emit("ifFalseGoTo", $4.name, "", end);

} RPAREN block {
    char* end = pop();
    char* start = pop();
    // emit condition expression
    
    // body
    emit("goto", "", "", start);
    emit("label", "", "", end);
};

do_while_loop : DO {
        char* start = newLabel();
        emit("label", "", "", start);
        push(start);

    } block WHILE LPAREN expression RPAREN SEMI
    {
        char* end = newLabel();
        char* start = pop();
        // emit code for block
        // emit code for condition
        emit("ifFalseGoTo", $6.name, "", end);
        emit("goto", "", "", start);
        emit("label", "", "", end);
    }
 ;

%%



int main(int argc, char **argv) {
    initStack();
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