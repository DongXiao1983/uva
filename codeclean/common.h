#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdlib.h>

#undef FLASE
#undef TRUE
typedef enum { FALSE, TRUE } boolean;


/* memory alignment macro's */
#define ALIGN(X,A)  (((X) + ((A) - 1)) & ~((A) - 1))
#define FLOOR(X,A)  ((X) & ~((A) - 1))


#define isspacetab(c)         ((c) == SPACE || (c) == TAB)
#define is_number(c)          ((c) >='0' && (c) <= '9' ) 
#define is_math_opt(c)        ((c) == '+' || ((c) == '-') || ((c) == '*') || ((c) == '/') || ((c) == '^') )
#define is_bit_opt(c)         ((c) == '<' || (c) == '>') 
#define is_logi_opt(c)        ((c) == '&' || (c) == '|')

#define is_math_expression(c)   (is_number(c)  || is_math_opt(c)||   \
                                 is_logi_opt(c)|| is_bit_opt(c) ||   \
                                 isspacetab(c) ||((c) == '(')||((c) == ')'))

#define is_alph(c)           (( (c) >= 'A' && (c) <= 'Z') || ((c)>='a' && (c) <= 'z'))
#define SET_MAX_VALUE( X, V) ( (X) = ((X) < (V) ? (V):(X)))

#endif




