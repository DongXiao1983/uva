     1	#include <iostream>
     2	#include <string>
     3	#include <cstring>
     4	#include <map>
     5	#include <set>
     6	using namespace std;
     7	
     8	int main()
     9	{
    10	  set<string> keyword;
    11	  string igorn;
    12	  char   title[1000];
    13	  map<string, char[1000]> title_table;
    14	  while(cin >> igorn){
    15	    if ( igorn != "::" )
    16	      keyword.insert(igorn);
    17	  }
    18	  while (cin.getline(title, 1000,'\n')){
    19	    for(int i = 0 ; i < strlen(title);i++){
    20	      string key = "";
    21	      if(title[i]==' ')
    22		continue;
    23	      while(title[i] != ' ')
    24		key+=(title[i++]);
    25	      title_table.insert(key,title);
    26	    }
    27	  }
    28	}
