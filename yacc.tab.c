
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.4.1"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 1 "yacc.y"

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
bool fromSwitch = 0;
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

    CaseLabel* curr = list;
    char* defaultLabel = NULL;
    while(curr){
        if(strcmp(curr->value, "default") == 0){
            defaultLabel = curr->label;
        }else{
            emit("ifEqualGoTo", switchTemp, curr->value, curr->label);
        }
        curr = curr->next;
    }
    if(defaultLabel){
        emit("goto", "", "", defaultLabel);
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


/* Line 189 of yacc.c  */
#line 560 "yacc.tab.c"

/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     INT = 258,
     FLOAT = 259,
     ID = 260,
     INT_LITERAL = 261,
     FLOAT_LITERAL = 262,
     CHAR_LITERAL = 263,
     BOOL_LITERAL = 264,
     IF = 265,
     ELSE = 266,
     WHILE = 267,
     DO = 268,
     RETURN = 269,
     BREAK = 270,
     MOD = 271,
     NOT = 272,
     VOID = 273,
     CONTINUE = 274,
     ASSIGN = 275,
     EQ = 276,
     NEQ = 277,
     LE = 278,
     GE = 279,
     LT = 280,
     GT = 281,
     PLUS = 282,
     MINUS = 283,
     MUL = 284,
     DIV = 285,
     LPAREN = 286,
     RPAREN = 287,
     LBRACE = 288,
     RBRACE = 289,
     SEMI = 290,
     COMMA = 291,
     SWITCH = 292,
     CASE = 293,
     DEFAULT = 294,
     COLON = 295,
     FOR = 296,
     AND = 297,
     OR = 298,
     CHAR = 299,
     CONST = 300,
     DOUBLE = 301,
     BOOL = 302,
     SHORT = 303,
     LONG = 304,
     UNSIGNED = 305,
     SIGNED = 306
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 487 "yacc.y"

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



/* Line 214 of yacc.c  */
#line 662 "yacc.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 674 "yacc.tab.c"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   286

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  52
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  54
/* YYNRULES -- Number of rules.  */
#define YYNRULES  115
/* YYNRULES -- Number of states.  */
#define YYNSTATES  203

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   306

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     6,     9,    11,    13,    15,    19,
      21,    22,    28,    30,    34,    36,    40,    42,    44,    46,
      48,    50,    52,    54,    56,    58,    59,    67,    68,    76,
      77,    79,    81,    85,    86,    88,    90,    94,    97,    98,
     103,   104,   107,   109,   111,   114,   116,   118,   120,   123,
     126,   130,   133,   135,   139,   141,   145,   147,   151,   153,
     157,   161,   165,   169,   173,   177,   179,   183,   187,   191,
     195,   199,   201,   203,   206,   210,   213,   215,   217,   219,
     221,   223,   228,   229,   237,   239,   240,   244,   245,   246,
     255,   256,   259,   261,   265,   266,   272,   273,   278,   280,
     282,   284,   286,   288,   290,   295,   296,   297,   309,   310,
     311,   323,   324,   325,   333,   334
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      53,     0,    -1,    54,    -1,    -1,    54,    55,    -1,    62,
      -1,    56,    -1,    73,    -1,    61,    59,    35,    -1,    57,
      -1,    -1,    45,    58,    61,    59,    35,    -1,    60,    -1,
      59,    36,    60,    -1,     5,    -1,     5,    20,    74,    -1,
       3,    -1,     4,    -1,    44,    -1,    46,    -1,    47,    -1,
      49,    -1,    48,    -1,    50,    -1,    51,    -1,    -1,    61,
       5,    63,    31,    67,    32,    69,    -1,    -1,    18,     5,
      64,    31,    67,    32,    69,    -1,    -1,    66,    -1,    74,
      -1,    66,    36,    74,    -1,    -1,    18,    -1,    68,    -1,
      67,    36,    68,    -1,    61,     5,    -1,    -1,    33,    70,
      71,    34,    -1,    -1,    71,    72,    -1,    56,    -1,    73,
      -1,    74,    35,    -1,    82,    -1,    94,    -1,    69,    -1,
      19,    35,    -1,    15,    35,    -1,    14,    74,    35,    -1,
      14,    35,    -1,    75,    -1,    74,    20,    74,    -1,    76,
      -1,    75,    43,    76,    -1,    77,    -1,    76,    42,    77,
      -1,    78,    -1,    77,    21,    78,    -1,    77,    22,    78,
      -1,    77,    25,    78,    -1,    77,    26,    78,    -1,    77,
      23,    78,    -1,    77,    24,    78,    -1,    79,    -1,    78,
      27,    79,    -1,    78,    28,    79,    -1,    79,    29,    80,
      -1,    79,    30,    80,    -1,    79,    16,    80,    -1,    80,
      -1,    81,    -1,    17,    80,    -1,    31,    74,    32,    -1,
      28,    80,    -1,     5,    -1,     8,    -1,     7,    -1,     6,
      -1,     9,    -1,     5,    31,    65,    32,    -1,    -1,    10,
      31,    74,    83,    32,    69,    84,    -1,    86,    -1,    -1,
      11,    85,    69,    -1,    -1,    -1,    37,    87,    31,    74,
      32,    33,    88,    34,    -1,    -1,    88,    90,    -1,    73,
      -1,    73,    15,    35,    -1,    -1,    38,    93,    40,    91,
      89,    -1,    -1,    39,    40,    92,    89,    -1,     6,    -1,
       7,    -1,     5,    -1,    96,    -1,   101,    -1,   104,    -1,
      61,     5,    20,    74,    -1,    -1,    -1,    41,    31,    74,
      35,    97,    74,    35,    98,    74,    32,    69,    -1,    -1,
      -1,    41,    31,    95,    35,    99,    74,    35,   100,    74,
      32,    69,    -1,    -1,    -1,    12,    31,   102,    74,   103,
      32,    69,    -1,    -1,    13,   105,    69,    12,    31,    74,
      32,    35,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   549,   549,   552,   554,   558,   559,   560,   564,   565,
     569,   568,   576,   577,   580,   586,   599,   600,   601,   602,
     603,   604,   605,   606,   607,   611,   611,   627,   627,   641,
     644,   648,   652,   658,   660,   661,   662,   666,   674,   674,
     685,   687,   691,   692,   695,   698,   701,   704,   707,   713,
     722,   740,   765,   768,   833,   836,   845,   848,   857,   860,
     866,   872,   878,   884,   890,   899,   902,   915,   933,   946,
     959,   965,   969,   970,   976,   979,   988,   996,  1001,  1006,
    1011,  1016,  1039,  1039,  1044,  1048,  1048,  1060,  1067,  1067,
    1079,  1079,  1089,  1090,  1092,  1092,  1110,  1110,  1127,  1132,
    1137,  1146,  1149,  1152,  1157,  1166,  1173,  1166,  1187,  1194,
    1187,  1210,  1214,  1210,  1232,  1232
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "INT", "FLOAT", "ID", "INT_LITERAL",
  "FLOAT_LITERAL", "CHAR_LITERAL", "BOOL_LITERAL", "IF", "ELSE", "WHILE",
  "DO", "RETURN", "BREAK", "MOD", "NOT", "VOID", "CONTINUE", "ASSIGN",
  "EQ", "NEQ", "LE", "GE", "LT", "GT", "PLUS", "MINUS", "MUL", "DIV",
  "LPAREN", "RPAREN", "LBRACE", "RBRACE", "SEMI", "COMMA", "SWITCH",
  "CASE", "DEFAULT", "COLON", "FOR", "AND", "OR", "CHAR", "CONST",
  "DOUBLE", "BOOL", "SHORT", "LONG", "UNSIGNED", "SIGNED", "$accept",
  "program", "external_list", "external", "declaration", "const_decl",
  "$@1", "var_list", "init_declarator", "type", "function_definition",
  "$@2", "$@3", "argument_list", "argument_sequence", "parameter_list",
  "parameter_declaration", "block", "$@4", "block_items", "block_item",
  "statement", "expression", "logical_or_expression",
  "logical_and_expression", "comparison_expression",
  "mathematical_expression", "term", "factor", "primary_expression",
  "conditional_statement", "$@5", "optional_else", "$@6",
  "switch_statement", "$@7", "switch_case_list", "break_statement",
  "switch_case", "$@8", "$@9", "constant", "loops", "for_init_decl",
  "for_loop", "$@10", "$@11", "$@12", "$@13", "while_loop", "$@14", "$@15",
  "do_while_loop", "$@16", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    52,    53,    54,    54,    55,    55,    55,    56,    56,
      58,    57,    59,    59,    60,    60,    61,    61,    61,    61,
      61,    61,    61,    61,    61,    63,    62,    64,    62,    65,
      65,    66,    66,    67,    67,    67,    67,    68,    70,    69,
      71,    71,    72,    72,    73,    73,    73,    73,    73,    73,
      73,    73,    74,    74,    75,    75,    76,    76,    77,    77,
      77,    77,    77,    77,    77,    78,    78,    78,    79,    79,
      79,    79,    80,    80,    80,    80,    81,    81,    81,    81,
      81,    81,    83,    82,    82,    85,    84,    84,    87,    86,
      88,    88,    89,    89,    91,    90,    92,    90,    93,    93,
      93,    94,    94,    94,    95,    97,    98,    96,    99,   100,
      96,   102,   103,   101,   105,   104
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     0,     2,     1,     1,     1,     3,     1,
       0,     5,     1,     3,     1,     3,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     0,     7,     0,     7,     0,
       1,     1,     3,     0,     1,     1,     3,     2,     0,     4,
       0,     2,     1,     1,     2,     1,     1,     1,     2,     2,
       3,     2,     1,     3,     1,     3,     1,     3,     1,     3,
       3,     3,     3,     3,     3,     1,     3,     3,     3,     3,
       3,     1,     1,     2,     3,     2,     1,     1,     1,     1,
       1,     4,     0,     7,     1,     0,     3,     0,     0,     8,
       0,     2,     1,     3,     0,     5,     0,     4,     1,     1,
       1,     1,     1,     1,     4,     0,     0,    11,     0,     0,
      11,     0,     0,     7,     0,     8
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,     2,     1,    16,    17,    76,    79,    78,    77,
      80,     0,     0,   114,     0,     0,     0,     0,     0,     0,
       0,    38,    88,     0,    18,    10,    19,    20,    22,    21,
      23,    24,     4,     6,     9,     0,     5,    47,     7,     0,
      52,    54,    56,    58,    65,    71,    72,    45,    84,    46,
     101,   102,   103,    29,     0,   111,     0,    51,     0,    49,
      73,    27,    48,    75,     0,    40,     0,     0,     0,    14,
       0,    12,     0,    44,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    30,    31,
      82,     0,     0,    50,     0,    74,     0,     0,     0,     0,
       0,     0,     0,     0,     8,     0,    53,    55,    57,    59,
      60,    63,    64,    61,    62,    66,    67,    70,    68,    69,
      81,     0,     0,   112,     0,    33,    39,    42,     0,    41,
      43,     0,     0,   105,   108,    14,     0,    15,    33,    13,
      32,     0,     0,     0,    34,     0,     0,    35,     0,     0,
       0,     0,    11,     0,    87,     0,     0,    37,     0,     0,
      90,   104,     0,     0,     0,    85,    83,   113,     0,    28,
      36,     0,   106,   109,    26,     0,   115,    89,     0,     0,
      91,     0,     0,    86,   100,    98,    99,     0,    96,     0,
       0,    94,     0,     0,     0,     0,    92,    97,   107,   110,
      95,     0,    93
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,    32,    33,    34,    68,    70,    71,   145,
      36,   103,    94,    87,    88,   146,   147,    37,    65,    96,
     129,   196,    39,    40,    41,    42,    43,    44,    45,    46,
      47,   122,   166,   175,    48,    66,   171,   197,   180,   195,
     192,   187,    49,   100,    50,   150,   181,   151,   182,    51,
      91,   142,    52,    56
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -56
static const yytype_int16 yypact[] =
{
     -56,    35,    15,   -56,   -56,   -56,   -16,   -56,   -56,   -56,
     -56,    45,    56,   -56,   192,    12,   249,     4,    73,   249,
     249,   -56,   -56,    80,   -56,   -56,   -56,   -56,   -56,   -56,
     -56,   -56,   -56,   -56,   -56,    86,   -56,   -56,   -56,    -9,
      84,    76,   137,    67,    25,   -56,   -56,   -56,   -56,   -56,
     -56,   -56,   -56,   249,   249,   -56,    95,   -56,    -4,   -56,
     -56,   -56,   -56,   -56,   -15,   -56,   100,   187,    75,    51,
       9,   -56,   249,   -56,   249,   249,   249,   249,   249,   249,
     249,   249,   249,   249,   249,   249,   249,    98,    96,   113,
     113,   249,   122,   -56,   109,   -56,   138,   249,   144,    16,
     119,   151,   249,   133,   -56,   151,   113,    76,   137,    67,
      67,    67,    67,    67,    67,    25,    25,   -56,   -56,   -56,
     -56,   249,   141,   113,   134,    66,   -56,   -56,   151,   -56,
     -56,    17,   150,   -56,   -56,   154,    63,   113,    66,   -56,
     113,    95,   145,   249,   -56,   171,   -22,   -56,   147,   249,
     249,   249,   -56,    53,   167,    95,    18,   -56,    95,    75,
     -56,   113,    22,    55,    95,   -56,   -56,   -56,   146,   -56,
     -56,   -26,   -56,   -56,   -56,    95,   -56,   -56,    99,   162,
     -56,   249,   249,   -56,   -56,   -56,   -56,   163,   -56,    60,
      61,   -56,   234,    95,    95,   234,   190,   -56,   -56,   -56,
     -56,   172,   -56
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -56,   -56,   -56,   -56,   110,   -56,   -56,   107,   105,     0,
     -56,   -56,   -56,   -56,   -56,    74,    52,   -55,   -56,   -56,
     -56,     1,   -14,   -56,   139,   142,   205,    19,   -12,   -56,
     -56,   -56,   -56,   -56,   -56,   -56,   -56,    21,   -56,   -56,
     -56,   -56,   -56,   -56,   -56,   -56,   -56,   -56,   -56,   -56,
     -56,   -56,   -56,   -56
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -26
static const yytype_int16 yytable[] =
{
      58,    92,    35,    38,    60,    72,    64,    63,   177,    61,
     158,    72,   178,   179,   159,    53,    72,    95,     4,     5,
       6,     7,     8,     9,    10,    11,    73,    12,    13,    14,
      15,    93,    16,    17,    18,     3,    72,    72,    72,    89,
      90,    84,    72,    19,   104,   105,    20,    59,    21,   148,
     168,   133,    22,    99,    85,    86,    23,   172,   106,    24,
      25,    26,    27,    28,    29,    30,    31,    98,   101,     4,
       5,   102,   117,   118,   119,    72,    54,   123,     4,     5,
      72,    72,   -25,   131,   144,   164,   154,    55,   137,   159,
     173,    69,   193,   194,    82,    83,   128,   130,   152,   105,
     167,   115,   116,   169,   184,   185,   186,   140,    62,   174,
      24,    67,    26,    27,    28,    29,    30,    31,    75,    24,
     183,    26,    27,    28,    29,    30,    31,    74,    21,   156,
     120,    97,   121,    72,   124,   161,   162,   163,   198,   199,
     125,     4,     5,     6,     7,     8,     9,    10,    11,   132,
      12,    13,    14,    15,   134,    16,   135,    18,    76,    77,
      78,    79,    80,    81,   138,   143,    19,   189,   190,    20,
     149,    21,   126,   141,   102,    22,   157,   155,   165,    23,
     160,   176,    24,    25,    26,    27,    28,    29,    30,    31,
       4,     5,     6,     7,     8,     9,    10,     6,     7,     8,
       9,    10,   188,   191,    16,   201,   127,   202,   136,    16,
     139,   170,   153,   107,     0,    19,   200,   108,    20,     0,
      19,     0,     0,    20,     0,     0,     0,    57,     0,     0,
       0,    24,     0,    26,    27,    28,    29,    30,    31,     6,
       7,     8,     9,    10,    11,     0,    12,    13,    14,    15,
       0,    16,     0,    18,     6,     7,     8,     9,    10,     0,
       0,     0,    19,     0,     0,    20,    16,    21,     0,     0,
       0,    22,     0,     0,     0,    23,     0,    19,     0,     0,
      20,   109,   110,   111,   112,   113,   114
};

static const yytype_int16 yycheck[] =
{
      14,    56,     2,     2,    16,    20,    20,    19,    34,     5,
      32,    20,    38,    39,    36,    31,    20,    32,     3,     4,
       5,     6,     7,     8,     9,    10,    35,    12,    13,    14,
      15,    35,    17,    18,    19,     0,    20,    20,    20,    53,
      54,    16,    20,    28,    35,    36,    31,    35,    33,    32,
      32,    35,    37,    67,    29,    30,    41,    35,    72,    44,
      45,    46,    47,    48,    49,    50,    51,    67,    68,     3,
       4,    20,    84,    85,    86,    20,    31,    91,     3,     4,
      20,    20,    31,    97,    18,    32,   141,    31,   102,    36,
      35,     5,    32,    32,    27,    28,    96,    96,    35,    36,
     155,    82,    83,   158,     5,     6,     7,   121,    35,   164,
      44,    31,    46,    47,    48,    49,    50,    51,    42,    44,
     175,    46,    47,    48,    49,    50,    51,    43,    33,   143,
      32,    31,    36,    20,    12,   149,   150,   151,   193,   194,
      31,     3,     4,     5,     6,     7,     8,     9,    10,     5,
      12,    13,    14,    15,    35,    17,     5,    19,    21,    22,
      23,    24,    25,    26,    31,    31,    28,   181,   182,    31,
      20,    33,    34,    32,    20,    37,     5,    32,    11,    41,
      33,    35,    44,    45,    46,    47,    48,    49,    50,    51,
       3,     4,     5,     6,     7,     8,     9,     5,     6,     7,
       8,     9,    40,    40,    17,    15,    96,    35,   101,    17,
     105,   159,   138,    74,    -1,    28,   195,    75,    31,    -1,
      28,    -1,    -1,    31,    -1,    -1,    -1,    35,    -1,    -1,
      -1,    44,    -1,    46,    47,    48,    49,    50,    51,     5,
       6,     7,     8,     9,    10,    -1,    12,    13,    14,    15,
      -1,    17,    -1,    19,     5,     6,     7,     8,     9,    -1,
      -1,    -1,    28,    -1,    -1,    31,    17,    33,    -1,    -1,
      -1,    37,    -1,    -1,    -1,    41,    -1,    28,    -1,    -1,
      31,    76,    77,    78,    79,    80,    81
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    53,    54,     0,     3,     4,     5,     6,     7,     8,
       9,    10,    12,    13,    14,    15,    17,    18,    19,    28,
      31,    33,    37,    41,    44,    45,    46,    47,    48,    49,
      50,    51,    55,    56,    57,    61,    62,    69,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    86,    94,
      96,   101,   104,    31,    31,    31,   105,    35,    74,    35,
      80,     5,    35,    80,    74,    70,    87,    31,    58,     5,
      59,    60,    20,    35,    43,    42,    21,    22,    23,    24,
      25,    26,    27,    28,    16,    29,    30,    65,    66,    74,
      74,   102,    69,    35,    64,    32,    71,    31,    61,    74,
      95,    61,    20,    63,    35,    36,    74,    76,    77,    78,
      78,    78,    78,    78,    78,    79,    79,    80,    80,    80,
      32,    36,    83,    74,    12,    31,    34,    56,    61,    72,
      73,    74,     5,    35,    35,     5,    59,    74,    31,    60,
      74,    32,   103,    31,    18,    61,    67,    68,    32,    20,
      97,    99,    35,    67,    69,    32,    74,     5,    32,    36,
      33,    74,    74,    74,    32,    11,    84,    69,    32,    69,
      68,    88,    35,    35,    69,    85,    35,    34,    38,    39,
      90,    98,   100,    69,     5,     6,     7,    93,    40,    74,
      74,    40,    92,    32,    32,    91,    73,    89,    69,    69,
      89,    15,    35
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}

/* Prevent warnings from -Wmissing-prototypes.  */
#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */


/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;



/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{


    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */
  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 10:

/* Line 1455 of yacc.c  */
#line 569 "yacc.y"
    {
    isConst = 1;
;}
    break;

  case 11:

/* Line 1455 of yacc.c  */
#line 571 "yacc.y"
    {
    isConst = 0;
;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 580 "yacc.y"
    { 
    insertSymbol((yyvsp[(1) - (1)].id), currentType, isConst); 
    if(isConstant((yyvsp[(1) - (1)].id)) == 1) {
        semanticError("Constant variables must be initialized");
    }
    ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 586 "yacc.y"
    {
    if (!areTypesCompatible(currentType, (yyvsp[(3) - (3)].exprInfo).type)) {
                char errorMsg[200];
                sprintf(errorMsg, "Type mismatch in initialization of '%s': expected '%s' but got '%s'", (yyvsp[(1) - (3)].id), currentType, (yyvsp[(3) - (3)].exprInfo).type);
                semanticError(errorMsg);
            }
    insertSymbol((yyvsp[(1) - (3)].id), currentType, isConst);
    setInitialized((yyvsp[(1) - (3)].id));  // mark as initialized
    emit("=", (yyvsp[(3) - (3)].exprInfo).name, "", (yyvsp[(1) - (3)].id));
  ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 599 "yacc.y"
    { currentType = "int"; ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 600 "yacc.y"
    { currentType = "float"; ;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 601 "yacc.y"
    { currentType = "char"; ;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 602 "yacc.y"
    { currentType = "double"; ;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 603 "yacc.y"
    { currentType = "bool"; ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 604 "yacc.y"
    { currentType = "long"; ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 605 "yacc.y"
    { currentType = "short"; ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 606 "yacc.y"
    { currentType = "unsigned"; ;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 607 "yacc.y"
    { currentType = "signed"; ;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 611 "yacc.y"
    {
        currentFunction = insertFunction((yyvsp[(2) - (2)].id), currentType);
        emit("function", (yyvsp[(2) - (2)].id), "", "");
        enterScope();  
        hasReturnStatement = 0;
        blockNestingLevel = -1;  // So the function's main block is level 0
    ;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 617 "yacc.y"
    {
        if (strcmp(currentFunction->returnType, "void") != 0 && hasReturnStatement == 0) {
            char errorMsg[200];
            sprintf(errorMsg, "Function '%s' with return type '%s' has no top-level return statement", (yyvsp[(2) - (7)].id), currentFunction->returnType);
            semanticError(errorMsg);
        }
        emit("endFunc", "", "", "");
        exitScope();
        currentFunction = NULL;
    ;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 627 "yacc.y"
    {
        currentFunction = insertFunction((yyvsp[(2) - (2)].id), "void");
        emit("function", (yyvsp[(2) - (2)].id), "", "");
        enterScope();
        hasReturnStatement = 0;
        blockNestingLevel = -1;  // So the function's main block is level 0
    ;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 633 "yacc.y"
    {
        emit("endFunc", "", "", "");
        exitScope();
        currentFunction = NULL;
    ;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 641 "yacc.y"
    {
        currentArgCount = 0;
    ;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 648 "yacc.y"
    {
        emitParameter((yyvsp[(1) - (1)].exprInfo).name);
        currentArgCount = 1; 
    ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 652 "yacc.y"
    {
        emitParameter((yyvsp[(3) - (3)].exprInfo).name);
        currentArgCount++;
    ;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 666 "yacc.y"
    {
        if (currentFunction) {
            addParameter(currentFunction, (yyvsp[(2) - (2)].id), currentType);
        }
    ;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 674 "yacc.y"
    {
        enterScope();
        blockNestingLevel++;
    ;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 677 "yacc.y"
    {        
        blockNestingLevel--;
        exitScope();
        (yyval.exprInfo).name = strdup("");
        (yyval.exprInfo).type = strdup("void");
    ;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 695 "yacc.y"
    {
        (yyval.exprInfo) = (yyvsp[(1) - (2)].exprInfo);
    ;}
    break;

  case 45:

/* Line 1455 of yacc.c  */
#line 698 "yacc.y"
    {
        (yyval.exprInfo) = (yyvsp[(1) - (1)].exprInfo);
    ;}
    break;

  case 46:

/* Line 1455 of yacc.c  */
#line 701 "yacc.y"
    {
        (yyval.exprInfo) = (yyvsp[(1) - (1)].exprInfo);
    ;}
    break;

  case 47:

/* Line 1455 of yacc.c  */
#line 704 "yacc.y"
    {
        (yyval.exprInfo) = (yyvsp[(1) - (1)].exprInfo);
    ;}
    break;

  case 48:

/* Line 1455 of yacc.c  */
#line 707 "yacc.y"
    {
        char* continueLabel = getCurrentContinueLabel();
        emit("goto", "", "", continueLabel);
        (yyval.exprInfo).name = strdup("");
        (yyval.exprInfo).type = strdup("void");
    ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 713 "yacc.y"
    {
        if(!fromSwitch){
            char* breakLabel = getCurrentBreakLabel();
            emit("goto", "", "", breakLabel);
            (yyval.exprInfo).name = strdup("");
            (yyval.exprInfo).type = strdup("void");
        }
        fromSwitch = 0;
    ;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 722 "yacc.y"
    {
    if (currentFunction) {
        if (strcmp(currentFunction->returnType, (yyvsp[(2) - (3)].exprInfo).type) != 0) {
            semanticError("Return type doesn't match function return type");
        }
        
        // Check if this is a nested return
        if (blockNestingLevel > 0) {
            semanticError("Return statement must be at the top level of function body");
        } else {
            hasReturnStatement = 1;
        }
        
        emit("return", (yyvsp[(2) - (3)].exprInfo).name, "", "");
    }
    (yyval.exprInfo).name = strdup("");
    (yyval.exprInfo).type = strdup("void");
;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 740 "yacc.y"
    {
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
    (yyval.exprInfo).name = strdup("");
    (yyval.exprInfo).type = strdup("void");
;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 765 "yacc.y"
    {
        (yyval.exprInfo) = (yyvsp[(1) - (1)].exprInfo);
    ;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 768 "yacc.y"
    {
        (yyvsp[(1) - (3)].exprInfo).isTarget = 1;  


        if ((yyvsp[(3) - (3)].exprInfo).name[0] != 't' && (yyvsp[(3) - (3)].exprInfo).name[0] != '\'' && !isdigit((yyvsp[(3) - (3)].exprInfo).name[0])) {
            checkInitialized((yyvsp[(3) - (3)].exprInfo).name);
        }

        if (!lookup((yyvsp[(1) - (3)].exprInfo).name)) {
            char errorMsg[100];
            sprintf(errorMsg, "Assignment to undeclared variable '%s'", (yyvsp[(1) - (3)].exprInfo).name);
            semanticError(errorMsg);
        }

        if (isConstant((yyvsp[(1) - (3)].exprInfo).name) == 1) {
            semanticError("Cannot modify constant variable");
        }
        
        char* lhs_type = getType((yyvsp[(1) - (3)].exprInfo).name);
        char* rhs_type = (yyvsp[(3) - (3)].exprInfo).type;

        // Only check type mismatch if the variable exists (has a type)
        if (lhs_type != NULL && !areTypesCompatible(lhs_type, rhs_type)) {
        if (lhs_type != NULL && !areTypesCompatible(lhs_type, rhs_type)) {
            semanticError("Type mismatch in assignment");
        }
        
        // Now set a fallback for code generation
        if (lhs_type == NULL) {
            lhs_type = strdup("unknown");
        }

    // Check for i = i + 1 or similar
    if (
        (yyvsp[(3) - (3)].exprInfo).name[0] == 't' &&  // it's a temp
        quadIndex >= 1 &&
        (
            (strcmp(quads[quadIndex - 1].op, "+") == 0 ||
             strcmp(quads[quadIndex - 1].op, "-") == 0 ||
             strcmp(quads[quadIndex - 1].op, "*") == 0 ||
             strcmp(quads[quadIndex - 1].op, "/") == 0)
        ) &&
        strcmp(quads[quadIndex - 1].result, (yyvsp[(3) - (3)].exprInfo).name) == 0 &&
        strcmp(quads[quadIndex - 1].arg1, (yyvsp[(1) - (3)].exprInfo).name) == 0
    ) {
        // Overwrite previous temp op with compound assignment
        char compound[4];
        sprintf(compound, "%s=", quads[quadIndex - 1].op);
        strcpy(quads[quadIndex - 1].op, compound);
        strcpy(quads[quadIndex - 1].result, (yyvsp[(1) - (3)].exprInfo).name);
        quadIndex--;  // remove redundant temp result
        emit(compound, (yyvsp[(1) - (3)].exprInfo).name, quads[quadIndex].arg2, (yyvsp[(1) - (3)].exprInfo).name);  // regenerate as compound
        setInitialized((yyvsp[(1) - (3)].exprInfo).name);  // mark as initialized
    } else {
        emit("=", (yyvsp[(3) - (3)].exprInfo).name, "", (yyvsp[(1) - (3)].exprInfo).name);
        setInitialized((yyvsp[(1) - (3)].exprInfo).name);  // mark as initialized
    }

        (yyval.exprInfo).name = (yyvsp[(1) - (3)].exprInfo).name;
        (yyval.exprInfo).type = lhs_type;
}
    ;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 833 "yacc.y"
    {
        (yyval.exprInfo) = (yyvsp[(1) - (1)].exprInfo);
    ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 836 "yacc.y"
    {
        char* temp = newTemp();
        emit("||", (yyvsp[(1) - (3)].exprInfo).name, (yyvsp[(3) - (3)].exprInfo).name, temp);
        (yyval.exprInfo).name = temp;
        (yyval.exprInfo).type = strdup("bool");  // result of logical OR is always bool
    ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 845 "yacc.y"
    {
        (yyval.exprInfo) = (yyvsp[(1) - (1)].exprInfo);
    ;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 848 "yacc.y"
    {
        char* temp = newTemp();
        emit("&&", (yyvsp[(1) - (3)].exprInfo).name, (yyvsp[(3) - (3)].exprInfo).name, temp);
        (yyval.exprInfo).name = temp;
        (yyval.exprInfo).type = strdup("bool");  
    ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 857 "yacc.y"
    {
        (yyval.exprInfo) = (yyvsp[(1) - (1)].exprInfo);
    ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 860 "yacc.y"
    {
        char* temp = newTemp();
        emit("==", (yyvsp[(1) - (3)].exprInfo).name, (yyvsp[(3) - (3)].exprInfo).name, temp);
        (yyval.exprInfo).name = temp;
        (yyval.exprInfo).type = strdup("bool");
    ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 866 "yacc.y"
    {
        char* temp = newTemp();
        emit("!=", (yyvsp[(1) - (3)].exprInfo).name, (yyvsp[(3) - (3)].exprInfo).name, temp);
        (yyval.exprInfo).name = temp;
        (yyval.exprInfo).type = strdup("bool");
    ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 872 "yacc.y"
    {
        char* temp = newTemp();
        emit("<", (yyvsp[(1) - (3)].exprInfo).name, (yyvsp[(3) - (3)].exprInfo).name, temp);
        (yyval.exprInfo).name = temp;
        (yyval.exprInfo).type = strdup("bool");
    ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 878 "yacc.y"
    {
        char* temp = newTemp();
        emit(">", (yyvsp[(1) - (3)].exprInfo).name, (yyvsp[(3) - (3)].exprInfo).name, temp);
        (yyval.exprInfo).name = temp;
        (yyval.exprInfo).type = strdup("bool");
    ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 884 "yacc.y"
    {
        char* temp = newTemp();
        emit("<=", (yyvsp[(1) - (3)].exprInfo).name, (yyvsp[(3) - (3)].exprInfo).name, temp);
        (yyval.exprInfo).name = temp;
        (yyval.exprInfo).type = strdup("bool");
    ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 890 "yacc.y"
    {
        char* temp = newTemp();
        emit(">=", (yyvsp[(1) - (3)].exprInfo).name, (yyvsp[(3) - (3)].exprInfo).name, temp);
        (yyval.exprInfo).name = temp;
        (yyval.exprInfo).type = strdup("bool");
    ;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 899 "yacc.y"
    {
        (yyval.exprInfo) = (yyvsp[(1) - (1)].exprInfo);
    ;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 902 "yacc.y"
    {
         if ((yyvsp[(1) - (3)].exprInfo).name[0] != 't' && (yyvsp[(1) - (3)].exprInfo).name[0] != '\'' && !isdigit((yyvsp[(1) - (3)].exprInfo).name[0])) {
            checkInitialized((yyvsp[(1) - (3)].exprInfo).name);
        }
        if ((yyvsp[(3) - (3)].exprInfo).name[0] != 't' && (yyvsp[(3) - (3)].exprInfo).name[0] != '\'' && !isdigit((yyvsp[(3) - (3)].exprInfo).name[0])) {
            checkInitialized((yyvsp[(3) - (3)].exprInfo).name);
        }
        
        char* temp = newTemp();
        emit("+", (yyvsp[(1) - (3)].exprInfo).name, (yyvsp[(3) - (3)].exprInfo).name, temp);
        (yyval.exprInfo).name = temp;
        (yyval.exprInfo).type = resolveType((yyvsp[(1) - (3)].exprInfo).type, (yyvsp[(3) - (3)].exprInfo).type);   
    ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 915 "yacc.y"
    {
        if ((yyvsp[(1) - (3)].exprInfo).name[0] != 't' && (yyvsp[(1) - (3)].exprInfo).name[0] != '\'' && !isdigit((yyvsp[(1) - (3)].exprInfo).name[0])) {
            checkInitialized((yyvsp[(1) - (3)].exprInfo).name);
        }
        if ((yyvsp[(3) - (3)].exprInfo).name[0] != 't' && (yyvsp[(3) - (3)].exprInfo).name[0] != '\'' && !isdigit((yyvsp[(3) - (3)].exprInfo).name[0])) {
            checkInitialized((yyvsp[(3) - (3)].exprInfo).name);
        }
        
        char* temp = newTemp();
        emit("-", (yyvsp[(1) - (3)].exprInfo).name, (yyvsp[(3) - (3)].exprInfo).name, temp);
        (yyval.exprInfo).name = temp;
        (yyval.exprInfo).type = resolveType((yyvsp[(1) - (3)].exprInfo).type, (yyvsp[(3) - (3)].exprInfo).type);
    ;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 933 "yacc.y"
    {
        if ((yyvsp[(1) - (3)].exprInfo).name[0] != 't' && (yyvsp[(1) - (3)].exprInfo).name[0] != '\'' && !isdigit((yyvsp[(1) - (3)].exprInfo).name[0])) {
            checkInitialized((yyvsp[(1) - (3)].exprInfo).name);
        }
        if ((yyvsp[(3) - (3)].exprInfo).name[0] != 't' && (yyvsp[(3) - (3)].exprInfo).name[0] != '\'' && !isdigit((yyvsp[(3) - (3)].exprInfo).name[0])) {
            checkInitialized((yyvsp[(3) - (3)].exprInfo).name);
        }
        
        char* temp = newTemp();
        emit("*", (yyvsp[(1) - (3)].exprInfo).name, (yyvsp[(3) - (3)].exprInfo).name, temp);
        (yyval.exprInfo).name = temp;
        (yyval.exprInfo).type = resolveType((yyvsp[(1) - (3)].exprInfo).type, (yyvsp[(3) - (3)].exprInfo).type);
    ;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 946 "yacc.y"
    {
        if ((yyvsp[(1) - (3)].exprInfo).name[0] != 't' && (yyvsp[(1) - (3)].exprInfo).name[0] != '\'' && !isdigit((yyvsp[(1) - (3)].exprInfo).name[0])) {
            checkInitialized((yyvsp[(1) - (3)].exprInfo).name);
        }
        if ((yyvsp[(3) - (3)].exprInfo).name[0] != 't' && (yyvsp[(3) - (3)].exprInfo).name[0] != '\'' && !isdigit((yyvsp[(3) - (3)].exprInfo).name[0])) {
            checkInitialized((yyvsp[(3) - (3)].exprInfo).name);
        }
        
        char* temp = newTemp();
        emit("/", (yyvsp[(1) - (3)].exprInfo).name, (yyvsp[(3) - (3)].exprInfo).name, temp);
        (yyval.exprInfo).name = temp;
        (yyval.exprInfo).type = resolveType((yyvsp[(1) - (3)].exprInfo).type, (yyvsp[(3) - (3)].exprInfo).type);
    ;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 959 "yacc.y"
    {
        char* temp = newTemp();
        emit("%", (yyvsp[(1) - (3)].exprInfo).name, (yyvsp[(3) - (3)].exprInfo).name, temp);
        (yyval.exprInfo).name = temp;
        (yyval.exprInfo).type = resolveType((yyvsp[(1) - (3)].exprInfo).type, (yyvsp[(3) - (3)].exprInfo).type);
    ;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 965 "yacc.y"
    {(yyval.exprInfo)=(yyvsp[(1) - (1)].exprInfo);;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 969 "yacc.y"
    {(yyval.exprInfo)=(yyvsp[(1) - (1)].exprInfo);;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 970 "yacc.y"
    {
        char* temp = newTemp();
        emit("!", (yyvsp[(2) - (2)].exprInfo).name, "", temp);
        (yyval.exprInfo).name = temp;
        (yyval.exprInfo).type = (yyvsp[(2) - (2)].exprInfo).type;
    ;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 976 "yacc.y"
    {
        (yyval.exprInfo) = (yyvsp[(2) - (3)].exprInfo);
    ;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 979 "yacc.y"
    {
        char* temp = newTemp();
        emit("uminus", (yyvsp[(2) - (2)].exprInfo).name, "", temp);
        (yyval.exprInfo).name = temp;
        (yyval.exprInfo).type = (yyvsp[(2) - (2)].exprInfo).type;
    ;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 988 "yacc.y"
    { 
    if (!lookup((yyvsp[(1) - (1)].id))) {
        semanticError("Undeclared identifier used in expression");
    }
    (yyval.exprInfo).name = (yyvsp[(1) - (1)].id);
    (yyval.exprInfo).type = getType((yyvsp[(1) - (1)].id));
    (yyval.exprInfo).isTarget = 0;  // Default: not a target
    ;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 996 "yacc.y"
    {
    char val[4]; sprintf(val, "'%c'", (yyvsp[(1) - (1)].c));
    (yyval.exprInfo).name = strdup(val);
    (yyval.exprInfo).type = strdup("char");
    ;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 1001 "yacc.y"
    {
    char val[20]; sprintf(val, "%f", (yyvsp[(1) - (1)].f));
    (yyval.exprInfo).name = strdup(val);
    (yyval.exprInfo).type = strdup("float");
    ;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 1006 "yacc.y"
    {
    char val[20]; sprintf(val, "%d", (yyvsp[(1) - (1)].i));
    (yyval.exprInfo).name = strdup(val);
    (yyval.exprInfo).type = strdup("int");
    ;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 1011 "yacc.y"
    {
    char val[10]; sprintf(val, "%d", (yyvsp[(1) - (1)].i));
    (yyval.exprInfo).name = strdup(val);
    (yyval.exprInfo).type = strdup("bool");
    ;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 1016 "yacc.y"
    {
        Symbol* func = lookupFunction((yyvsp[(1) - (4)].id));
        if (!func) {
            char errorMsg[100];
            sprintf(errorMsg, "Undefined function '%s'", (yyvsp[(1) - (4)].id));
            yyerror(errorMsg);
            (yyval.exprInfo).name = newTemp();
            (yyval.exprInfo).type = strdup("unknown");
        } else {
            // Validate argument count
            if (validateFunctionCall(func, currentArgCount)) {
                (yyval.exprInfo).name = generateFunctionCall((yyvsp[(1) - (4)].id), currentArgCount);
                (yyval.exprInfo).type = strdup(func->returnType);
            } else {
                (yyval.exprInfo).name = newTemp();
                (yyval.exprInfo).type = strdup(func->returnType);
            }
        }
    ;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 1039 "yacc.y"
    {
        char* elseLabel = newLabel();
        emit("ifFalseGoto", (yyvsp[(3) - (3)].exprInfo).name, "", elseLabel);
        push(elseLabel);
    ;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 1048 "yacc.y"
    {
        // Get the else label and place it here
        // Generate label for the end of the entire if-else
        char* elseLabel = pop();
        char* endLabel = newLabel();
        emit("goto", "", "", endLabel);
        emit("label", "", "", elseLabel);
        push(endLabel);
    ;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 1056 "yacc.y"
    {
        char* endLabel = pop();
        emit("label", "", "", endLabel);
    ;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 1060 "yacc.y"
    {
        // Get the else label and place it here
        char* elseLabel = pop();
        emit("label", "", "", elseLabel);
    ;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 1067 "yacc.y"
    {
    char* switchEndLabel = newLabel();
    push(switchEndLabel);
;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 1070 "yacc.y"
    {
    // if expression is true
    char* switchTemp = newTemp();
    emit("=", (yyvsp[(4) - (8)].exprInfo).name, "", switchTemp);
    emitCases((yyvsp[(7) - (8)].caseLabel), switchTemp);
    char* switchEndLabel = pop();
    emit("label", "", "", switchEndLabel);           
;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 1079 "yacc.y"
    {(yyval.caseLabel) = NULL;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 1079 "yacc.y"
    {
    CaseLabel* q = (yyvsp[(1) - (2)].caseLabel);
    if(!q) (yyval.caseLabel) = (yyvsp[(2) - (2)].caseLabel);
    else{
        while(q->next) q = q->next;
        q->next = (yyvsp[(2) - (2)].caseLabel);
        (yyval.caseLabel) = (yyvsp[(1) - (2)].caseLabel);
    }
;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 1092 "yacc.y"
    {
                char* label = newLabel();
                emit("label", "", "", label);
                //emit statement code;
                push(label);
                fromSwitch = 1;
                ;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 1099 "yacc.y"
    {
                char* label = pop();
                char* end = pop();
                
                emit("goto", "", "", end);
                push(end);
                (yyval.caseLabel) = newCaseLabel((yyvsp[(2) - (5)].id), label);
                fromSwitch = 0;

            ;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 1110 "yacc.y"
    {
                char* label = newLabel();
                emit("label", "", "", label);
                // emit statement code
                push(label);
                fromSwitch = 1;
            ;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 1116 "yacc.y"
    {
                char* label = pop();
                char* end = pop();
                emit("goto", "", "", end);
                (yyval.caseLabel) = newCaseLabel("default", label);
                push(end);
                fromSwitch = 0;

            ;}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 1127 "yacc.y"
    {
    char buffer[20];
    sprintf(buffer, "%d", (yyvsp[(1) - (1)].i));
    (yyval.id) = strdup(buffer);
;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 1132 "yacc.y"
    {
    char buffer[20];
    sprintf(buffer, "%f", (yyvsp[(1) - (1)].f));
    (yyval.id) = strdup(buffer);
;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 1137 "yacc.y"
    {
    if(!lookup((yyvsp[(1) - (1)].id))){
        semanticError("Undeclared identifier used as a constant for switch\n");
    }
    (yyval.id) = strdup((yyvsp[(1) - (1)].id));
;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 1146 "yacc.y"
    {
        (yyval.exprInfo) = (yyvsp[(1) - (1)].exprInfo);
    ;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 1149 "yacc.y"
    {
        (yyval.exprInfo) = (yyvsp[(1) - (1)].exprInfo);
    ;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 1152 "yacc.y"
    {
        (yyval.exprInfo) = (yyvsp[(1) - (1)].exprInfo);
    ;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 1157 "yacc.y"
    {
        insertSymbol((yyvsp[(2) - (4)].id), currentType, isConst);
        emit("=", (yyvsp[(4) - (4)].exprInfo).name, "", (yyvsp[(2) - (4)].id));
        setInitialized((yyvsp[(2) - (4)].id));  // mark as initialized
        (yyval.exprInfo).name = (yyvsp[(2) - (4)].id);
        (yyval.exprInfo).type = currentType;
    ;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 1166 "yacc.y"
    {
        char* start = newLabel();
        char* end = newLabel();
        pushLoopLabels(start, end);
        emit("label", "", "", start);
        push(start); push(end);
    ;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 1173 "yacc.y"
    {
        char* end = pop();
        emit("ifFalseGoTo", (yyvsp[(6) - (7)].exprInfo).name, "", end);
        push(end);
        for_start = quadIndex;
    ;}
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 1179 "yacc.y"
    {
        moveOneToEnd(quadIndex, for_start);
        char* end = pop();
        char* start = pop();
        popLoopLabels();
        emit("goto", "", "", start);
        emit("label", "", "", end);
    ;}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 1187 "yacc.y"
    {
        char* start = newLabel();
        char* end = newLabel();
        pushLoopLabels(start, end);
        emit("label", "", "", start);
        push(start); push(end);
    ;}
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 1194 "yacc.y"
    {
        char* end = pop();
        emit("ifFalseGoTo", (yyvsp[(6) - (7)].exprInfo).name, "", end);
        push(end);
        for_start = quadIndex;
    ;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 1200 "yacc.y"
    {
        moveOneToEnd(quadIndex, for_start);       
        char* end = pop();
        char* start = pop();
        popLoopLabels();
        emit("goto", "", "", start);
        emit("label", "", "", end);
    ;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 1210 "yacc.y"
    {
    char* start = newLabel();
    emit("label", "", "", start);
    push(start);
;}
    break;

  case 112:

/* Line 1455 of yacc.c  */
#line 1214 "yacc.y"
    {
    char* start = pop();
    char* end = newLabel();
    pushLoopLabels(start, end);
    push(start);
    push(end);
    emit("ifFalseGoTo", (yyvsp[(4) - (4)].exprInfo).name, "", end);

;}
    break;

  case 113:

/* Line 1455 of yacc.c  */
#line 1222 "yacc.y"
    {
    char* end = pop();
    char* start = pop();
    // emit condition expression
    popLoopLabels();
    // body
    emit("goto", "", "", start);
    emit("label", "", "", end);
;}
    break;

  case 114:

/* Line 1455 of yacc.c  */
#line 1232 "yacc.y"
    {
        char* start = newLabel();
        char* end = newLabel();
        emit("label", "", "", start);
        push(start); push(end);
        pushLoopLabels(start, end);

    ;}
    break;

  case 115:

/* Line 1455 of yacc.c  */
#line 1240 "yacc.y"
    {
        char* end = pop();
        char* start = pop();
        // emit code for block
        // emit code for condition
        emit("ifFalseGoTo", (yyvsp[(6) - (8)].exprInfo).name, "", end);
        emit("goto", "", "", start);
        emit("label", "", "", end);
        popLoopLabels();
        (yyval.exprInfo).name = strdup("");
        (yyval.exprInfo).type = strdup("void");
    ;}
    break;



/* Line 1455 of yacc.c  */
#line 3222 "yacc.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



/* Line 1675 of yacc.c  */
#line 1253 "yacc.y"



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
