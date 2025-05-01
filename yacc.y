/* Definitions */
%{
    #include <stdio.h>
    int yylex(void);
    void yyerror(char *);
%}

/* Production rules */
%%

%%


/* Subroutines */
void yyerror(char *s) {
    fprintf(stderr, "%s\n", s);
}

int main(int argc, char **argv) {
    if (argc > 1) {
        yyin = fopen(argv[1], "r");
        if (!yyin) {
            perror("Error opening file");
            return 1;
        }
    }

    if (yyparse() == 0){
        return 0;
    }

    return 1;
}