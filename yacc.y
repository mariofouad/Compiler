%{
    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <ctype.h>
    #define MAX 1000
    #define MAX_PARAMS 20  // Maximum number of parameters for a function
    #define MAX_STACK_SIZE 100 
    #define LABEL_SIZE 20 
    #define MAX_NESTED_LOOPS 100


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
int for_start = -1;
int hasReturnStatement = 0;
int blockNestingLevel = 0;

    // === ERROR HANDLING ===

    extern int yylineno;
    void semanticError(const char* msg) {
    fprintf(stderr, "Semantic Error : %s\n",  msg);
    semanticErrorOccurred = 1;
}
    extern int yylineno;
    extern int column;
    extern char* yytext;
    extern FILE *yyin;
    int yylex(void);
    void yyerror(const char* msg) {
    fprintf(stderr, "Syntax Error at line %d, column %d near '%s': %s\n", yylineno, column, yytext, msg);
}


int lookupInCurrentScope(char* name) {
    Symbol* sym = symbolTable;
    while (sym != NULL) {
        // Only check current scope
        if (strcmp(sym->name, name) == 0 && sym->scopeLevel == currentScopeLevel)
            return 1;
        sym = sym->next;
    }
    return 0;
}

int lookup(char* name) {
    Symbol* sym = symbolTable;
    while (sym != NULL) {
        // Check if name matches AND it's in the current scope or an enclosing scope
        if (strcmp(sym->name, name) == 0 && sym->scopeLevel <= currentScopeLevel)
            return 1;
        sym = sym->next;
    }
    return 0;
}

void insertSymbol(char* name, char* type, int isConst) {
    if (lookupInCurrentScope(name)) {
        char errorMsg[100];
        sprintf(errorMsg, "Variable '%s' already declared in current scope", name);
        semanticError(errorMsg);
        return;
    }
    Symbol* sym = (Symbol*)malloc(sizeof(Symbol));
    sym->name = strdup(name);
    sym->type = strdup(type);
    sym->isConstant = isConst;
    sym->scopeLevel = currentScopeLevel;
    sym->next = symbolTable;
    sym->isInitialized = 0; // Initialize to false for new symbols
    sym->isFunction = 0;
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

int isConstant(char* name) {
    Symbol* sym = symbolTable;
    while (sym != NULL) {
        if (strcmp(sym->name, name) == 0)
            return sym->isConstant;
        sym = sym->next;
    }
    return -1;  // not found
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
    if (strcmp(type1, "bool") == 0 && strcmp(type2, "bool") == 0)
        return "bool";
    if ((strcmp(type1, "bool")==0 && strcmp(type2, "int")==0) || (strcmp(type1, "int")==0 && strcmp(type2, "bool")==0))
        return "bool"; // bool and int can be mixed in some cases
    

    return "unknown"; // add more logic if needed
}
int areTypesCompatible(char* expected, char* actual) {
    if (expected == NULL || actual == NULL) {
        return 0; // NULL types are not compatible
    }
    if (strcmp(expected, actual) == 0) {
        return 1; // Types are the same
    }
    if((strcmp(expected, "int")==0 && strcmp(actual, "bool")==0) || (strcmp(expected, "bool")==0 && strcmp(actual, "int")==0)){
        return 1; // bool and int can be mixed in some cases
    }
    return 0; // Types are not compatible
}


// === INTERMEDIATE CODE ===
typedef struct {
    char* op;
    char* arg1;
    char* arg2;
    char* result;
} Quadruple;

Quadruple quads[MAX];
char* continueLabels[MAX_NESTED_LOOPS];
char* breakLabels[MAX_NESTED_LOOPS];
int loopDepth = 0;
int quadIndex = 0;
int tempCount = 0;
int labelCount = 0;
char* switchCharacter;
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
        param->isInitialized = 1;
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
        if (func->paramCount > argCount) {
            char errorMsg[100];
            sprintf(errorMsg, "Function '%s' called with too few arguments", func->name);
            yyerror(errorMsg);
            return 0;
        }else if (func->paramCount < argCount) {
            char errorMsg[100];
            sprintf(errorMsg, "Function '%s' called with too many arguments", func->name);
            yyerror(errorMsg);
            return 0;
        }
        // Ideally, we would also check argument types here
        return 1;
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
    void moveOneToEnd(int n, int x) {
        if (x < 0 || x >= n) {
            printf("Invalid position\n");
            return;
        }

        Quadruple a = quads[x];
        // Shift elements left
        for (int i = x + 1; i < n; i++) {
            quads[i - 1] = quads[i];
        }

        // Place the saved element at the end
        quads[n - 1] = a;
    } 

// Helper functions
void pushLoopLabels(char* continueLabel, char* breakLabel) {
    if (loopDepth < MAX_NESTED_LOOPS) {
        continueLabels[loopDepth] = strdup(continueLabel);
        breakLabels[loopDepth] = strdup(breakLabel);
        loopDepth++;
    } else {
        semanticError("Too many nested loops");
    }
}

void popLoopLabels() {
    if (loopDepth > 0) {
        loopDepth--;
        free(continueLabels[loopDepth]);
        free(breakLabels[loopDepth]);
    }
}

char* getCurrentContinueLabel() {
    if (loopDepth > 0) {
        return continueLabels[loopDepth-1];
    }
    semanticError("'continue' statement not within loop");
    return "";
}

char* getCurrentBreakLabel() {
    if (loopDepth > 0) {
        return breakLabels[loopDepth-1];
    }
    semanticError("'break' statement not within loop");
    return "";
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
        int isTarget;  //1 if target of assignment, 0 otherwise
    } exprInfo;     // for full expression information
}

%token <i> INT
%token <f> FLOAT
%token <id> ID
%token <i> INT_LITERAL
%token <f> FLOAT_LITERAL
%token <c> CHAR_LITERAL
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
  : type var_list SEMI  
  | const_decl          
  ;

const_decl : CONST
{
    isConst = 1;
} type var_list SEMI {
    isConst = 0;
}
;

var_list: init_declarator
  | var_list COMMA init_declarator
  ;

init_declarator: ID   { 
    insertSymbol($1, currentType, isConst); 
    if(isConstant($1) == 1) {
        semanticError("Constant variables must be initialized");
    }
    }
  | ID ASSIGN expression {
    if (!areTypesCompatible(currentType, $3.type)) {
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
    | LONG      { currentType = "long"; }
    | SHORT     { currentType = "short"; }
    | UNSIGNED  { currentType = "unsigned"; }
    | SIGNED    { currentType = "signed"; }
    ;

function_definition
    : type ID {
        currentFunction = insertFunction($2, currentType);
        emit("function", $2, "", "");
        enterScope();  
        hasReturnStatement = 0;
        blockNestingLevel = -1;  // So the function's main block is level 0
    } LPAREN parameter_list RPAREN block {
        if (strcmp(currentFunction->returnType, "void") != 0 && hasReturnStatement == 0) {
            char errorMsg[200];
            sprintf(errorMsg, "Function '%s' with return type '%s' has no top-level return statement", $2, currentFunction->returnType);
            semanticError(errorMsg);
        }
        emit("endFunc", "", "", "");
        exitScope();
        currentFunction = NULL;
    }
    | VOID ID {
        currentFunction = insertFunction($2, "void");
        emit("function", $2, "", "");
        enterScope();
        hasReturnStatement = 0;
        blockNestingLevel = -1;  // So the function's main block is level 0
    } LPAREN parameter_list RPAREN block {
        emit("endFunc", "", "", "");
        exitScope();
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
        blockNestingLevel++;
    } block_items RBRACE {        
        blockNestingLevel--;
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
        char* continueLabel = getCurrentContinueLabel();
        emit("goto", "", "", continueLabel);
        $$.name = strdup("");
        $$.type = strdup("void");
    }
    | BREAK SEMI {
        char* breakLabel = getCurrentBreakLabel();
        emit("goto", "", "", breakLabel);
        $$.name = strdup("");
        $$.type = strdup("void");
    }
| RETURN expression SEMI {
    if (currentFunction) {
        if (strcmp(currentFunction->returnType, $2.type) != 0) {
            semanticError("Return type doesn't match function return type");
        }
        
        // Check if this is a nested return
        if (blockNestingLevel > 0) {
            semanticError("Return statement must be at the top level of function body");
        } else {
            hasReturnStatement = 1;
        }
        
        emit("return", $2.name, "", "");
    }
    $$.name = strdup("");
    $$.type = strdup("void");
}
| RETURN SEMI {
    if (currentFunction && strcmp(currentFunction->returnType, "void") != 0) {
        char errorMsg[100];
        sprintf(errorMsg, "Function '%s' has return type '%s' but returns no value", 
                currentFunction->name, currentFunction->returnType);
        semanticError(errorMsg);
    }
    
    // Check if this is a nested return
    if (blockNestingLevel > 0) {
        semanticError("Return statement must be at the top level of function body");
    } else if (currentFunction) {
        hasReturnStatement = 1;
    }
    
    if (currentFunction) {
        emit("return", "", "", "");
    }
    $$.name = strdup("");
    $$.type = strdup("void");
}
    ;


expression
    : logical_or_expression {
        $$ = $1;
    }
    | expression ASSIGN expression {
    $1.isTarget = 1;

    if ($3.name[0] != 't' && $3.name[0] != '\'' && !isdigit($3.name[0])) {
        checkInitialized($3.name);
    }

    if (!lookup($1.name)) {
        char errorMsg[100];
        sprintf(errorMsg, "Assignment to undeclared variable '%s'", $1.name);
        semanticError(errorMsg);
    }

    if (isConstant($1.name)) {
        semanticError("Cannot modify constant variable");
    }

    char* lhs_type = getType($1.name);
    char* rhs_type = $3.type;

    if (lhs_type != NULL && !areTypesCompatible(lhs_type, rhs_type)) {
        semanticError("Type mismatch in assignment");
    }

    if (lhs_type == NULL) {
        lhs_type = strdup("unknown");
    }

    // Compound assignment detection
    if (
        $3.name[0] == 't' &&
        quadIndex >= 1 &&
        (
            strcmp(quads[quadIndex - 1].op, "+") == 0 ||
            strcmp(quads[quadIndex - 1].op, "-") == 0 ||
            strcmp(quads[quadIndex - 1].op, "*") == 0 ||
            strcmp(quads[quadIndex - 1].op, "/") == 0
        ) &&
        strcmp(quads[quadIndex - 1].result, $3.name) == 0 &&
        strcmp(quads[quadIndex - 1].arg1, $1.name) == 0
    ) {
        // Convert to compound assignment
        char compound[4];
        sprintf(compound, "%s=", quads[quadIndex - 1].op);

        // Reuse operand 2 from previous quad
        char* rhs = quads[quadIndex - 1].arg2;

        // Remove previous quad
        quadIndex--;

        // Emit optimized compound quad
        emit(compound, $1.name, rhs, $1.name);

        setInitialized($1.name);
    } else {
        emit("=", $3.name, "", $1.name);
        setInitialized($1.name);
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
         if ($1.name[0] != 't' && $1.name[0] != '\'' && !isdigit($1.name[0])) {
            checkInitialized($1.name);
        }
        if ($3.name[0] != 't' && $3.name[0] != '\'' && !isdigit($3.name[0])) {
            checkInitialized($3.name);
        }
        
        char* temp = newTemp();
        emit("+", $1.name, $3.name, temp);
        $$.name = temp;
        $$.type = resolveType($1.type, $3.type);   
    }
    | mathematical_expression MINUS term {
        if ($1.name[0] != 't' && $1.name[0] != '\'' && !isdigit($1.name[0])) {
            checkInitialized($1.name);
        }
        if ($3.name[0] != 't' && $3.name[0] != '\'' && !isdigit($3.name[0])) {
            checkInitialized($3.name);
        }
        
        char* temp = newTemp();
        emit("-", $1.name, $3.name, temp);
        $$.name = temp;
        $$.type = resolveType($1.type, $3.type);
    }
    ;



term
    : term MUL factor {
        if ($1.name[0] != 't' && $1.name[0] != '\'' && !isdigit($1.name[0])) {
            checkInitialized($1.name);
        }
        if ($3.name[0] != 't' && $3.name[0] != '\'' && !isdigit($3.name[0])) {
            checkInitialized($3.name);
        }
        
        char* temp = newTemp();
        emit("*", $1.name, $3.name, temp);
        $$.name = temp;
        $$.type = resolveType($1.type, $3.type);
    }
    | term DIV factor {
        if ($1.name[0] != 't' && $1.name[0] != '\'' && !isdigit($1.name[0])) {
            checkInitialized($1.name);
        }
        if ($3.name[0] != 't' && $3.name[0] != '\'' && !isdigit($3.name[0])) {
            checkInitialized($3.name);
        }
        
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
    }
    $$.name = $1;
    $$.type = getType($1);
    $$.isTarget = 0;  // Default: not a target
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

switch_statement : SWITCH {
    char* switchEndLabel = newLabel();
    push(switchEndLabel);
}LPAREN expression RPAREN LBRACE {
    switchCharacter = $4.name; 
}switch_case_list RBRACE {
    char* switchEndLabel = pop();
    emit("label", "", "", switchEndLabel);   
};

switch_case_list : /* empty */ {$$ = NULL}| switch_case_list switch_case;
recursive_switch_statement:
    /* */
    | recursive_switch_statement statement;
switch_case 
            :CASE constant COLON {
                char* temp = newTemp();
                emit("==", switchCharacter, $2, temp);
                char* end = newLabel();
                emit("ifFaleGoTo", temp, "", end);
                push(end);
                }
            recursive_switch_statement{
                char* currentEnd = pop();
                char* allEnd = pop();
                emit("goto", "", "", allEnd);
                emit("label", "", "", currentEnd);
                push(allEnd);

            }
                
            | DEFAULT COLON  {
            }recursive_switch_statement{
                char* end = pop();
                emit("goto", "", "", end);
                push(end);
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
    : FOR LPAREN expression SEMI {
        char* start = newLabel();
        char* end = newLabel();
        pushLoopLabels(start, end);
        emit("label", "", "", start);
        push(start); push(end);
    }
    expression SEMI {
        char* end = pop();
        emit("ifFalseGoTo", $6.name, "", end);
        push(end);
        for_start = quadIndex;
    }
    expression RPAREN block {
        moveOneToEnd(quadIndex, for_start);
        char* end = pop();
        char* start = pop();
        popLoopLabels();
        emit("goto", "", "", start);
        emit("label", "", "", end);
    }
    | FOR LPAREN for_init_decl SEMI{
        char* start = newLabel();
        char* end = newLabel();
        pushLoopLabels(start, end);
        emit("label", "", "", start);
        push(start); push(end);
    }
    expression SEMI {
        char* end = pop();
        emit("ifFalseGoTo", $6.name, "", end);
        push(end);
        for_start = quadIndex;
    }
    expression RPAREN block {
        moveOneToEnd(quadIndex, for_start);       
        char* end = pop();
        char* start = pop();
        popLoopLabels();
        emit("goto", "", "", start);
        emit("label", "", "", end);
    }
    ;

while_loop : WHILE LPAREN{
    char* start = newLabel();
    emit("label", "", "", start);
    push(start);
} expression{
    char* start = pop();
    char* end = newLabel();
    pushLoopLabels(start, end);
    push(start);
    push(end);
    emit("ifFalseGoTo", $4.name, "", end);

} RPAREN block {
    char* end = pop();
    char* start = pop();
    // emit condition expression
    popLoopLabels();
    // body
    emit("goto", "", "", start);
    emit("label", "", "", end);
};

do_while_loop : DO {
        char* start = newLabel();
        char* end = newLabel();
        emit("label", "", "", start);
        push(start); push(end);
        pushLoopLabels(start, end);

    } block WHILE LPAREN expression RPAREN SEMI
    {
        char* end = pop();
        char* start = pop();
        // emit code for block
        // emit code for condition
        emit("ifFalseGoTo", $6.name, "", end);
        emit("goto", "", "", start);
        emit("label", "", "", end);
        popLoopLabels();
        $$.name = strdup("");
        $$.type = strdup("void");
    }
    ;
%%


void printSymbolTable() {
    printf("\nSymbol Table:\n");
    for (Symbol* sym = symbolTable; sym != NULL; sym = sym->next) {
        printf("Name: %s, Type: %s, Constant: %s, isFunction: %s\n", sym->name, sym->type, sym->isConstant ? "Yes" : "No", sym->isFunction ? "Yes" : "No");
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