all:
    bison -d -v yacc.y
    flex lex.l
    gcc Parser.tab.c lex.yy.c -o compiler.exe