     1	#include <iostream>
     2	#include <string>
     3	#include <cstring>
     4	using namespace std;
     5	
     6	int main()
     7	{
     8	  string node;
     9	  int    tree[256] = {0};
    10	  while ( cin >> node ){
    11		if ( node != "()" ){
    12		  int value = 0;
    13		  int pos   = 0;
    14		  for ( int i = 0 ; i < node.length(); i++ ){
    15			if ( node[i] >= '0' && node[i] <= '9' )
    16			  value = value * 10 + (node[i]-'0');
    17			else if ( node[i] == ',' && node[i+1] == ')') {
    18			  pos = 0;
    19			}
    20			else if (node[i] == 'R') {
    21			  pos = pos * 2 + 2;
    22			}
    23			else if (node[i] == 'L'){
    24			  pos = pos * 2 + 1;
    25			}
    26			else if ( node[i] == ')') {
    27			 tree[pos] = value;
    28			}
    29			else if ( node[i] == '(' ){
    30			  pos = 0;value =0;
    31			}
    32		  }
    33	      
    34		}
    35		else {
    36		  int tag = 0;
    37		  for ( int i = 255; i >= 0 ;i--){
    38			if ( tree[i] && (tree[(i-1)/2] == 0)){
    39			  tag = 1;
    40			  break;
    41			}
    42		  }
    43	      if (tag ) {
    44			  cout << "not complete" << endl;
    45		  }
    46		  else{
    47			for ( int i = 0; i < 256; i++){
    48			  if ( tree[i] )
    49				cout << tree[i] << " ";
    50			}
    51			cout << endl;
    52		  }
    53	       memset(tree,0,sizeof(tree));
    54		   node = "";
    55		}
    56	  }
    57	}
