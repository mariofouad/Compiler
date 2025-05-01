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

"int"           { return INT; }
"float"         { return FLOAT; }
"char"          { return CHAR; }
"double"        { return DOUBLE; }
"bool"          { return BOOL; }
"string"        { return STRING; } 
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
\"([^\\\"]|\\.)*\"    { yylval.id = strdup(yytext); return STRING_LITERAL; }
[0-9]+"."[0-9]*     { yylval.f = atof(yytext); return FLOAT_LITERAL; }
[0-9]+              { yylval.i = atoi(yytext); return INT_LITERAL; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.id = strdup(yytext); return ID; }

[ \t\r\n]+          { /* ignore whitespace */ }

.                   { fprintf(stderr, "Unknown character: '%c'\n", *yytext); }

%%

/* Subroutines */

int yywrap() {
    return 1;
}
