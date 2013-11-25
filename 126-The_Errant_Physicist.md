     1	#include <iostream>
     2	#include <string>
     3	#include <vector>
     4	using namespace std;
     5	
     6	typedef struct item_t {
     7	  int flag;
     8	  int arg;
     9	  int x_exp;
    10	  int y_exp;
    11	}item;
    12	
    13	vector<item> first;
    14	vector<item> second;
    15	
    16	int main()
    17	{
    18	  string input;
    19	  while(!cin.eof()){
    20	    cin >> input;
    21	    input.erase(remove(input.begin(),input.end(),' ' ), input.end());
    22	    bool new_item = true;
    23	    bool at_flag  = true;
    24	    bool at_x     = true;
    25	    bool at_y     = true;
    26	    for(int i = 0 ; i < input.length(); i++ ){
    27	      if ( new_item){
    28	        item tmp;
    29	      }
    30	      else {
    31	        new_item = 0;
    32		if ( input[i] == '-'){
    33		  tmp.flag = -1;
    34		  at_flag = true;
    35		}
    36		else if ( input[i] >= '0' && input[i] <= '9' ){
    37		  if ( at_flag )
    38		    tmp.arg = tmp.arg * 10 + (iput[i] - '0');
    39		  else if ( at_x )
    40		    tmp.x_exp = tmp.x_exp * 10 + (input[i] - '0');
    41		  else if ( at_y )
    42		    tmp.y_exp = tmp.y_exp * 10 + (input[i] - '0');
    43		}
    44		else if ( input[i] == 'x' ){
    45		  at_flag = false;
    46		  at_x    = true;
    47		  at_y    = false;
    48		}
    49		else if ( input[i] == 'y' ){
    50		  at_flag = false;
    51		  at_x    = false;
    52		  at_y    = true;
    53		}
    54	      }
    55	    }
    56	  }
    57	}
