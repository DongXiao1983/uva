#ifndef _EXPRESSION_H_
#define _EXPRESSION_H_


#include <stdlib.h>


/* Evaluate the Expression , just a simple way, not consider the operation priority */

int exp_get_next();

int exp_caculate();

extern int exp_eval(char* exp) ;

#endif

