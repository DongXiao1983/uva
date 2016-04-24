#include "expression.h"


char* ch;


int exp_get_next() 
{
    while (*ch == ' ')
      ++ch;
    if ( *ch++ == '(')
      return exp_caculate();
    else
      return strtol (ch-1, &ch, 10) ; 
}


int exp_caculate() 
{
   int  left_value, right_value ;
   left_value = exp_get_next();
   
   for (;;) {
      while (*ch == ' ') 
         ++ch;
      
      int op = *ch++;
      
      if (op == 0 || op == ')') 
         return left_value;
      
      if (op == '<' || op == '>')
         ch++;

      
      right_value = exp_get_next();
      
      switch (op) {
         
         case '+' :  left_value  +=  right_value  ; break;
         case '-' :  left_value  -=  right_value  ; break;
         case '*' :  left_value  *=  right_value  ; break;      
         case '|' :  left_value  |=  right_value  ; break;
         case '&' :  left_value  &=  right_value  ; break;
         case '^' :  left_value  &=  right_value  ; break;
         case '<' :  left_value <<=  right_value  ; break;
         case '>' :  left_value >>=  right_value  ; break;
         case '/' :  
            right_value ? left_value  /=  right_value :left_value ; 
            break;
            
         default: break;
      }  
   }
}

char *exp_pre_process(char* exp)
{
   //should construct the operate priority before
   return exp;
}

extern int exp_eval(char* exp) 
{
    int result = 0;
    ch = exp_pre_process(exp);
    result = exp_caculate();
    return result;
}

