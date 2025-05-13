
/* A Bison parser, made by GNU Bison 2.4.1.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
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

/* Line 1676 of yacc.c  */
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



/* Line 1676 of yacc.c  */
#line 118 "yacc.tab.h"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


