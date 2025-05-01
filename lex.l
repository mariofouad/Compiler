/* Definitions */
%{
#include "yacc.tab.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
void yyerror(const char *);
%}

/* Regex rules */
%%

"if"        { return IF; }
"then"      { return THEN; }
"else"      { return ELSE; }
"while"     { return WHILE; }
"do"        { return DO; }
"int"       { return INT; }
"float"     { return FLOAT; }
"return"    { return RETURN; }

"="         { return ASSIGN; }
"=="        { return EQ; }
"!="        { return NEQ; }
"<="        { return LE; }
">="        { return GE; }
"<"         { return LT; }
">"         { return GT; }
"+"         { return PLUS; }
"-"         { return MINUS; }
"*"         { return MUL; }
"/"         { return DIV; }
"("         { return LPAREN; }
")"         { return RPAREN; }
"{"         { return LBRACE; }
"}"         { return RBRACE; }
";"         { return SEMI; }
","         { return COMMA; }

[0-9]+"."[0-9]*     { yylval.f = atof(yytext); return FLOAT_NUM; }
[0-9]+              { yylval.i = atoi(yytext); return INT_NUM; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.id = strdup(yytext); return ID; }

[ \t\r\n]+          { /* ignore whitespace */ }

.                   { fprintf(stderr, "Unknown character: '%c'\n", *yytext); }

%%

/* Subroutines */
int yywrap() {
    return 1;
}
