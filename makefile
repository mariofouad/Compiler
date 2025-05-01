all:
	@bison -d -v yacc.y
	@flex lex.l
	@gcc yacc.tab.c lex.yy.c -o compiler.exe
