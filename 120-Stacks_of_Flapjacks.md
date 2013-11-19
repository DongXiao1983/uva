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
    25	
    26	}
    27	
    28	int flappick(int n)
    29	{
    30	  if ( 0 == n )
    31		return 0;
    32	  int max = 0;
    33	  int   i = 0;
    34	  for ( i = 1 ; i < n ; i++ ) {
    35		if( pank[i]>pank[max]){
    36		  max = i;
    37		}
    38	  }
    39	  
    40	  if ( max+1 != n ){
    41		cout << total - n +max << " ";
    42		revert(max,n);
    43		flappick(n-1);
    44	  }
    45	
    46	}
    47	
    48	int main()
    49	{
    50	  while (cin.getline(input, MAX, '\n')){
    51	    memset(pank,0,31);
    52	    cout << input <<  endl;
    53	    int count = 0;
    54	    int i = 0;
    55	    for ( i =0 ; input[i] == ' ' ; i++ ) ;
    56	    for ( i; i < strlen(input) ; i++){
    57	      if ( input[i] == ' '){
    58		    count++;
    59	      }
    60	      else {	
    61		    pank[count] = 10 * pank[count] + (input[i]-'0');
    62	      }
    63	    }
    64	
    65	    total = count+2;
    66		flappick(count+1);
    67		cout << 0 << endl;
    68	  }
    69	 
    70	  
    71	  return 0;
    72	}
