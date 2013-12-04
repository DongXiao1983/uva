     1	#include <stdio.h>
     2	#include <string.h>
     3	#include <stdlib.h>
     4	const int N = 1030;
     5	const int g = 34943;
     6	const int tmp = 256;
     7	
     8	
     9	int main()
    10	{
    11	  char num[N],c[]="0123456789ABCDEF";
    12	  int sign[4];
    13	
    14	  while(gets(num) && num[0] != '#'){
    15		unsigned long long ans ,t = 0;
    16		for ( int i = 0 ; num[i]; i++ )
    17		  t = ( t * tmp + num[i] ) % g;
    18		t = (t * tmp * tmp) %g;
    19		ans = g-t;
    20		for ( int i = 0; i < 4;i++ ){
    21		  sign[i] = ans % 16;
    22		  ans = ans / 16;
    23		}
    24		printf("%c%c %c%c\n",c[sign[3]],c[sign[2]],c[sign[1]], c[sign[0]]);
    25	  }
    26	}
