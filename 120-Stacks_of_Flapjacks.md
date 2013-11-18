#include <iostream>
#include <string>
#include <algorithm>
#include <cstring>
using namespace std;

#define MAX 1000
char  input[1000];
int   pank[31] = { 0};
int main()
{
  while (cin.getline(input, MAX, '\n')){
    memset(pank,0,31);
    cout << input <<  endl;
    int count = 0;
    int i = 0;
    for ( i =0 ; input[i] == ' ' ; i++ ) ;
    for ( i; i < strlen(input) ; i++){
      if ( input[i] == ' '){
	count++;
      }
      else {	
	  pank[count] = 10 * pank[count] + (input[i]-'0');
      }
    }

    for ( int i = 0 ; i <= count ; i++ )
       cout << pank[i] << " ";
  }
  return 0;
}
     1	#include <iostream>
     2	#include <string>
     3	#include <algorithm>
     4	#include <cstring>
     5	using namespace std;
     6	
     7	#define MAX 1000
     8	char  input[1000];
     9	int   total;
    10	int   pank[31] = { 0};
    11	void revert(int index, int n)
    12	{
    13	  int tmp[31] = { 0 };
    14	  int i = n;
    15	  int j = 0;
    16	  for ( i = n-1, j = 0 ; i > index ; i--, j++){
    17		tmp[j] = pank[i];
    18	  }
    19	  for ( i = 0 ; i <= index ; i++,j++ ){
    20		tmp[j] = pank[i];
    21	  }
    22	  for ( i = 0 ; i < n ; i++ ){
    23		pank[i] = tmp[i];
    24	  }
    25	#if 0
    26	 for ( int i = 0 ; i < n ; i++ )
    27	       cout << pank[i] << " ";
    28	#endif
    29	}
    30	
    31	int flappick(int n)
    32	{
    33	  if ( 0 == n )
    34		return 0;
    35	  int max = 0;
    36	  int   i = 0;
    37	  for ( i = 1 ; i < n ; i++ ) {
    38		if( pank[i]>pank[max]){
    39		  max = i;
    40		}
    41	  }
    42	  
    43	  if ( max+1 != n ){
    44		cout << total - n +max << " ";
    45		revert(max,n);
    46		flappick(n-1);
    47	  }
    48	
    49	}
    50	
    51	int main()
    52	{
    53	  while (cin.getline(input, MAX, '\n')){
    54	    memset(pank,0,31);
    55	    cout << input <<  endl;
    56	    int count = 0;
    57	    int i = 0;
    58	    for ( i =0 ; input[i] == ' ' ; i++ ) ;
    59	    for ( i; i < strlen(input) ; i++){
    60	      if ( input[i] == ' '){
    61		    count++;
    62	      }
    63	      else {	
    64		    pank[count] = 10 * pank[count] + (input[i]-'0');
    65	      }
    66	    }
    67	
    68	    total = count+2;
    69		flappick(count+1);
    70		cout << 0 << endl;
    71	  }
    72	 
    73	  
    74	  return 0;
    75	}
