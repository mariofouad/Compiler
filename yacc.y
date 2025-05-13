%{
    #include <stdio.h>
    #include <string.h>  // for strdup, strcmp
    #include <stdlib.h>
    #define MAX 1000
    #define MAX_PARAMS 20  // Maximum number of parameters for a function
    

    // === SYMBOL TABLE ===
typedef struct Symbol {
    char *name;
    char *type;
    int isConstant;
    int isFunction;           // Flag to indicate if this is a function
    char *returnType;         // Return type for functions
    struct Symbol *parameters[MAX_PARAMS];  // Array of parameter symbols
    int paramCount;           // Number of parameters
    int scopeLevel;           // Track scope level for block scoping
    struct Symbol *next;
    int isInitialized;       // Flag to indicate if the variable is initialized
} Symbol;

Symbol* symbolTable = NULL;
char* currentType = NULL;
int isConst = 0;
int currentScopeLevel = 0;  //Scope level management
Symbol* functionTable = NULL;   // Function table for storing function symbols
Symbol* currentFunction = NULL; // Current function being processed
int currentArgCount = 0;
int semanticErrorOccurred = 0;



    // === ERROR HANDLING ===

    extern int yylineno;
    void semanticError(const char* msg) {
    fprintf(stderr, "Semantic Error : %s\n",  msg);
    semanticErrorOccurred = 1;
}
    extern char* yytext;
    extern FILE *yyin;
    int yylex(void);
    void yyerror(char* s) {
    fprintf(stderr, "Syntax Error: %s \n", s);
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

void insertSymbol(char* name, char* type, int isConst) {
    if (lookup(name)) {
        char errorMsg[100];
        sprintf(errorMsg, "Variable '%s' already declared", name);
        semanticError(errorMsg);
        return;
    }
    Symbol* sym = (Symbol*)malloc(sizeof(Symbol));
    sym->name = strdup(name);
    sym->type = strdup(type);
    sym->isConstant = isConst;
    sym->next = symbolTable;
    sym->isInitialized = 0; // Initialize to false for new symbols
    symbolTable = sym;
}

void setInitialized(char* name) {
    Symbol* sym = symbolTable;
    while (sym != NULL) {
        if (strcmp(sym->name, name) == 0) {
            sym->isInitialized = 1; // Set to true
            return;
        }
        sym = sym->next;
    }
}

void checkInitialized(char* name) {
    Symbol* sym = symbolTable;
    while (sym != NULL) {
        if (strcmp(sym->name, name) == 0) {
            if (!sym->isInitialized) {
                char errorMsg[100];
                sprintf(errorMsg, "Variable '%s' used before initialization", name);
                semanticError(errorMsg);
            }
            return;
        }
        sym = sym->next;
    }
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


    // === FUNCTION TABLE MANAGEMENT ===
    Symbol* insertFunction(char* name, char* returnType) {
        // check if function already exists
        if (lookup(name)) {
            char errorMsg[100];
            sprintf(errorMsg, "Function '%s' already declared", name);
            yyerror(errorMsg);
            return NULL;
        }
        
        // Create function symbol
        Symbol* func = (Symbol*)malloc(sizeof(Symbol));
        func->name = strdup(name);
        func->type = strdup("function");
        func->returnType = strdup(returnType);
        func->isFunction = 1;
        func->isConstant = 0;
        func->paramCount = 0;
        func->next = functionTable;
        func->scopeLevel = currentScopeLevel;
        
        // Insert into symbol table
        Symbol* symFunc = (Symbol*)malloc(sizeof(Symbol));
        symFunc->name = strdup(name);
        symFunc->type = strdup(returnType);
        symFunc->isConstant = 0;
        symFunc->isFunction = 1;
        symFunc->next = symbolTable;
        symFunc->scopeLevel = currentScopeLevel;
        symbolTable = symFunc;
        
        functionTable = func;
        return func;
    }

    // Add a parameter to a function
    void addParameter(Symbol* function, char* name, char* type) {
        if (function->paramCount >= MAX_PARAMS) {
            yyerror("Too many parameters for function");
            return;
        }
        
        // Create parameter symbol
        Symbol* param = (Symbol*)malloc(sizeof(Symbol));
        param->name = strdup(name);
        param->type = strdup(type);
        param->isConstant = 0;
        param->isFunction = 0;
        param->next = NULL;
        param->scopeLevel = currentScopeLevel + 1;  // Parameters are in function's scope
        
        // Add to function's parameter list
        function->parameters[function->paramCount++] = param;
        
        // Also add to symbol table
        param->next = symbolTable;
        symbolTable = param;
    }

    // Look up a function in the function table
    Symbol* lookupFunction(char* name) {
        Symbol* func = functionTable;
        while (func != NULL) {
            if (strcmp(func->name, name) == 0)
                return func;
            func = func->next;
        }
        return NULL;
    }

    // Enter a new scope (block)
    void enterScope() {
        currentScopeLevel++;
    }

    // Exit current scope
    void exitScope() {
        // Remove all symbols from the current scope level
        Symbol* current = symbolTable;
        Symbol* prev = NULL;
        
        while (current != NULL) {
            if (current->scopeLevel == currentScopeLevel) {
                // Remove this symbol
                Symbol* toDelete = current;
                if (prev == NULL) {
                    symbolTable = current->next;
                    current = symbolTable;
                } else {
                    prev->next = current->next;
                    current = current->next;
                }
                free(toDelete->name);
                free(toDelete->type);
                free(toDelete);
            } else {
                prev = current;
                current = current->next;
            }
        }
        
        currentScopeLevel--;
    }

    // Lookup in current scope or parent scopes
    Symbol* lookupInScope(char* name) {
        Symbol* sym = symbolTable;
        while (sym != NULL) {
            if (strcmp(sym->name, name) == 0)
                return sym;
            sym = sym->next;
        }
        return NULL;
    }

    // Generate function call code
    char* generateFunctionCall(char* functionName, int argCount) {
        char argCountStr[10];
        sprintf(argCountStr, "%d", argCount);
        
        char* resultTemp = newTemp();
        emit("call", functionName, argCountStr, resultTemp);
        return resultTemp;
    }

    // Generate parameter passing code
    void emitParameter(char* argName) {
        emit("param", argName, "", "");
    }

    // Validate function call arguments against parameters
    int validateFunctionCall(Symbol* func, int argCount) {
        if (func->paramCount != argCount) {
            char errorMsg[100];
            sprintf(errorMsg, "Function '%s' called with wrong number of arguments", func->name);
            yyerror(errorMsg);
            return 0;
        }
        // Ideally, we would also check argument types here
        return 1;
    }

%}

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
%type <exprInfo> for_init_decl





%start program

%type <id> constant
%type <caseLabel> switch_case
%type <caseLabel> switch_case_list
%type <exprInfo> conditional_statement
%type <exprInfo> loops
%type <exprInfo> block
%type <exprInfo> for_loop
%type <exprInfo> while_loop
%type <exprInfo> do_while_loop
%type <exprInfo> switch_statement

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
    if (strcmp(currentType, $3.type) != 0) {
                char errorMsg[200];
                sprintf(errorMsg, "Type mismatch in initialization of '%s': expected '%s' but got '%s'", $1, currentType, $3.type);
                semanticError(errorMsg);
            }
    insertSymbol($1, currentType, isConst);
    setInitialized($1);  // mark as initialized
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
    : type ID {
        currentFunction = insertFunction($2, currentType);
        emit("function", $2, "", "");
        enterScope();  // Enter function scope
    } LPAREN parameter_list RPAREN block {
        emit("endFunc", "", "", "");
        exitScope();   // Exit function scope
        currentFunction = NULL;
    }
    | VOID ID {
        currentFunction = insertFunction($2, "void");
        emit("function", $2, "", "");
        enterScope();  // Enter function scope
    } LPAREN parameter_list RPAREN block {
        emit("endFunc", "", "", "");
        exitScope();   // Exit function scope
        currentFunction = NULL;
    }
    ;

argument_list
    : /* empty */ {
        currentArgCount = 0;
    }
    | argument_sequence
    ;

argument_sequence
    : expression {
        emitParameter($1.name);
        currentArgCount = 1; 
    }
    | argument_sequence COMMA expression {
        emitParameter($3.name);
        currentArgCount++;
    }
    ;

parameter_list
    : /* empty */
    | VOID
    | parameter_declaration
    | parameter_list COMMA parameter_declaration
    ;

parameter_declaration
    : type ID {
        if (currentFunction) {
            addParameter(currentFunction, $2, currentType);
        }
    }
    ;

block 
    : LBRACE {
        enterScope();
    } block_items RBRACE {
        exitScope();
        $$.name = strdup("");
        $$.type = strdup("void");
    }
    ;

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
    | conditional_statement {
        $$ = $1;
    }
    | loops {
        $$ = $1;
    }
    | block {
        $$ = $1;
    }
    | CONTINUE SEMI {
        $$.name = strdup("");
        $$.type = strdup("void");
    }
    | BREAK SEMI {
        $$.name = strdup("");
        $$.type = strdup("void");
    }
    | RETURN expression SEMI {
        $$.name = $2.name;
        $$.type = $2.type;
    }
    | RETURN SEMI {
        $$.name = strdup("");
        $$.type = strdup("void");
    }
    ;


expression
    : logical_or_expression {
        $$ = $1;
    }
    | expression ASSIGN expression {
    char* lhs_type = getType($1.name);
    if (lhs_type == NULL) {
    lhs_type = strdup("unknown"); // Fallback to avoid NULL dereference
    }
    char* rhs_type = $3.type;

    if (strcmp(lhs_type, rhs_type) != 0) {
        semanticError("Type mismatch in assignment");
    }

    // Check for i = i + 1 or similar
    if (
        $3.name[0] == 't' &&  // it's a temp
        quadIndex >= 1 &&
        (
            (strcmp(quads[quadIndex - 1].op, "+") == 0 ||
             strcmp(quads[quadIndex - 1].op, "-") == 0 ||
             strcmp(quads[quadIndex - 1].op, "*") == 0 ||
             strcmp(quads[quadIndex - 1].op, "/") == 0)
        ) &&
        strcmp(quads[quadIndex - 1].result, $3.name) == 0 &&
        strcmp(quads[quadIndex - 1].arg1, $1.name) == 0
    ) {
        // Overwrite previous temp op with compound assignment
        char compound[4];
        sprintf(compound, "%s=", quads[quadIndex - 1].op);
        strcpy(quads[quadIndex - 1].op, compound);
        strcpy(quads[quadIndex - 1].result, $1.name);
        quadIndex--;  // remove redundant temp result
        emit(compound, $1.name, quads[quadIndex].arg2, $1.name);  // regenerate as compound
        setInitialized($1.name);  // mark as initialized
    } else {
        emit("=", $3.name, "", $1.name);
        setInitialized($1.name);  // mark as initialized
    }

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
    ;



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
        $$.type = $2.type;
    }
    | LPAREN expression RPAREN {
        $$ = $2;
    }
    | MINUS factor {
        char* temp = newTemp();
        emit("uminus", $2.name, "", temp);
        $$.name = temp;
        $$.type = $2.type;
    }
    ;

primary_expression
    : ID { 
    if (!lookup($1)) {
        semanticError("Undeclared identifier used in expression");
    } else  {
        checkInitialized($1);  // Check if the variable is initialized
    }
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
        Symbol* func = lookupFunction($1);
        if (!func) {
            char errorMsg[100];
            sprintf(errorMsg, "Undefined function '%s'", $1);
            yyerror(errorMsg);
            $$.name = newTemp();
            $$.type = strdup("unknown");
        } else {
            // Validate argument count
            if (validateFunctionCall(func, currentArgCount)) {
                $$.name = generateFunctionCall($1, currentArgCount);
                $$.type = strdup(func->returnType);
            } else {
                $$.name = newTemp();
                $$.type = strdup(func->returnType);
            }
        }
    }
    ;
    
conditional_statement
    : IF LPAREN expression RPAREN block {
        char* endLabel = newLabel();
        emit("ifFalseGoTo", $3.name, "", endLabel);
        emit("label", "", "", endLabel);
        $$.name = strdup("");
        $$.type = strdup("void");
    }
    | IF LPAREN expression RPAREN block ELSE block {
        char* endLabel = newLabel();
        char* elseLabel = newLabel();
        emit("ifFalseGoTo", $3.name, "", elseLabel);
        emit("goto", "", "", endLabel);
        emit("label", "", "", elseLabel);
        emit("label", "", "", endLabel);
        $$.name = strdup("");
        $$.type = strdup("void");
    }
    | switch_statement {
        $$ = $1;
    }
    ;

switch_statement 
    : SWITCH LPAREN expression RPAREN LBRACE switch_case_list RBRACE {
        char* switchTemp = newTemp();
        emit("assign", $3.name, "", switchTemp);
        emitCases($6, switchTemp);
        $$.name = strdup("");
        $$.type = strdup("void");
    }
    ;

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
        semanticError("Undeclared identifier used as a constant for switch\n");
    }
    $$ = strdup($1);
}
;

loops
    : for_loop {
        $$ = $1;
    }
    | while_loop {
        $$ = $1;
    }
    | do_while_loop {
        $$ = $1;
    }
    ;

for_init_decl
    : type ID ASSIGN expression {
        insertSymbol($2, currentType, isConst);
        emit("=", $4.name, "", $2);
        setInitialized($2);  // mark as initialized
        $$.name = $2;
        $$.type = currentType;
    }
    ;

for_loop 
    : FOR LPAREN expression SEMI expression SEMI expression RPAREN block {
        char* start = newLabel();
        char* end = newLabel();
        emit("label", "", "", start);
        emit("ifFalseGoTo", $5.name, "", end);
        emit("goto", "", "", start);
        emit("label", "", "", end);
        $$.name = strdup("");
        $$.type = strdup("void");
    }
    | FOR LPAREN for_init_decl SEMI expression SEMI expression RPAREN block
    {
    char* start = newLabel();
    char* end = newLabel();
    emit("label", "", "", start);
    emit("ifFalseGoTo", $5.name, "", end);
    // emite code l body ally hwa l b lock;
    // emit code for incrementing
    emit("goto", "", "", start);
    emit("label", "", "", end);
    }
    ;

while_loop 
    : WHILE LPAREN expression RPAREN block {
        char* start = newLabel();
        char* end = newLabel();
        emit("label", "", "", start);
        emit("ifFalseGoTo", $3.name, "", end);
        emit("goto", "", "", start);
        emit("label", "", "", end);
        $$.name = strdup("");
        $$.type = strdup("void");
    }
    ;

do_while_loop 
    : DO block WHILE LPAREN expression RPAREN SEMI {
        char* start = newLabel();
        char* end = newLabel();
        emit("label", "", "", start);
        emit("ifFalseGoTo", $5.name, "", end);
        emit("goto", "", "", start);
        emit("label", "", "", end);
        $$.name = strdup("");
        $$.type = strdup("void");
    }
    ;
%%


void printSymbolTable() {
    printf("\nSymbol Table:\n");
    for (Symbol* sym = symbolTable; sym != NULL; sym = sym->next) {
        printf("Name: %s, Type: %s, Constant: %s\n", sym->name, sym->type, sym->isConstant ? "Yes" : "No");
    }
   
}
int main(int argc, char **argv) {
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            perror("Error opening file");
            return 1;
        }
    }

    if (yyparse() == 0 && !semanticErrorOccurred){
        printQuads();
        printSymbolTable();
        return 0;
    } else if (semanticErrorOccurred) {
        fprintf(stderr, "Compilation failed due to semantic errors.\n");
    }

    return 1;
}