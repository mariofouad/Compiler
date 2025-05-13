/* Definitions */
%{
int yylineno = 1;
int column = 1;

#include "yacc.tab.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#define YY_USER_ACTION column += yyleng;

void yyerror(const char *);
%}


/* Regex rules */
%%

"int"           { return INT; }
"float"         { return FLOAT; }
"char"          { return CHAR; }
"double"        { return DOUBLE; }
"bool"          { return BOOL; }
"long"          { return LONG; }
"short"         { return SHORT; }
"unsigned"      { return UNSIGNED; }
"signed"        { return SIGNED; }
"const"         { return CONST; }
"void"          { return VOID; }
"if"            { return IF; }
"else"          { return ELSE; }
"while"         { return WHILE; }
"do"            { return DO; }
"for"           { return FOR; }
"switch"        { return SWITCH; }
"case"          { return CASE; }
"default"       { return DEFAULT; }
"break"         { return BREAK; }
"continue"      { return CONTINUE; }
"return"        { return RETURN; }

"true"          { return BOOL_LITERAL; }
"false"         { return BOOL_LITERAL; }

"&&"            { return AND; }
"||"            { return OR; }
"!"             { return NOT; }

"=="            { return EQ; }
"!="            { return NEQ; }
"<="            { return LE; }
">="            { return GE; }
"<"             { return LT; }
">"             { return GT; }

"+"             { return PLUS; }
"-"             { return MINUS; }
"*"             { return MUL; }
"/"             { return DIV; }
"%"             { return MOD; }

"="             { return ASSIGN; }

"("             { return LPAREN; }
")"             { return RPAREN; }
"{"             { return LBRACE; }
"}"             { return RBRACE; }
";"             { return SEMI; }
","             { return COMMA; }
":"             { return COLON; }

\'[^\']\'             { yylval.i = yytext[1]; return CHAR_LITERAL; }
[0-9]+"."[0-9]*     { yylval.f = atof(yytext); return FLOAT_LITERAL; }
[0-9]+              { yylval.i = atoi(yytext); return INT_LITERAL; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.id = strdup(yytext); return ID; }

\n         { yylineno++; column = 1; }
[ \t\r]+   { /* ignore tabs and spaces */ column += yyleng; }

. {
    fprintf(stderr, "Lexical Error at line %d, column %d: Unknown character '%c'\n", yylineno, column, *yytext);
}

%%

/* Subroutines */

int yywrap() {
    return 1;
}
