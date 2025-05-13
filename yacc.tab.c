
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
} Symbol;

Symbol* symbolTable = NULL;
char* currentType = NULL;
int isConst = 0;
int currentScopeLevel = 0;  //Scope level management
Symbol* functionTable = NULL;   // Function table for storing function symbols
Symbol* currentFunction = NULL; // Current function being processed
int currentArgCount = 0;

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
    // === ERROR HANDLING ===
    extern int yylineno;
    extern char* yytext;
    extern FILE *yyin;
    int yylex(void);
    void yyerror(char* s) {
    fprintf(stderr, "Syntax Error: %s \n", s);
}
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



/* Line 189 of yacc.c  */
#line 374 "yacc.tab.c"

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
     STRING = 261,
     INT_LITERAL = 262,
     FLOAT_LITERAL = 263,
     CHAR_LITERAL = 264,
     STRING_LITERAL = 265,
     BOOL_LITERAL = 266,
     IF = 267,
     ELSE = 268,
     WHILE = 269,
     DO = 270,
     RETURN = 271,
     BREAK = 272,
     MOD = 273,
     NOT = 274,
     VOID = 275,
     CONTINUE = 276,
     ASSIGN = 277,
     EQ = 278,
     NEQ = 279,
     LE = 280,
     GE = 281,
     LT = 282,
     GT = 283,
     PLUS = 284,
     MINUS = 285,
     MUL = 286,
     DIV = 287,
     LPAREN = 288,
     RPAREN = 289,
     LBRACE = 290,
     RBRACE = 291,
     SEMI = 292,
     COMMA = 293,
     SWITCH = 294,
     CASE = 295,
     DEFAULT = 296,
     COLON = 297,
     FOR = 298,
     AND = 299,
     OR = 300,
     CHAR = 301,
     CONST = 302,
     DOUBLE = 303,
     BOOL = 304,
     SHORT = 305,
     LONG = 306,
     UNSIGNED = 307,
     SIGNED = 308
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 214 of yacc.c  */
#line 301 "yacc.y"

    int i;
    float f;
    char c;
    char* id;
    struct CaseLabel* caseLabel;
    struct {
        char* name;  // name of the result  (e.g., temp variable name)
        char* type;  // type of the result (e.g., "int", "float")
    } exprInfo;     // for full expression information



/* Line 214 of yacc.c  */
#line 477 "yacc.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 489 "yacc.tab.c"

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
#define YYLAST   297

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  54
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  38
/* YYNRULES -- Number of rules.  */
#define YYNRULES  100
/* YYNRULES -- Number of states.  */
#define YYNSTATES  185

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   308

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
      45,    46,    47,    48,    49,    50,    51,    52,    53
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     6,     9,    11,    13,    15,    19,
      21,    26,    28,    32,    34,    38,    40,    42,    44,    46,
      48,    50,    52,    54,    56,    58,    59,    67,    68,    76,
      77,    79,    81,    85,    86,    88,    90,    94,    97,    98,
     103,   104,   107,   109,   111,   114,   116,   118,   120,   123,
     126,   130,   133,   135,   139,   141,   145,   147,   151,   153,
     157,   161,   165,   169,   173,   177,   179,   183,   187,   191,
     195,   199,   201,   203,   206,   210,   213,   215,   217,   219,
     221,   223,   225,   230,   236,   244,   246,   254,   255,   258,
     263,   267,   269,   271,   273,   275,   277,   279,   289,   300,
     306
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      55,     0,    -1,    56,    -1,    -1,    56,    57,    -1,    63,
      -1,    58,    -1,    74,    -1,    62,    60,    37,    -1,    59,
      -1,    47,    62,    60,    37,    -1,    61,    -1,    60,    38,
      61,    -1,     5,    -1,     5,    22,    75,    -1,     3,    -1,
       4,    -1,    46,    -1,    48,    -1,    49,    -1,     6,    -1,
      51,    -1,    50,    -1,    52,    -1,    53,    -1,    -1,    62,
       5,    64,    33,    68,    34,    70,    -1,    -1,    20,     5,
      65,    33,    68,    34,    70,    -1,    -1,    67,    -1,    75,
      -1,    67,    38,    75,    -1,    -1,    20,    -1,    69,    -1,
      68,    38,    69,    -1,    62,     5,    -1,    -1,    35,    71,
      72,    36,    -1,    -1,    72,    73,    -1,    58,    -1,    74,
      -1,    75,    37,    -1,    83,    -1,    88,    -1,    70,    -1,
      21,    37,    -1,    17,    37,    -1,    16,    75,    37,    -1,
      16,    37,    -1,    76,    -1,    75,    22,    75,    -1,    77,
      -1,    76,    45,    77,    -1,    78,    -1,    77,    44,    78,
      -1,    79,    -1,    78,    23,    79,    -1,    78,    24,    79,
      -1,    78,    27,    79,    -1,    78,    28,    79,    -1,    78,
      25,    79,    -1,    78,    26,    79,    -1,    80,    -1,    79,
      29,    80,    -1,    79,    30,    80,    -1,    80,    31,    81,
      -1,    80,    32,    81,    -1,    80,    18,    81,    -1,    81,
      -1,    82,    -1,    19,    81,    -1,    33,    75,    34,    -1,
      30,    81,    -1,     5,    -1,    10,    -1,     9,    -1,     8,
      -1,     7,    -1,    11,    -1,     5,    33,    66,    34,    -1,
      12,    33,    75,    34,    70,    -1,    12,    33,    75,    34,
      70,    13,    70,    -1,    84,    -1,    39,    33,    75,    34,
      35,    85,    36,    -1,    -1,    85,    86,    -1,    40,    87,
      42,    74,    -1,    41,    42,    74,    -1,     7,    -1,     8,
      -1,     5,    -1,    89,    -1,    90,    -1,    91,    -1,    43,
      33,    75,    37,    75,    37,    75,    34,    70,    -1,    43,
      33,     3,    75,    37,    75,    37,    75,    34,    70,    -1,
      14,    33,    75,    34,    70,    -1,    15,    70,    14,    33,
      75,    34,    37,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   356,   356,   359,   361,   365,   366,   367,   371,   372,
     375,   380,   381,   384,   385,   392,   393,   394,   395,   396,
     397,   398,   399,   400,   401,   405,   405,   414,   414,   426,
     429,   433,   437,   443,   445,   446,   447,   451,   459,   459,
     466,   468,   472,   473,   477,   480,   481,   482,   483,   484,
     485,   486,   490,   493,   505,   508,   517,   520,   529,   532,
     538,   544,   550,   556,   562,   571,   574,   580,   590,   596,
     602,   608,   612,   613,   619,   622,   626,   631,   637,   642,
     647,   652,   657,   679,   685,   695,   698,   705,   705,   715,
     721,   729,   734,   739,   748,   749,   750,   753,   766,   779,
     790
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "INT", "FLOAT", "ID", "STRING",
  "INT_LITERAL", "FLOAT_LITERAL", "CHAR_LITERAL", "STRING_LITERAL",
  "BOOL_LITERAL", "IF", "ELSE", "WHILE", "DO", "RETURN", "BREAK", "MOD",
  "NOT", "VOID", "CONTINUE", "ASSIGN", "EQ", "NEQ", "LE", "GE", "LT", "GT",
  "PLUS", "MINUS", "MUL", "DIV", "LPAREN", "RPAREN", "LBRACE", "RBRACE",
  "SEMI", "COMMA", "SWITCH", "CASE", "DEFAULT", "COLON", "FOR", "AND",
  "OR", "CHAR", "CONST", "DOUBLE", "BOOL", "SHORT", "LONG", "UNSIGNED",
  "SIGNED", "$accept", "program", "external_list", "external",
  "declaration", "const_decl", "var_list", "init_declarator", "type",
  "function_definition", "$@1", "$@2", "argument_list",
  "argument_sequence", "parameter_list", "parameter_declaration", "block",
  "$@3", "block_items", "block_item", "statement", "expression",
  "logical_or_expression", "logical_and_expression",
  "comparison_expression", "mathematical_expression", "term", "factor",
  "primary_expression", "conditional_statement", "switch_statement",
  "switch_case_list", "switch_case", "constant", "loops", "for_loop",
  "while_loop", "do_while_loop", 0
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
     305,   306,   307,   308
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    54,    55,    56,    56,    57,    57,    57,    58,    58,
      59,    60,    60,    61,    61,    62,    62,    62,    62,    62,
      62,    62,    62,    62,    62,    64,    63,    65,    63,    66,
      66,    67,    67,    68,    68,    68,    68,    69,    71,    70,
      72,    72,    73,    73,    74,    74,    74,    74,    74,    74,
      74,    74,    75,    75,    76,    76,    77,    77,    78,    78,
      78,    78,    78,    78,    78,    79,    79,    79,    80,    80,
      80,    80,    81,    81,    81,    81,    82,    82,    82,    82,
      82,    82,    82,    83,    83,    83,    84,    85,    85,    86,
      86,    87,    87,    87,    88,    88,    88,    89,    89,    90,
      91
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     0,     2,     1,     1,     1,     3,     1,
       4,     1,     3,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     0,     7,     0,     7,     0,
       1,     1,     3,     0,     1,     1,     3,     2,     0,     4,
       0,     2,     1,     1,     2,     1,     1,     1,     2,     2,
       3,     2,     1,     3,     1,     3,     1,     3,     1,     3,
       3,     3,     3,     3,     3,     1,     3,     3,     3,     3,
       3,     1,     1,     2,     3,     2,     1,     1,     1,     1,
       1,     1,     4,     5,     7,     1,     7,     0,     2,     4,
       3,     1,     1,     1,     1,     1,     1,     9,    10,     5,
       7
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,     2,     1,    15,    16,    76,    20,    80,    79,
      78,    77,    81,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    38,     0,     0,    17,     0,    18,    19,
      22,    21,    23,    24,     4,     6,     9,     0,     5,    47,
       7,     0,    52,    54,    56,    58,    65,    71,    72,    45,
      85,    46,    94,    95,    96,    29,     0,     0,     0,    51,
       0,    49,    73,    27,    48,    75,     0,    40,     0,     0,
       0,    13,     0,    11,     0,    44,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      30,    31,     0,     0,     0,    50,     0,    74,     0,     0,
       0,     0,    13,     0,     0,     0,     8,     0,    53,    55,
      57,    59,    60,    63,    64,    61,    62,    66,    67,    70,
      68,    69,    82,     0,     0,     0,     0,    33,    39,    42,
       0,    41,    43,     0,     0,     0,    10,    14,    33,    12,
      32,    83,    99,     0,    34,     0,     0,    35,    87,     0,
       0,     0,     0,     0,    37,     0,     0,     0,     0,     0,
       0,    84,   100,    28,    36,    86,     0,     0,    88,     0,
       0,    26,    93,    91,    92,     0,     0,     0,     0,     0,
      90,     0,    97,    89,    98
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,    34,    35,    36,    72,    73,   145,    38,
     105,    96,    89,    90,   146,   147,    39,    67,    98,   131,
      40,    41,    42,    43,    44,    45,    46,    47,    48,    49,
      50,   157,   168,   175,    51,    52,    53,    54
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -97
static const yytype_int16 yypact[] =
{
     -97,    17,   164,   -97,   -97,   -97,   -10,   -97,   -97,   -97,
     -97,   -97,   -97,    15,    51,    36,     5,    68,   264,    82,
      71,   264,   264,   -97,    78,    81,   -97,    50,   -97,   -97,
     -97,   -97,   -97,   -97,   -97,   -97,   -97,   126,   -97,   -97,
     -97,   -19,    91,   103,   163,   -21,    19,   -97,   -97,   -97,
     -97,   -97,   -97,   -97,   -97,   264,   264,   264,   125,   -97,
     -16,   -97,   -97,   -97,   -97,   -97,    25,   -97,   264,    22,
     148,    55,     2,   -97,   264,   -97,   264,   264,   264,   264,
     264,   264,   264,   264,   264,   264,   264,   264,   264,   121,
     118,   135,    48,    56,   127,   -97,   128,   -97,   215,    57,
     264,     6,   137,    97,   264,   129,   -97,   148,   135,   103,
     163,   -21,   -21,   -21,   -21,   -21,   -21,    19,    19,   -97,
     -97,   -97,   -97,   264,    36,    36,   264,    16,   -97,   -97,
     148,   -97,   -97,   130,    12,   264,   -97,   135,    16,   -97,
     135,   151,   -97,    70,   -97,   172,    79,   -97,   -97,   264,
      38,    95,    36,   145,   -97,    36,    50,    45,    39,   264,
      36,   -97,   -97,   -97,   -97,   -97,   144,   150,   -97,   264,
      72,   -97,   -97,   -97,   -97,   153,   111,    73,    36,   111,
     -97,    36,   -97,   -97,   -97
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -97,   -97,   -97,   -97,    98,   -97,   123,    93,    -1,   -97,
     -97,   -97,   -97,   -97,    60,    46,   -15,   -97,   -97,   -97,
     -96,   -11,   -97,   132,   124,   159,    58,   -14,   -97,   -97,
     -97,   -97,   -97,   -97,   -97,   -97,   -97,   -97
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -26
static const yytype_int16 yytable[] =
{
      58,    37,   132,    74,    62,    60,    74,    65,    84,    85,
       6,    66,     8,     9,    10,    11,    12,     3,    75,     4,
       5,    95,     7,    55,    18,   100,    70,     6,    74,     8,
       9,    10,    11,    12,    74,    21,   144,    86,    22,   106,
     107,    18,    59,   135,    91,    92,    93,    74,    56,   149,
      87,    88,    21,     4,     5,    22,     7,    99,   101,    97,
      74,    74,    26,   108,    28,    29,    30,    31,    32,    33,
      74,    23,   119,   120,   121,   159,   169,   104,    74,    74,
     180,   165,   124,   183,    57,   166,   167,    63,   -25,   134,
     125,   133,    74,   137,    74,    74,    26,   130,    28,    29,
      30,    31,    32,    33,   153,    61,   178,   181,    64,   141,
     142,    68,   140,   155,    69,   143,     6,   156,     8,     9,
      10,    11,    12,    13,   150,    14,    15,    16,    17,   160,
      18,    71,    20,   156,   136,   107,    76,   161,   158,    94,
     163,    21,   117,   118,    22,   171,    23,    77,   170,   172,
      24,   173,   174,   102,    25,   122,   123,    74,   177,   104,
     126,   127,   138,   182,   152,   148,   184,     4,     5,     6,
       7,     8,     9,    10,    11,    12,    13,   154,    14,    15,
      16,    17,   162,    18,    19,    20,    78,    79,    80,    81,
      82,    83,   176,   103,    21,   179,   129,    22,   151,    23,
     139,   110,   164,    24,     0,     0,     0,    25,   109,     0,
      26,    27,    28,    29,    30,    31,    32,    33,     4,     5,
       6,     7,     8,     9,    10,    11,    12,    13,     0,    14,
      15,    16,    17,     0,    18,     0,    20,   111,   112,   113,
     114,   115,   116,     0,     0,    21,     0,     0,    22,     0,
      23,   128,     0,     0,    24,     0,     0,     0,    25,     0,
       0,    26,    27,    28,    29,    30,    31,    32,    33,     6,
       0,     8,     9,    10,    11,    12,     0,     0,     0,     0,
       0,     0,     0,    18,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    21,     0,     0,    22
};

static const yytype_int16 yycheck[] =
{
      15,     2,    98,    22,    18,    16,    22,    21,    29,    30,
       5,    22,     7,     8,     9,    10,    11,     0,    37,     3,
       4,    37,     6,    33,    19,     3,    27,     5,    22,     7,
       8,     9,    10,    11,    22,    30,    20,    18,    33,    37,
      38,    19,    37,    37,    55,    56,    57,    22,    33,    37,
      31,    32,    30,     3,     4,    33,     6,    68,    69,    34,
      22,    22,    46,    74,    48,    49,    50,    51,    52,    53,
      22,    35,    86,    87,    88,    37,    37,    22,    22,    22,
     176,    36,    34,   179,    33,    40,    41,     5,    33,   100,
      34,    34,    22,   104,    22,    22,    46,    98,    48,    49,
      50,    51,    52,    53,    34,    37,    34,    34,    37,   124,
     125,    33,   123,    34,    33,   126,     5,    38,     7,     8,
       9,    10,    11,    12,   135,    14,    15,    16,    17,    34,
      19,     5,    21,    38,    37,    38,    45,   152,   149,    14,
     155,    30,    84,    85,    33,   160,    35,    44,   159,     5,
      39,     7,     8,     5,    43,    34,    38,    22,   169,    22,
      33,    33,    33,   178,    13,    35,   181,     3,     4,     5,
       6,     7,     8,     9,    10,    11,    12,     5,    14,    15,
      16,    17,    37,    19,    20,    21,    23,    24,    25,    26,
      27,    28,    42,    70,    30,    42,    98,    33,   138,    35,
     107,    77,   156,    39,    -1,    -1,    -1,    43,    76,    -1,
      46,    47,    48,    49,    50,    51,    52,    53,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    -1,    14,
      15,    16,    17,    -1,    19,    -1,    21,    78,    79,    80,
      81,    82,    83,    -1,    -1,    30,    -1,    -1,    33,    -1,
      35,    36,    -1,    -1,    39,    -1,    -1,    -1,    43,    -1,
      -1,    46,    47,    48,    49,    50,    51,    52,    53,     5,
      -1,     7,     8,     9,    10,    11,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    19,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    30,    -1,    -1,    33
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    55,    56,     0,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    14,    15,    16,    17,    19,    20,
      21,    30,    33,    35,    39,    43,    46,    47,    48,    49,
      50,    51,    52,    53,    57,    58,    59,    62,    63,    70,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    88,    89,    90,    91,    33,    33,    33,    70,    37,
      75,    37,    81,     5,    37,    81,    75,    71,    33,    33,
      62,     5,    60,    61,    22,    37,    45,    44,    23,    24,
      25,    26,    27,    28,    29,    30,    18,    31,    32,    66,
      67,    75,    75,    75,    14,    37,    65,    34,    72,    75,
       3,    75,     5,    60,    22,    64,    37,    38,    75,    77,
      78,    79,    79,    79,    79,    79,    79,    80,    80,    81,
      81,    81,    34,    38,    34,    34,    33,    33,    36,    58,
      62,    73,    74,    34,    75,    37,    37,    75,    33,    61,
      75,    70,    70,    75,    20,    62,    68,    69,    35,    37,
      75,    68,    13,    34,     5,    34,    38,    85,    75,    37,
      34,    70,    37,    70,    69,    36,    40,    41,    86,    37,
      75,    70,     5,     7,     8,    87,    42,    75,    34,    42,
      74,    34,    70,    74,    70
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
        case 8:

/* Line 1455 of yacc.c  */
#line 371 "yacc.y"
    {isConst = 0;;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 372 "yacc.y"
    {isConst = 0;;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 375 "yacc.y"
    {
    isConst = 1;
;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 384 "yacc.y"
    { insertSymbol((yyvsp[(1) - (1)].id), currentType, isConst); ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 385 "yacc.y"
    {
    insertSymbol((yyvsp[(1) - (3)].id), currentType, isConst);
    emit("=", (yyvsp[(3) - (3)].exprInfo).name, "", (yyvsp[(1) - (3)].id));
  ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 392 "yacc.y"
    { currentType = "int"; ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 393 "yacc.y"
    { currentType = "float"; ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 394 "yacc.y"
    { currentType = "char"; ;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 395 "yacc.y"
    { currentType = "double"; ;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 396 "yacc.y"
    { currentType = "bool"; ;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 397 "yacc.y"
    { currentType = "string"; ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 398 "yacc.y"
    { currentType = "long"; ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 399 "yacc.y"
    { currentType = "short"; ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 400 "yacc.y"
    { currentType = "unsigned"; ;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 401 "yacc.y"
    { currentType = "signed"; ;}
    break;

  case 25:

/* Line 1455 of yacc.c  */
#line 405 "yacc.y"
    {
        currentFunction = insertFunction((yyvsp[(2) - (2)].id), currentType);
        emit("function", (yyvsp[(2) - (2)].id), "", "");
        enterScope();  // Enter function scope
    ;}
    break;

  case 26:

/* Line 1455 of yacc.c  */
#line 409 "yacc.y"
    {
        emit("endFunc", "", "", "");
        exitScope();   // Exit function scope
        currentFunction = NULL;
    ;}
    break;

  case 27:

/* Line 1455 of yacc.c  */
#line 414 "yacc.y"
    {
        currentFunction = insertFunction((yyvsp[(2) - (2)].id), "void");
        emit("function", (yyvsp[(2) - (2)].id), "", "");
        enterScope();  // Enter function scope
    ;}
    break;

  case 28:

/* Line 1455 of yacc.c  */
#line 418 "yacc.y"
    {
        emit("endFunc", "", "", "");
        exitScope();   // Exit function scope
        currentFunction = NULL;
    ;}
    break;

  case 29:

/* Line 1455 of yacc.c  */
#line 426 "yacc.y"
    {
        currentArgCount = 0;
    ;}
    break;

  case 31:

/* Line 1455 of yacc.c  */
#line 433 "yacc.y"
    {
        emitParameter((yyvsp[(1) - (1)].exprInfo).name);
        currentArgCount = 1; 
    ;}
    break;

  case 32:

/* Line 1455 of yacc.c  */
#line 437 "yacc.y"
    {
        emitParameter((yyvsp[(3) - (3)].exprInfo).name);
        currentArgCount++;
    ;}
    break;

  case 37:

/* Line 1455 of yacc.c  */
#line 451 "yacc.y"
    {
        if (currentFunction) {
            addParameter(currentFunction, (yyvsp[(2) - (2)].id), currentType);
        }
    ;}
    break;

  case 38:

/* Line 1455 of yacc.c  */
#line 459 "yacc.y"
    {
        enterScope();
    ;}
    break;

  case 39:

/* Line 1455 of yacc.c  */
#line 461 "yacc.y"
    {
        exitScope();
    ;}
    break;

  case 44:

/* Line 1455 of yacc.c  */
#line 477 "yacc.y"
    {
        (yyval.exprInfo) = (yyvsp[(1) - (2)].exprInfo);
    ;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 490 "yacc.y"
    {
        (yyval.exprInfo) = (yyvsp[(1) - (1)].exprInfo);
    ;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 493 "yacc.y"
    {
        // need to check if the variable is declared and if the types match

        char* lhs_type = getType((yyvsp[(1) - (3)].exprInfo).name);

        emit("=", (yyvsp[(3) - (3)].exprInfo).name, "", (yyvsp[(1) - (3)].exprInfo).name);
        (yyval.exprInfo).name = (yyvsp[(1) - (3)].exprInfo).name;
        (yyval.exprInfo).type = lhs_type;
    ;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 505 "yacc.y"
    {
        (yyval.exprInfo) = (yyvsp[(1) - (1)].exprInfo);
    ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 508 "yacc.y"
    {
        char* temp = newTemp();
        emit("||", (yyvsp[(1) - (3)].exprInfo).name, (yyvsp[(3) - (3)].exprInfo).name, temp);
        (yyval.exprInfo).name = temp;
        (yyval.exprInfo).type = strdup("bool");  // result of logical OR is always bool
    ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 517 "yacc.y"
    {
        (yyval.exprInfo) = (yyvsp[(1) - (1)].exprInfo);
    ;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 520 "yacc.y"
    {
        char* temp = newTemp();
        emit("&&", (yyvsp[(1) - (3)].exprInfo).name, (yyvsp[(3) - (3)].exprInfo).name, temp);
        (yyval.exprInfo).name = temp;
        (yyval.exprInfo).type = strdup("bool");  
    ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 529 "yacc.y"
    {
        (yyval.exprInfo) = (yyvsp[(1) - (1)].exprInfo);
    ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 532 "yacc.y"
    {
        char* temp = newTemp();
        emit("==", (yyvsp[(1) - (3)].exprInfo).name, (yyvsp[(3) - (3)].exprInfo).name, temp);
        (yyval.exprInfo).name = temp;
        (yyval.exprInfo).type = strdup("bool");
    ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 538 "yacc.y"
    {
        char* temp = newTemp();
        emit("!=", (yyvsp[(1) - (3)].exprInfo).name, (yyvsp[(3) - (3)].exprInfo).name, temp);
        (yyval.exprInfo).name = temp;
        (yyval.exprInfo).type = strdup("bool");
    ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 544 "yacc.y"
    {
        char* temp = newTemp();
        emit("<", (yyvsp[(1) - (3)].exprInfo).name, (yyvsp[(3) - (3)].exprInfo).name, temp);
        (yyval.exprInfo).name = temp;
        (yyval.exprInfo).type = strdup("bool");
    ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 550 "yacc.y"
    {
        char* temp = newTemp();
        emit(">", (yyvsp[(1) - (3)].exprInfo).name, (yyvsp[(3) - (3)].exprInfo).name, temp);
        (yyval.exprInfo).name = temp;
        (yyval.exprInfo).type = strdup("bool");
    ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 556 "yacc.y"
    {
        char* temp = newTemp();
        emit("<=", (yyvsp[(1) - (3)].exprInfo).name, (yyvsp[(3) - (3)].exprInfo).name, temp);
        (yyval.exprInfo).name = temp;
        (yyval.exprInfo).type = strdup("bool");
    ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 562 "yacc.y"
    {
        char* temp = newTemp();
        emit(">=", (yyvsp[(1) - (3)].exprInfo).name, (yyvsp[(3) - (3)].exprInfo).name, temp);
        (yyval.exprInfo).name = temp;
        (yyval.exprInfo).type = strdup("bool");
    ;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 571 "yacc.y"
    {
        (yyval.exprInfo) = (yyvsp[(1) - (1)].exprInfo);
    ;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 574 "yacc.y"
    {
        char* temp = newTemp();
        emit("+", (yyvsp[(1) - (3)].exprInfo).name, (yyvsp[(3) - (3)].exprInfo).name, temp);
        (yyval.exprInfo).name = temp;
        (yyval.exprInfo).type = resolveType((yyvsp[(1) - (3)].exprInfo).type, (yyvsp[(3) - (3)].exprInfo).type);  
    ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 580 "yacc.y"
    {
        char* temp = newTemp();
        emit("-", (yyvsp[(1) - (3)].exprInfo).name, (yyvsp[(3) - (3)].exprInfo).name, temp);
        (yyval.exprInfo).name = temp;
        (yyval.exprInfo).type = resolveType((yyvsp[(1) - (3)].exprInfo).type, (yyvsp[(3) - (3)].exprInfo).type);
    ;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 590 "yacc.y"
    {
        char* temp = newTemp();
        emit("*", (yyvsp[(1) - (3)].exprInfo).name, (yyvsp[(3) - (3)].exprInfo).name, temp);
        (yyval.exprInfo).name = temp;
        (yyval.exprInfo).type = resolveType((yyvsp[(1) - (3)].exprInfo).type, (yyvsp[(3) - (3)].exprInfo).type);
    ;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 596 "yacc.y"
    {
        char* temp = newTemp();
        emit("/", (yyvsp[(1) - (3)].exprInfo).name, (yyvsp[(3) - (3)].exprInfo).name, temp);
        (yyval.exprInfo).name = temp;
        (yyval.exprInfo).type = resolveType((yyvsp[(1) - (3)].exprInfo).type, (yyvsp[(3) - (3)].exprInfo).type);
    ;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 602 "yacc.y"
    {
        char* temp = newTemp();
        emit("%", (yyvsp[(1) - (3)].exprInfo).name, (yyvsp[(3) - (3)].exprInfo).name, temp);
        (yyval.exprInfo).name = temp;
        (yyval.exprInfo).type = resolveType((yyvsp[(1) - (3)].exprInfo).type, (yyvsp[(3) - (3)].exprInfo).type);
    ;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 608 "yacc.y"
    {(yyval.exprInfo)=(yyvsp[(1) - (1)].exprInfo);;}
    break;

  case 72:

/* Line 1455 of yacc.c  */
#line 612 "yacc.y"
    {(yyval.exprInfo)=(yyvsp[(1) - (1)].exprInfo);;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 613 "yacc.y"
    {
        char* temp = newTemp();
        emit("!", (yyvsp[(2) - (2)].exprInfo).name, "", temp);
        (yyval.exprInfo).name = temp;
        (yyval.exprInfo).type = (yyvsp[(2) - (2)].exprInfo).type; // here type remains the same
    ;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 619 "yacc.y"
    {
        (yyval.exprInfo) = (yyvsp[(2) - (3)].exprInfo);
    ;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 626 "yacc.y"
    {
    // need to check if the variable is declared 
    (yyval.exprInfo).name = (yyvsp[(1) - (1)].id);
    (yyval.exprInfo).type = getType((yyvsp[(1) - (1)].id));
    ;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 631 "yacc.y"
    {
    char* temp = newTemp();
    emit("=", (yyvsp[(1) - (1)].id), "", temp);
    (yyval.exprInfo).name = temp;
    (yyval.exprInfo).type = strdup("string");
    ;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 637 "yacc.y"
    {
    char val[4]; sprintf(val, "'%c'", (yyvsp[(1) - (1)].c));
    (yyval.exprInfo).name = strdup(val);
    (yyval.exprInfo).type = strdup("char");
    ;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 642 "yacc.y"
    {
    char val[20]; sprintf(val, "%f", (yyvsp[(1) - (1)].f));
    (yyval.exprInfo).name = strdup(val);
    (yyval.exprInfo).type = strdup("float");
    ;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 647 "yacc.y"
    {
    char val[20]; sprintf(val, "%d", (yyvsp[(1) - (1)].i));
    (yyval.exprInfo).name = strdup(val);
    (yyval.exprInfo).type = strdup("int");
    ;}
    break;

  case 81:

/* Line 1455 of yacc.c  */
#line 652 "yacc.y"
    {
    char val[10]; sprintf(val, "%d", (yyvsp[(1) - (1)].i));
    (yyval.exprInfo).name = strdup(val);
    (yyval.exprInfo).type = strdup("bool");
    ;}
    break;

  case 82:

/* Line 1455 of yacc.c  */
#line 657 "yacc.y"
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
                (yyval.exprInfo).type = strdup(func->returnType); // Still use return type if known
            }
        }
    ;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 679 "yacc.y"
    {
            char* endLabel = newLabel();
            emit("ifFalseGoTo", (yyvsp[(3) - (5)].exprInfo).name, "", endLabel);
            // Emit code for the block
            emit("label", "", "", endLabel);
        ;}
    break;

  case 84:

/* Line 1455 of yacc.c  */
#line 685 "yacc.y"
    {
            char* endLabel = newLabel();
            char* elseLabel = newLabel();
            emit("ifFalseGoTo", (yyvsp[(3) - (7)].exprInfo).name, "", elseLabel);
            // Emit code for the true block ($5)
            emit("goto", "", "", endLabel);
            emit("label", "", "", elseLabel);
            // Emit code for the else block ($7)
            emit("label", "", "", endLabel);
        ;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 698 "yacc.y"
    {
    // if expression is true
    char* switchTemp = newTemp();
    emit("assign", (yyvsp[(3) - (7)].exprInfo).name, "", switchTemp);
    emitCases((yyvsp[(6) - (7)].caseLabel), switchTemp);           
;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 705 "yacc.y"
    {(yyval.caseLabel) = NULL;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 705 "yacc.y"
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

  case 89:

/* Line 1455 of yacc.c  */
#line 715 "yacc.y"
    {
                char* label = newLabel();
                emit("label", "", "", label);
                //emit statement code;
                (yyval.caseLabel) = newCaseLabel((yyvsp[(2) - (4)].id), label);
;}
    break;

  case 90:

/* Line 1455 of yacc.c  */
#line 721 "yacc.y"
    {
                char* label = newLabel();
                emit("label", "", "", label);
                // emit statement code
                (yyval.caseLabel) = newCaseLabel("default", label);
            ;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 729 "yacc.y"
    {
    char buffer[20];
    sprintf(buffer, "%d", (yyvsp[(1) - (1)].i));
    (yyval.id) = strdup(buffer);
;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 734 "yacc.y"
    {
    char buffer[20];
    sprintf(buffer, "%f", (yyvsp[(1) - (1)].f));
    (yyval.id) = strdup(buffer);
;}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 739 "yacc.y"
    {
    if(!lookup((yyvsp[(1) - (1)].id))){
        yyerror("Undeclared identifier used as a constant for switch\n");
    }
    (yyval.id) = strdup((yyvsp[(1) - (1)].id));
;}
    break;

  case 97:

/* Line 1455 of yacc.c  */
#line 754 "yacc.y"
    {
            char* start = newLabel();
            char* end = newLabel();
            // emit code related to init
            emit("label", "", "", start);
            emit("ifFalseGoTo", (yyvsp[(5) - (9)].exprInfo).name, "", end);
            // emite code l body ally hwa l b lock;
            // emit code for incrementing
            emit("goto", "", "", start);
            emit("label", "", "", end);
            
        ;}
    break;

  case 98:

/* Line 1455 of yacc.c  */
#line 767 "yacc.y"
    {
            char* start = newLabel();
            char* end = newLabel();
            emit("label", "", "", start);
            emit("ifFalseGoTo", (yyvsp[(6) - (10)].exprInfo).name, "", end);
            // emite code l body ally hwa l b lock;
            // emit code for incrementing
            emit("goto", "", "", start);
            emit("label", "", "", end);
            ;}
    break;

  case 99:

/* Line 1455 of yacc.c  */
#line 779 "yacc.y"
    {
    char* start = newLabel();
    char* end = newLabel();
    // emit condition expression
    emit("label", "", "", start);
    emit("ifFalseGoTo", (yyvsp[(3) - (5)].exprInfo).name, "", end);
    // body
    emit("goto", "", "", start);
    emit("label", "", "", end);
;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 791 "yacc.y"
    {
        char* start = newLabel();
        char* end = newLabel();
        emit("label", "", "", start);

        // emit code for block
        // emit code for condition
        emit("ifFalseGoTo", (yyvsp[(5) - (7)].exprInfo).name, "", end);
        emit("goto", "", "", start);
        emit("label", "", "", end);
    ;}
    break;



/* Line 1455 of yacc.c  */
#line 2631 "yacc.tab.c"
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
#line 804 "yacc.y"




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
