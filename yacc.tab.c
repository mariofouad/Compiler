
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
}

/* Line 189 of yacc.c  */
#line 274 "yacc.tab.c"

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
#line 201 "yacc.y"

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
#line 377 "yacc.tab.c"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 389 "yacc.tab.c"

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
#define YYLAST   303

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  54
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  49
/* YYNRULES -- Number of rules.  */
#define YYNRULES  111
/* YYNRULES -- Number of states.  */
#define YYNSTATES  202

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
      48,    50,    52,    54,    56,    58,    65,    72,    73,    75,
      77,    81,    82,    84,    86,    90,    93,    97,    98,   101,
     103,   105,   108,   110,   112,   114,   117,   120,   124,   127,
     129,   133,   135,   139,   141,   145,   147,   151,   155,   159,
     163,   167,   171,   173,   177,   181,   185,   189,   193,   195,
     197,   200,   204,   207,   209,   211,   213,   215,   217,   219,
     224,   225,   233,   235,   236,   240,   241,   242,   251,   252,
     255,   256,   264,   265,   272,   274,   276,   278,   280,   282,
     284,   289,   290,   291,   303,   304,   305,   317,   318,   319,
     327,   328
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int8 yyrhs[] =
{
      55,     0,    -1,    56,    -1,    -1,    56,    57,    -1,    63,
      -1,    58,    -1,    71,    -1,    62,    60,    37,    -1,    59,
      -1,    47,    62,    60,    37,    -1,    61,    -1,    60,    38,
      61,    -1,     5,    -1,     5,    22,    72,    -1,     3,    -1,
       4,    -1,    46,    -1,    48,    -1,    49,    -1,     6,    -1,
      51,    -1,    50,    -1,    52,    -1,    53,    -1,    62,     5,
      33,    66,    34,    68,    -1,    20,     5,    33,    66,    34,
      68,    -1,    -1,    65,    -1,    72,    -1,    65,    38,    72,
      -1,    -1,    20,    -1,    67,    -1,    66,    38,    67,    -1,
      62,     5,    -1,    35,    69,    36,    -1,    -1,    69,    70,
      -1,    58,    -1,    71,    -1,    72,    37,    -1,    80,    -1,
      91,    -1,    68,    -1,    21,    37,    -1,    17,    37,    -1,
      16,    72,    37,    -1,    16,    37,    -1,    73,    -1,    72,
      22,    72,    -1,    74,    -1,    73,    45,    74,    -1,    75,
      -1,    74,    44,    75,    -1,    76,    -1,    75,    23,    76,
      -1,    75,    24,    76,    -1,    75,    27,    76,    -1,    75,
      28,    76,    -1,    75,    25,    76,    -1,    75,    26,    76,
      -1,    77,    -1,    76,    29,    77,    -1,    76,    30,    77,
      -1,    77,    31,    78,    -1,    77,    32,    78,    -1,    77,
      18,    78,    -1,    78,    -1,    79,    -1,    19,    78,    -1,
      33,    72,    34,    -1,    30,    78,    -1,     5,    -1,    10,
      -1,     9,    -1,     8,    -1,     7,    -1,    11,    -1,     5,
      33,    64,    34,    -1,    -1,    12,    33,    72,    81,    34,
      68,    82,    -1,    84,    -1,    -1,    13,    83,    68,    -1,
      -1,    -1,    39,    85,    33,    72,    34,    35,    86,    36,
      -1,    -1,    86,    87,    -1,    -1,    40,    90,    42,    88,
      71,    17,    37,    -1,    -1,    41,    42,    89,    71,    17,
      37,    -1,     7,    -1,     8,    -1,     5,    -1,    93,    -1,
      98,    -1,   101,    -1,    62,     5,    22,    72,    -1,    -1,
      -1,    43,    33,    72,    37,    94,    72,    37,    95,    72,
      34,    68,    -1,    -1,    -1,    43,    33,    92,    37,    96,
      72,    37,    97,    72,    34,    68,    -1,    -1,    -1,    14,
      33,    99,    72,   100,    34,    68,    -1,    -1,    15,   102,
      68,    14,    33,    72,    34,    37,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   256,   256,   259,   261,   265,   266,   267,   271,   272,
     275,   280,   281,   284,   285,   292,   293,   294,   295,   296,
     297,   298,   299,   300,   301,   305,   306,   309,   311,   315,
     316,   319,   321,   322,   323,   327,   330,   332,   334,   338,
     339,   343,   346,   347,   348,   349,   350,   351,   352,   356,
     359,   371,   374,   383,   386,   395,   398,   404,   410,   416,
     422,   428,   437,   440,   446,   456,   462,   468,   474,   478,
     479,   485,   488,   492,   497,   503,   508,   513,   518,   523,
     534,   534,   539,   543,   543,   555,   562,   562,   574,   574,
     584,   584,   601,   601,   616,   621,   626,   635,   636,   637,
     640,   648,   654,   648,   667,   673,   667,   688,   692,   688,
     707,   707
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
  "function_definition", "argument_list", "argument_sequence",
  "parameter_list", "parameter_declaration", "block", "block_items",
  "block_item", "statement", "expression", "logical_or_expression",
  "logical_and_expression", "comparison_expression",
  "mathematical_expression", "term", "factor", "primary_expression",
  "conditional_statement", "$@1", "optional_else", "$@2",
  "switch_statement", "$@3", "switch_case_list", "switch_case", "$@4",
  "$@5", "constant", "loops", "for_init_decl", "for_loop", "$@6", "$@7",
  "$@8", "$@9", "while_loop", "$@10", "$@11", "do_while_loop", "$@12", 0
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
      62,    62,    62,    62,    62,    63,    63,    64,    64,    65,
      65,    66,    66,    66,    66,    67,    68,    69,    69,    70,
      70,    71,    71,    71,    71,    71,    71,    71,    71,    72,
      72,    73,    73,    74,    74,    75,    75,    75,    75,    75,
      75,    75,    76,    76,    76,    77,    77,    77,    77,    78,
      78,    78,    78,    79,    79,    79,    79,    79,    79,    79,
      81,    80,    80,    83,    82,    82,    85,    84,    86,    86,
      88,    87,    89,    87,    90,    90,    90,    91,    91,    91,
      92,    94,    95,    93,    96,    97,    93,    99,   100,    98,
     102,   101
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     0,     2,     1,     1,     1,     3,     1,
       4,     1,     3,     1,     3,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     6,     6,     0,     1,     1,
       3,     0,     1,     1,     3,     2,     3,     0,     2,     1,
       1,     2,     1,     1,     1,     2,     2,     3,     2,     1,
       3,     1,     3,     1,     3,     1,     3,     3,     3,     3,
       3,     3,     1,     3,     3,     3,     3,     3,     1,     1,
       2,     3,     2,     1,     1,     1,     1,     1,     1,     4,
       0,     7,     1,     0,     3,     0,     0,     8,     0,     2,
       0,     7,     0,     6,     1,     1,     1,     1,     1,     1,
       4,     0,     0,    11,     0,     0,    11,     0,     0,     7,
       0,     8
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,     2,     1,    15,    16,    73,    20,    77,    76,
      75,    74,    78,     0,     0,   110,     0,     0,     0,     0,
       0,     0,     0,    37,    86,     0,    17,     0,    18,    19,
      22,    21,    23,    24,     4,     6,     9,     0,     5,    44,
       7,     0,    49,    51,    53,    55,    62,    68,    69,    42,
      82,    43,    97,    98,    99,    27,     0,   107,     0,    48,
       0,    46,    70,     0,    45,    72,     0,     0,     0,     0,
       0,    13,     0,    11,     0,    41,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      28,    29,    80,     0,     0,    47,    31,    71,    36,    39,
       0,    38,    40,     0,     0,     0,     0,    13,     0,     0,
      31,     8,     0,    50,    52,    54,    56,    57,    60,    61,
      58,    59,    63,    64,    67,    65,    66,    79,     0,     0,
     108,     0,    32,     0,     0,    33,     0,     0,   101,   104,
      10,    14,     0,    12,    30,     0,     0,     0,    35,     0,
       0,     0,     0,     0,     0,     0,    85,     0,     0,    26,
      34,    88,   100,     0,     0,    25,    83,    81,   109,     0,
       0,   102,   105,     0,   111,    87,     0,     0,    89,     0,
       0,    84,    96,    94,    95,     0,    92,     0,     0,    90,
       0,     0,     0,     0,     0,   103,   106,     0,     0,     0,
      93,    91
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,    34,    35,    36,    72,    73,   133,    38,
      89,    90,   134,   135,    39,    67,   101,    40,    41,    42,
      43,    44,    45,    46,    47,    48,    49,   129,   167,   173,
      50,    68,   170,   178,   193,   190,   185,    51,   106,    52,
     153,   179,   154,   180,    53,    93,   146,    54,    58
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -66
static const yytype_int16 yypact[] =
{
     -66,     5,   136,   -66,   -66,   -66,   -25,   -66,   -66,   -66,
     -66,   -66,   -66,   -20,    -4,   -66,    96,    -5,   270,    18,
      15,   270,   270,   -66,   -66,    38,   -66,   206,   -66,   -66,
     -66,   -66,   -66,   -66,   -66,   -66,   -66,    70,   -66,   -66,
     -66,   -13,    41,    45,    94,     8,    17,   -66,   -66,   -66,
     -66,   -66,   -66,   -66,   -66,   270,   270,   -66,    56,   -66,
     -12,   -66,   -66,    66,   -66,   -66,    20,   187,    97,    11,
     122,    23,    29,   -66,   270,   -66,   270,   270,   270,   270,
     270,   270,   270,   270,   270,   270,   270,   270,   270,    98,
     111,   132,   132,   270,   144,   -66,    30,   -66,   -66,   -66,
     122,   -66,   -66,   270,   154,   -11,   123,   139,    57,   270,
      30,   -66,   122,   132,    45,    94,     8,     8,     8,     8,
       8,     8,    17,    17,   -66,   -66,   -66,   -66,   270,   128,
     132,   134,   -66,   160,    13,   -66,    21,   146,   -66,   -66,
     -66,   132,    31,   -66,   132,    56,   138,   270,   -66,    56,
     206,   135,   270,   270,   270,    56,   161,    56,    62,   -66,
     -66,   -66,   132,   -10,     9,   -66,   -66,   -66,   -66,   140,
      73,   -66,   -66,    56,   -66,   -66,   103,   131,   -66,   270,
     270,   -66,   -66,   -66,   -66,   158,   -66,    63,    68,   -66,
     255,    56,    56,   255,   159,   -66,   -66,   164,   141,   143,
     -66,   -66
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
     -66,   -66,   -66,   -66,   147,   -66,   137,    93,     1,   -66,
     -66,   -66,   101,    65,   -57,   -66,   -66,   -65,   -16,   -66,
     142,   148,   163,    39,   -14,   -66,   -66,   -66,   -66,   -66,
     -66,   -66,   -66,   -66,   -66,   -66,   -66,   -66,   -66,   -66,
     -66,   -66,   -66,   -66,   -66,   -66,   -66,   -66,   -66
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_uint8 yytable[] =
{
      60,    94,   102,    37,    62,     3,    66,    65,    55,    74,
      74,    74,    74,    56,     4,     5,     6,     7,     8,     9,
      10,    11,    12,    63,    75,    95,   138,   171,    70,    57,
      18,    74,    61,     4,     5,    86,     7,    84,    85,    91,
      92,    21,    74,    74,    22,   109,   172,   149,    87,    88,
     132,   150,    64,   105,    97,   151,   110,    26,   113,    28,
      29,    30,    31,    32,    33,   155,   111,   112,   100,   150,
     104,    69,   124,   125,   126,    71,    26,   130,    28,    29,
      30,    31,    32,    33,    74,    74,    76,   136,   156,    77,
      74,    23,   159,   141,   140,   112,   169,   191,   165,    96,
     168,     6,   192,     8,     9,    10,    11,    12,   182,   175,
     183,   184,   144,   176,   177,    18,   181,    78,    79,    80,
      81,    82,    83,   122,   123,   194,    21,   107,   197,    22,
     103,   158,   127,    59,   195,   196,   162,   163,   164,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,   128,
      14,    15,    16,    17,    74,    18,    19,    20,   131,   137,
     139,   109,   145,   187,   188,   148,    21,   147,   152,    22,
     161,    23,   157,   186,   166,    24,   198,   174,   200,    25,
     201,   199,    26,    27,    28,    29,    30,    31,    32,    33,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    13,
     189,    14,    15,    16,    17,   143,    18,   108,    20,     4,
       5,   142,     7,     0,    99,   160,     0,    21,   114,     0,
      22,     0,    23,    98,     0,   115,    24,     0,     0,     0,
      25,     0,     0,    26,    27,    28,    29,    30,    31,    32,
      33,   116,   117,   118,   119,   120,   121,     0,     0,     0,
       0,     0,    26,     0,    28,    29,    30,    31,    32,    33,
       6,     0,     8,     9,    10,    11,    12,    13,     0,    14,
      15,    16,    17,     0,    18,     6,    20,     8,     9,    10,
      11,    12,     0,     0,     0,    21,     0,     0,    22,    18,
      23,     0,     0,     0,    24,     0,     0,     0,    25,     0,
      21,     0,     0,    22
};

static const yytype_int16 yycheck[] =
{
      16,    58,    67,     2,    18,     0,    22,    21,    33,    22,
      22,    22,    22,    33,     3,     4,     5,     6,     7,     8,
       9,    10,    11,     5,    37,    37,    37,    37,    27,    33,
      19,    22,    37,     3,     4,    18,     6,    29,    30,    55,
      56,    30,    22,    22,    33,    22,    37,    34,    31,    32,
      20,    38,    37,    69,    34,    34,    33,    46,    74,    48,
      49,    50,    51,    52,    53,    34,    37,    38,    67,    38,
      69,    33,    86,    87,    88,     5,    46,    93,    48,    49,
      50,    51,    52,    53,    22,    22,    45,   103,   145,    44,
      22,    35,   149,   109,    37,    38,    34,    34,   155,    33,
     157,     5,    34,     7,     8,     9,    10,    11,     5,    36,
       7,     8,   128,    40,    41,    19,   173,    23,    24,    25,
      26,    27,    28,    84,    85,   190,    30,     5,   193,    33,
      33,   147,    34,    37,   191,   192,   152,   153,   154,     3,
       4,     5,     6,     7,     8,     9,    10,    11,    12,    38,
      14,    15,    16,    17,    22,    19,    20,    21,    14,     5,
      37,    22,    34,   179,   180,     5,    30,    33,    22,    33,
      35,    35,    34,    42,    13,    39,    17,    37,    37,    43,
      37,    17,    46,    47,    48,    49,    50,    51,    52,    53,
       3,     4,     5,     6,     7,     8,     9,    10,    11,    12,
      42,    14,    15,    16,    17,   112,    19,    70,    21,     3,
       4,   110,     6,    -1,    67,   150,    -1,    30,    76,    -1,
      33,    -1,    35,    36,    -1,    77,    39,    -1,    -1,    -1,
      43,    -1,    -1,    46,    47,    48,    49,    50,    51,    52,
      53,    78,    79,    80,    81,    82,    83,    -1,    -1,    -1,
      -1,    -1,    46,    -1,    48,    49,    50,    51,    52,    53,
       5,    -1,     7,     8,     9,    10,    11,    12,    -1,    14,
      15,    16,    17,    -1,    19,     5,    21,     7,     8,     9,
      10,    11,    -1,    -1,    -1,    30,    -1,    -1,    33,    19,
      35,    -1,    -1,    -1,    39,    -1,    -1,    -1,    43,    -1,
      30,    -1,    -1,    33
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,    55,    56,     0,     3,     4,     5,     6,     7,     8,
       9,    10,    11,    12,    14,    15,    16,    17,    19,    20,
      21,    30,    33,    35,    39,    43,    46,    47,    48,    49,
      50,    51,    52,    53,    57,    58,    59,    62,    63,    68,
      71,    72,    73,    74,    75,    76,    77,    78,    79,    80,
      84,    91,    93,    98,   101,    33,    33,    33,   102,    37,
      72,    37,    78,     5,    37,    78,    72,    69,    85,    33,
      62,     5,    60,    61,    22,    37,    45,    44,    23,    24,
      25,    26,    27,    28,    29,    30,    18,    31,    32,    64,
      65,    72,    72,    99,    68,    37,    33,    34,    36,    58,
      62,    70,    71,    33,    62,    72,    92,     5,    60,    22,
      33,    37,    38,    72,    74,    75,    76,    76,    76,    76,
      76,    76,    77,    77,    78,    78,    78,    34,    38,    81,
      72,    14,    20,    62,    66,    67,    72,     5,    37,    37,
      37,    72,    66,    61,    72,    34,   100,    33,     5,    34,
      38,    34,    22,    94,    96,    34,    68,    34,    72,    68,
      67,    35,    72,    72,    72,    68,    13,    82,    68,    34,
      86,    37,    37,    83,    37,    36,    40,    41,    87,    95,
      97,    68,     5,     7,     8,    90,    42,    72,    72,    42,
      89,    34,    34,    88,    71,    68,    68,    71,    17,    17,
      37,    37
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
#line 271 "yacc.y"
    {isConst = 0;;}
    break;

  case 9:

/* Line 1455 of yacc.c  */
#line 272 "yacc.y"
    {isConst = 0;;}
    break;

  case 10:

/* Line 1455 of yacc.c  */
#line 275 "yacc.y"
    {
    isConst = 1;
;}
    break;

  case 13:

/* Line 1455 of yacc.c  */
#line 284 "yacc.y"
    { insertSymbol((yyvsp[(1) - (1)].id), currentType, isConst); ;}
    break;

  case 14:

/* Line 1455 of yacc.c  */
#line 285 "yacc.y"
    {
    insertSymbol((yyvsp[(1) - (3)].id), currentType, isConst);
    emit("=", (yyvsp[(3) - (3)].exprInfo).name, "", (yyvsp[(1) - (3)].id));
  ;}
    break;

  case 15:

/* Line 1455 of yacc.c  */
#line 292 "yacc.y"
    { currentType = "int"; ;}
    break;

  case 16:

/* Line 1455 of yacc.c  */
#line 293 "yacc.y"
    { currentType = "float"; ;}
    break;

  case 17:

/* Line 1455 of yacc.c  */
#line 294 "yacc.y"
    { currentType = "char"; ;}
    break;

  case 18:

/* Line 1455 of yacc.c  */
#line 295 "yacc.y"
    { currentType = "double"; ;}
    break;

  case 19:

/* Line 1455 of yacc.c  */
#line 296 "yacc.y"
    { currentType = "bool"; ;}
    break;

  case 20:

/* Line 1455 of yacc.c  */
#line 297 "yacc.y"
    { currentType = "string"; ;}
    break;

  case 21:

/* Line 1455 of yacc.c  */
#line 298 "yacc.y"
    { currentType = "long"; ;}
    break;

  case 22:

/* Line 1455 of yacc.c  */
#line 299 "yacc.y"
    { currentType = "short"; ;}
    break;

  case 23:

/* Line 1455 of yacc.c  */
#line 300 "yacc.y"
    { currentType = "unsigned"; ;}
    break;

  case 24:

/* Line 1455 of yacc.c  */
#line 301 "yacc.y"
    { currentType = "signed"; ;}
    break;

  case 41:

/* Line 1455 of yacc.c  */
#line 343 "yacc.y"
    {
        (yyval.exprInfo) = (yyvsp[(1) - (2)].exprInfo);
    ;}
    break;

  case 49:

/* Line 1455 of yacc.c  */
#line 356 "yacc.y"
    {
        (yyval.exprInfo) = (yyvsp[(1) - (1)].exprInfo);
    ;}
    break;

  case 50:

/* Line 1455 of yacc.c  */
#line 359 "yacc.y"
    {
        // need to check if the variable is declared and if the types match

        char* lhs_type = getType((yyvsp[(1) - (3)].exprInfo).name);

        emit("=", (yyvsp[(3) - (3)].exprInfo).name, "", (yyvsp[(1) - (3)].exprInfo).name);
        (yyval.exprInfo).name = (yyvsp[(1) - (3)].exprInfo).name;
        (yyval.exprInfo).type = lhs_type;
    ;}
    break;

  case 51:

/* Line 1455 of yacc.c  */
#line 371 "yacc.y"
    {
        (yyval.exprInfo) = (yyvsp[(1) - (1)].exprInfo);
    ;}
    break;

  case 52:

/* Line 1455 of yacc.c  */
#line 374 "yacc.y"
    {
        char* temp = newTemp();
        emit("||", (yyvsp[(1) - (3)].exprInfo).name, (yyvsp[(3) - (3)].exprInfo).name, temp);
        (yyval.exprInfo).name = temp;
        (yyval.exprInfo).type = strdup("bool");  // result of logical OR is always bool
    ;}
    break;

  case 53:

/* Line 1455 of yacc.c  */
#line 383 "yacc.y"
    {
        (yyval.exprInfo) = (yyvsp[(1) - (1)].exprInfo);
    ;}
    break;

  case 54:

/* Line 1455 of yacc.c  */
#line 386 "yacc.y"
    {
        char* temp = newTemp();
        emit("&&", (yyvsp[(1) - (3)].exprInfo).name, (yyvsp[(3) - (3)].exprInfo).name, temp);
        (yyval.exprInfo).name = temp;
        (yyval.exprInfo).type = strdup("bool");  
    ;}
    break;

  case 55:

/* Line 1455 of yacc.c  */
#line 395 "yacc.y"
    {
        (yyval.exprInfo) = (yyvsp[(1) - (1)].exprInfo);
    ;}
    break;

  case 56:

/* Line 1455 of yacc.c  */
#line 398 "yacc.y"
    {
        char* temp = newTemp();
        emit("==", (yyvsp[(1) - (3)].exprInfo).name, (yyvsp[(3) - (3)].exprInfo).name, temp);
        (yyval.exprInfo).name = temp;
        (yyval.exprInfo).type = strdup("bool");
    ;}
    break;

  case 57:

/* Line 1455 of yacc.c  */
#line 404 "yacc.y"
    {
        char* temp = newTemp();
        emit("!=", (yyvsp[(1) - (3)].exprInfo).name, (yyvsp[(3) - (3)].exprInfo).name, temp);
        (yyval.exprInfo).name = temp;
        (yyval.exprInfo).type = strdup("bool");
    ;}
    break;

  case 58:

/* Line 1455 of yacc.c  */
#line 410 "yacc.y"
    {
        char* temp = newTemp();
        emit("<", (yyvsp[(1) - (3)].exprInfo).name, (yyvsp[(3) - (3)].exprInfo).name, temp);
        (yyval.exprInfo).name = temp;
        (yyval.exprInfo).type = strdup("bool");
    ;}
    break;

  case 59:

/* Line 1455 of yacc.c  */
#line 416 "yacc.y"
    {
        char* temp = newTemp();
        emit(">", (yyvsp[(1) - (3)].exprInfo).name, (yyvsp[(3) - (3)].exprInfo).name, temp);
        (yyval.exprInfo).name = temp;
        (yyval.exprInfo).type = strdup("bool");
    ;}
    break;

  case 60:

/* Line 1455 of yacc.c  */
#line 422 "yacc.y"
    {
        char* temp = newTemp();
        emit("<=", (yyvsp[(1) - (3)].exprInfo).name, (yyvsp[(3) - (3)].exprInfo).name, temp);
        (yyval.exprInfo).name = temp;
        (yyval.exprInfo).type = strdup("bool");
    ;}
    break;

  case 61:

/* Line 1455 of yacc.c  */
#line 428 "yacc.y"
    {
        char* temp = newTemp();
        emit(">=", (yyvsp[(1) - (3)].exprInfo).name, (yyvsp[(3) - (3)].exprInfo).name, temp);
        (yyval.exprInfo).name = temp;
        (yyval.exprInfo).type = strdup("bool");
    ;}
    break;

  case 62:

/* Line 1455 of yacc.c  */
#line 437 "yacc.y"
    {
        (yyval.exprInfo) = (yyvsp[(1) - (1)].exprInfo);
    ;}
    break;

  case 63:

/* Line 1455 of yacc.c  */
#line 440 "yacc.y"
    {
        char* temp = newTemp();
        emit("+", (yyvsp[(1) - (3)].exprInfo).name, (yyvsp[(3) - (3)].exprInfo).name, temp);
        (yyval.exprInfo).name = temp;
        (yyval.exprInfo).type = resolveType((yyvsp[(1) - (3)].exprInfo).type, (yyvsp[(3) - (3)].exprInfo).type);  
    ;}
    break;

  case 64:

/* Line 1455 of yacc.c  */
#line 446 "yacc.y"
    {
        char* temp = newTemp();
        emit("-", (yyvsp[(1) - (3)].exprInfo).name, (yyvsp[(3) - (3)].exprInfo).name, temp);
        (yyval.exprInfo).name = temp;
        (yyval.exprInfo).type = resolveType((yyvsp[(1) - (3)].exprInfo).type, (yyvsp[(3) - (3)].exprInfo).type);
    ;}
    break;

  case 65:

/* Line 1455 of yacc.c  */
#line 456 "yacc.y"
    {
        char* temp = newTemp();
        emit("*", (yyvsp[(1) - (3)].exprInfo).name, (yyvsp[(3) - (3)].exprInfo).name, temp);
        (yyval.exprInfo).name = temp;
        (yyval.exprInfo).type = resolveType((yyvsp[(1) - (3)].exprInfo).type, (yyvsp[(3) - (3)].exprInfo).type);
    ;}
    break;

  case 66:

/* Line 1455 of yacc.c  */
#line 462 "yacc.y"
    {
        char* temp = newTemp();
        emit("/", (yyvsp[(1) - (3)].exprInfo).name, (yyvsp[(3) - (3)].exprInfo).name, temp);
        (yyval.exprInfo).name = temp;
        (yyval.exprInfo).type = resolveType((yyvsp[(1) - (3)].exprInfo).type, (yyvsp[(3) - (3)].exprInfo).type);
    ;}
    break;

  case 67:

/* Line 1455 of yacc.c  */
#line 468 "yacc.y"
    {
        char* temp = newTemp();
        emit("%", (yyvsp[(1) - (3)].exprInfo).name, (yyvsp[(3) - (3)].exprInfo).name, temp);
        (yyval.exprInfo).name = temp;
        (yyval.exprInfo).type = resolveType((yyvsp[(1) - (3)].exprInfo).type, (yyvsp[(3) - (3)].exprInfo).type);
    ;}
    break;

  case 68:

/* Line 1455 of yacc.c  */
#line 474 "yacc.y"
    {(yyval.exprInfo)=(yyvsp[(1) - (1)].exprInfo);;}
    break;

  case 69:

/* Line 1455 of yacc.c  */
#line 478 "yacc.y"
    {(yyval.exprInfo)=(yyvsp[(1) - (1)].exprInfo);;}
    break;

  case 70:

/* Line 1455 of yacc.c  */
#line 479 "yacc.y"
    {
        char* temp = newTemp();
        emit("!", (yyvsp[(2) - (2)].exprInfo).name, "", temp);
        (yyval.exprInfo).name = temp;
        (yyval.exprInfo).type = (yyvsp[(2) - (2)].exprInfo).type; // here type remains the same
    ;}
    break;

  case 71:

/* Line 1455 of yacc.c  */
#line 485 "yacc.y"
    {
        (yyval.exprInfo) = (yyvsp[(2) - (3)].exprInfo);
    ;}
    break;

  case 73:

/* Line 1455 of yacc.c  */
#line 492 "yacc.y"
    {
    // need to check if the variable is declared 
    (yyval.exprInfo).name = (yyvsp[(1) - (1)].id);
    (yyval.exprInfo).type = getType((yyvsp[(1) - (1)].id));
    ;}
    break;

  case 74:

/* Line 1455 of yacc.c  */
#line 497 "yacc.y"
    {
    char* temp = newTemp();
    emit("=", (yyvsp[(1) - (1)].id), "", temp);
    (yyval.exprInfo).name = temp;
    (yyval.exprInfo).type = strdup("string");
    ;}
    break;

  case 75:

/* Line 1455 of yacc.c  */
#line 503 "yacc.y"
    {
    char val[4]; sprintf(val, "'%c'", (yyvsp[(1) - (1)].c));
    (yyval.exprInfo).name = strdup(val);
    (yyval.exprInfo).type = strdup("char");
    ;}
    break;

  case 76:

/* Line 1455 of yacc.c  */
#line 508 "yacc.y"
    {
    char val[20]; sprintf(val, "%f", (yyvsp[(1) - (1)].f));
    (yyval.exprInfo).name = strdup(val);
    (yyval.exprInfo).type = strdup("float");
    ;}
    break;

  case 77:

/* Line 1455 of yacc.c  */
#line 513 "yacc.y"
    {
    char val[20]; sprintf(val, "%d", (yyvsp[(1) - (1)].i));
    (yyval.exprInfo).name = strdup(val);
    (yyval.exprInfo).type = strdup("int");
    ;}
    break;

  case 78:

/* Line 1455 of yacc.c  */
#line 518 "yacc.y"
    {
    char val[10]; sprintf(val, "%d", (yyvsp[(1) - (1)].i));
    (yyval.exprInfo).name = strdup(val);
    (yyval.exprInfo).type = strdup("bool");
    ;}
    break;

  case 79:

/* Line 1455 of yacc.c  */
#line 523 "yacc.y"
    {
    // need to check if the function is declared
    char* temp = newTemp();
    emit("CALL", (yyvsp[(1) - (4)].id), "", temp);
    (yyval.exprInfo).name = temp;
    (yyval.exprInfo).type = getType((yyvsp[(1) - (4)].id)); // assuming function return type is stored in symbol table 
    ;}
    break;

  case 80:

/* Line 1455 of yacc.c  */
#line 534 "yacc.y"
    {
        char* elseLabel = newLabel();
        emit("ifFalseGoto", (yyvsp[(3) - (3)].exprInfo).name, "", elseLabel);
        push(elseLabel);
    ;}
    break;

  case 83:

/* Line 1455 of yacc.c  */
#line 543 "yacc.y"
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

  case 84:

/* Line 1455 of yacc.c  */
#line 551 "yacc.y"
    {
        char* endLabel = pop();
        emit("label", "", "", endLabel);
    ;}
    break;

  case 85:

/* Line 1455 of yacc.c  */
#line 555 "yacc.y"
    {
        // Get the else label and place it here
        char* elseLabel = pop();
        emit("label", "", "", elseLabel);
    ;}
    break;

  case 86:

/* Line 1455 of yacc.c  */
#line 562 "yacc.y"
    {
    char* switchEndLabel = newLabel();
    push(switchEndLabel);
;}
    break;

  case 87:

/* Line 1455 of yacc.c  */
#line 565 "yacc.y"
    {
    // if expression is true
    char* switchTemp = newTemp();
    emit("=", (yyvsp[(4) - (8)].exprInfo).name, "", switchTemp);
    emitCases((yyvsp[(7) - (8)].caseLabel), switchTemp);
    char* switchEndLabel = pop();
    emit("label", "", "", switchEndLabel);           
;}
    break;

  case 88:

/* Line 1455 of yacc.c  */
#line 574 "yacc.y"
    {(yyval.caseLabel) = NULL;}
    break;

  case 89:

/* Line 1455 of yacc.c  */
#line 574 "yacc.y"
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

  case 90:

/* Line 1455 of yacc.c  */
#line 584 "yacc.y"
    {
                char* label = newLabel();
                emit("label", "", "", label);
                //emit statement code;
                printf("Stack length is %d\n", isStackEmpty());
                push(label);

                ;}
    break;

  case 91:

/* Line 1455 of yacc.c  */
#line 592 "yacc.y"
    {
                char* label = pop();
                char* end = pop();
                
                emit("goto", "", "", end);
                push(end);
                (yyval.caseLabel) = newCaseLabel((yyvsp[(2) - (7)].id), label);
            ;}
    break;

  case 92:

/* Line 1455 of yacc.c  */
#line 601 "yacc.y"
    {
                char* label = newLabel();
                emit("label", "", "", label);
                // emit statement code
                push(label);
            ;}
    break;

  case 93:

/* Line 1455 of yacc.c  */
#line 606 "yacc.y"
    {
                char* label = pop();
                char* end = pop();
                emit("goto", "", "", end);
                (yyval.caseLabel) = newCaseLabel("default", label);
                push(end);

            ;}
    break;

  case 94:

/* Line 1455 of yacc.c  */
#line 616 "yacc.y"
    {
    char buffer[20];
    sprintf(buffer, "%d", (yyvsp[(1) - (1)].i));
    (yyval.id) = strdup(buffer);
;}
    break;

  case 95:

/* Line 1455 of yacc.c  */
#line 621 "yacc.y"
    {
    char buffer[20];
    sprintf(buffer, "%f", (yyvsp[(1) - (1)].f));
    (yyval.id) = strdup(buffer);
;}
    break;

  case 96:

/* Line 1455 of yacc.c  */
#line 626 "yacc.y"
    {
    if(!lookup((yyvsp[(1) - (1)].id))){
        yyerror("Undeclared identifier used as a constant for switch\n");
    }
    (yyval.id) = strdup((yyvsp[(1) - (1)].id));
;}
    break;

  case 100:

/* Line 1455 of yacc.c  */
#line 640 "yacc.y"
    {
        insertSymbol((yyvsp[(2) - (4)].id), currentType, isConst);
        emit("=", (yyvsp[(4) - (4)].exprInfo).name, "", (yyvsp[(2) - (4)].id));
        (yyval.exprInfo).name = (yyvsp[(2) - (4)].id);
        (yyval.exprInfo).type = currentType;
    ;}
    break;

  case 101:

/* Line 1455 of yacc.c  */
#line 648 "yacc.y"
    {
        char* start = newLabel();
        char* end = newLabel();
        emit("label", "", "", start);
        push(start); push(end);
    ;}
    break;

  case 102:

/* Line 1455 of yacc.c  */
#line 654 "yacc.y"
    {
        char* end = pop();
        emit("ifFalseGoTo", (yyvsp[(6) - (7)].exprInfo).name, "", end);
        push(end);
        for_start = quadIndex;
    ;}
    break;

  case 103:

/* Line 1455 of yacc.c  */
#line 660 "yacc.y"
    {
        moveTwoToEnd(quadIndex, for_start);
        char* end = pop();
        char* start = pop();
        emit("goto", "", "", start);
        emit("label", "", "", end);
    ;}
    break;

  case 104:

/* Line 1455 of yacc.c  */
#line 667 "yacc.y"
    {
        char* start = newLabel();
        char* end = newLabel();
        emit("label", "", "", start);
        push(start); push(end);
    ;}
    break;

  case 105:

/* Line 1455 of yacc.c  */
#line 673 "yacc.y"
    {
        char* end = pop();
        emit("ifFalseGoTo", (yyvsp[(6) - (7)].exprInfo).name, "", end);
        push(end);
        for_start = quadIndex;
    ;}
    break;

  case 106:

/* Line 1455 of yacc.c  */
#line 679 "yacc.y"
    {
        moveTwoToEnd(quadIndex, for_start);       
        char* end = pop();
        char* start = pop();
        emit("goto", "", "", start);
        emit("label", "", "", end);
    ;}
    break;

  case 107:

/* Line 1455 of yacc.c  */
#line 688 "yacc.y"
    {
    char* start = newLabel();
    emit("label", "", "", start);
    push(start);
;}
    break;

  case 108:

/* Line 1455 of yacc.c  */
#line 692 "yacc.y"
    {
    char* end = newLabel();
    push(end);
    emit("ifFalseGoTo", (yyvsp[(4) - (4)].exprInfo).name, "", end);

;}
    break;

  case 109:

/* Line 1455 of yacc.c  */
#line 697 "yacc.y"
    {
    char* end = pop();
    char* start = pop();
    // emit condition expression
    
    // body
    emit("goto", "", "", start);
    emit("label", "", "", end);
;}
    break;

  case 110:

/* Line 1455 of yacc.c  */
#line 707 "yacc.y"
    {
        char* start = newLabel();
        emit("label", "", "", start);
        push(start);

    ;}
    break;

  case 111:

/* Line 1455 of yacc.c  */
#line 713 "yacc.y"
    {
        char* end = newLabel();
        char* start = pop();
        // emit code for block
        // emit code for condition
        emit("ifFalseGoTo", (yyvsp[(6) - (8)].exprInfo).name, "", end);
        emit("goto", "", "", start);
        emit("label", "", "", end);
    ;}
    break;



/* Line 1455 of yacc.c  */
#line 2579 "yacc.tab.c"
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
#line 724 "yacc.y"




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
