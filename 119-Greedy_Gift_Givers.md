     1	#include <iostream>
     2	#include <string>
     3	using namespace std;
     4	
     5	#define PEOPLE  11
     6	int num;
     7	string person[PEOPLE];
     8	int found_person(string name)
     9	{
    10	  for ( int i = 0 ; i < num ; i++ ){
    11	    if ( name == person[i])
    12	      return i;
    13	  }
    14	}
    15	
    16	int main()
    17	{
    18	
    19	 
    20	  int price[PEOPLE] = {0};
    21	
    22	
    23	   
    24	  while ( cin >> num ){
    25	    for ( int i = 0 ; i < PEOPLE; i++ )  person[i]="";    
    26	    for ( int i = 0 ; i < num ; i++ ){
    27	      cin >> person[i];
    28	    }
    29	    string one;
    30	    int    spent;
    31	    int    count;
    32	    while ( cin >> one >> spent >> count ){
    33	      int index = found_person(one);
    34	
    35	      if (spent == 0)
    36		continue;
    37	      int onw   = spent / count;
    38	      price[index] -= (onw * count);
    39	      for ( int i = 0 ; i < count ; i++ ){
    40		string fiends;
    41		cin >> fiends;
    42		int t = found_person(fiends);
    43	
    44		price[t] += onw;
    45	      }
    46	    }
    47	
    48	    for ( int i = 0 ; i < num   ;i++ ){
    49	      cout << person[i] << " " << price[i] << endl;
    50	    }
    51	  }
    52	
    53	}
