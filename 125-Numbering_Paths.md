     1	#include <iostream>
     2	#include <cstring>
     3	#include <string>
     4	
     5	using namespace std;
     6	
     7	int main()
     8	{
     9	
    10	  int    count;
    11	  int    times = 0;
    12	  while(  cin >> count ){
    13	  int    street[30][30] = { 0 };
    14	  string input;
    15	  int    tail = 0;
    16	  
    17	  memset(street, 0 , sizeof(street));
    18	  for ( int i = 0 ; i < count; i++){
    19	    int start,end;
    20	    cin >> start >> end;
    21	    street[start][end]=1;
    22	    tail = max(tail,max(start,end));
    23	  }
    24	  
    25	  tail += 1;
    26	  for ( int k = 0 ; k < tail ; k++ ){
    27	    for ( int i = 0 ; i < tail; i ++ ) {
    28	      for ( int j = 0 ; j < tail ; j++) {
    29	        street[i][j] += street[i][k] * street[k][j];
    30	      }
    31	    }
    32	  }
    33	  
    34	  for ( int k = 0 ; k < tail ; k++ ){
    35	    if ( street[k][k]  ) {
    36	      for ( int i = 0 ; i < tail ; i++){
    37		for ( int j = 0 ; j < tail ; j++ ){
    38		  if (street[i][k] * street[k][j] )
    39		    street[i][j] = -1;
    40		}
    41	      }
    42	    }
    43	  }  
    44	
    45	  cout << "matrix for city " << times++ << endl;
    46	  for ( int i = 0 ; i < tail ; i++ ){
    47	    for ( int j = 0 ; j < tail ;  j++ ){
    48	      cout << street[i][j] << " ";
    49	    }
    50	    cout << endl;
    51	  }
    52	  }
    53	}
    54	
    55	
    56	
