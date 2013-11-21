 1  #include <iostream>
 2  #include <string>
 3  #include <cstring>
 4  #include <algorithm>
 5  #include <map>
 6  #include <set>
 7  
 8  using namespace std;
 9  
10  int main()
11  {
12    string line;
13    set<string> ignore;
14    set<string>::iterator it;
15    typedef  multimap<string, string> title_m ;
16    title_m title ;
17  
18    while(!cin.eof()) {
19      getline(cin,line);
20      if ( line == "::")
21        break;
22      transform(line.begin(),line.end(),line.begin(),::tolower);
23      ignore.insert(line);
24    }
25    while(!cin.eof()){
26      getline(cin,line);
27      
28  
29      for ( int i = 0 ; i < line.length(); i++ ){
30        transform(line.begin(),line.end(),line.begin(),::tolower);
31        string word ="";
32        int start   =  i;
33        if ( line[i] == ' ')
34          continue;
35        while(line[i] != ' ' && i < line.length())
36          word += (line[i++]);
37  
38        if ( ignore.find(word) != ignore.end())
39          continue;
40        transform(word.begin(),word.end(),word.begin(),::toupper);
41        line.replace(start,i-start,word);
42        transform(word.begin(),word.end(),word.begin(),::tolower);
43        title.insert(pair<string,string>(word,line));
44      }
45      for ( title_m::iterator it = title.begin(); it != title.end(); it++) {
46        cout <<  it->second << endl;  
47      }
48    }
49  }
