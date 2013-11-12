     1	
     2	
     3	
     4	
     5	
     1	#include <iostream>
     2	#include <stack>
     3	#include <algorithm>
     4	#include <string>
     5	
     6	using namespace std;
     7	
     8	
     9	int main()
    10	{
    11	  stack<char>  input;
    12	  stack<int>   value;
    13	  int target ; 
    14	
    15	  char s;
    16	  int  pos;
    17	  int is_negative = 1;
    18	  cin >> target;
    19	
    20	  int process_num = 0;
    21	  int tmp_value = 0;
    22	  do{
    23	    do {
    24		  cin >> s;
    25		}while( s == ' ' || s == '\r' || s == '\n' || s == '\t');
    26	
    27		//input.push(s);
    28	    if ( s == '('){
    29		  if ( process_num  ){
    30			if (value.empty()){
    31			  value.push(tmp_value * is_negative);
    32			}
    33			else{
    34			  value.push(value.top()+tmp_value * is_negative);
    35			}
    36			input.push('0');
    37			is_negative = 1;
    38			tmp_value = 0;
    39			process_num  = 0;
    40			//  cout << "#" << value.top() << "--" << tmp_value << " ";
    41		  }
    42		  input.push(s);
    43		}    
    44		else if ( s >='0' && s<='9'){
    45		
    46			tmp_value = 10*tmp_value + (s-'0'); 
    47	        process_num = 1;
    48			//	cout << "--->" << tmp_value << "<---";
    49		  
    50		}
    51		else if ( s == ')'){
    52		  if(input.top()=='(') {
    53			pos++;
    54			input.pop();
    55				cout << value.top() << " " ;
    56		  }
    57		  else if ( input.top() >= '0' && input.top() <= '9'){
    58			  input.pop();
    59			  input.pop();
    60			  value.pop();
    61			}	  
    62		}
    63		else if ( s == '-' ){
    64		  is_negative = -1;
    65		}
    66		else 
    67		  input.push(s);
    68	  }while(!input.empty());
    69	
    70	  
    71	}
    72	//22 (5(14(1(7()())(2()()))()) (8(3()())(4()(1()()))))
