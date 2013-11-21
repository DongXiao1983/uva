     1	#include <iostream>
     2	#include <string>
     3	#include <vector>
     4	#include <algorithm>
     5	using namespace std;
     6	
     7	
     8	int main()
     9	{
    10	  string input;
    11	
    12	  while(!cin.eof()){
    13	    char speci[21] = {0};
    14	    bool compare[26][26] = {0};
    15	    getline(cin, input);
    16	
    17	    int cnt = 0;
    18	    for ( int i = 0 ; i < input.length(); i++ ){
    19	      if (input[i]!=' ')
    20		speci[cnt++] = input[i];
    21	    }
    22	
    23	    getline(cin , input);
    24	    
    25	    input.erase(remove(input.begin(),input.end(),' '),input.end());
    26	    
    27	    for ( string::iterator i = input.begin(); i != input.end(); i+=2){
    28	      compare[*i-'a'][*(i+1)-'a'] = true;
    29	    }
    30	    
    31	    sort(speci, speci+cnt);
    32	    for ( bool flag = true; flag; flag =  next_permutation(speci,speci+cnt)){
    33	       for ( int i = 0 ; i < cnt && flag; i++ ){
    34		 for ( int j = i + 1; j < cnt && flag;j++) 
    35		   flag &= !compare[speci[j]-'a'][speci[i]-'a'];
    36	       }
    37	       if (flag )
    38		 cout << speci << endl;
    39	    }
    40	  }
    41	}
