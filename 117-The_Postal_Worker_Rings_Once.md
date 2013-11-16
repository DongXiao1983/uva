if no odd node, just sum all path, if have odd node, it mush be pair. so get there two node's sortest path length

     1	#include <iostream>
     2	#include <string>
     3	#include <cstdio>
     4	#include <memory.h>
     5	using namespace std;
     6	#define INT_MAX 1<<10
     7	int    city[26][26] = { 0 };
     8	int    cross[26]    = { 0 };
     9	int    path[26][26] = { 0 };
    10	int floyd(int start, int end)
    11	{
    12	  for ( int i = 0 ; i < 26 ; i++ )
    13		city[i][i]=  0;
    14	  for( int k = 0 ; k < 26 ; k++ ){
    15		for ( int i = 0 ; i < 26 ;i++ ){
    16		  for ( int j = 0 ; j < 26 ; j++ ){
    17			city[i][j] = min (city[i][j] , city[i][k] + city[k][j]);
    18		  }
    19		}
    20	  }
    21	  return city[start][end];
    22	}
    23	
    24	
    25	int main()
    26	{
    27	  string street;
    28	  char   head,tail;
    29	  int    len ,sum;
    30	  int    start, end;
    31	  int    eulr = 1;
    32	  memset(city, 9999, sizeof(city));
    33	
    34	  while ( cin >> street && street != "deadend") {
    35		head = street[0];
    36		tail  = street[street.length()-1];
    37	    
    38		len  = street.length();
    39	    city[head-'a'][tail-'a'] = len;
    40	    city[tail-'a'][head-'a'] = len;
    41	
    42		cross[head-'a']++;
    43		cross[tail-'a'] ++;
    44		sum += len;
    45	  } 
    46	
    47	  for ( int i = 0 ; i < 26 ; i++ ){
    48		if (cross[i] & 1 == 1){
    49		  if ( eulr == 1)
    50			start = i;
    51		  else
    52			end = i;
    53		  eulr = 0;
    54		}
    55	  }
    56	
    57	  if (eulr) {
    58		cout << sum << endl;
    59		return 0;
    60	  }
    61	
    62	  sum += floyd(start, end);
    63	
    64	  cout << sum << endl;
    65	  return 0;
    66	}
