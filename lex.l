/* Definitions */
%{
    #include "y.tab.h"
    #include <stdlib.h>
    void yyerror(char *);
%}

/* Regex rules */
%%

%%

/* Subroutines */
int yywrap(void) {
    return 1;
}
