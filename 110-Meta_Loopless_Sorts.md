####这是个递归全排列的输出问题，  
从集合里只有一个元素开始，每次添加一个元素，插入到上个集合每一个元素的每一个位置。共N！输出  
1 elements:  a   
2 elements:  ab    ba  
3 elements:  abc   acb  cab  bac bca  cba  

     1	#include <stdio.h>
     2	#include <iostram>
     3	#include <algrothm>
     4	using namespace std;
     5	
     6	void writeln(string &str)
     7	{
     8	  cout << "writelen(" << str[0];
     9	  for ( int i = 1 ; i < str.size(); i++ ){
    10		cout << "," << str[i];
    11	  }
    12	  cout << ")" << endl;
    13	}
    14	
    15	void mateloop_sort(int start, int end , string &str)
    16	{
    17	  if (start == end){
    18		writeln(str);
    19		return;
    20	  }
    21	  for ( int i = 0 ; i < str.size() ; i++){
    22		cout << "if " << str[start-1] << " < " << str[start] << " then" << endl;
    23		str.insert(i,'a'+i,1);
    24	  }
    25	
    26	  
    27	  count << "else"
    28	}
    29	
    30	int main()
    31	{
    32	  int num;
    33	  int i ;
    34	  char var[7]={"abcdef"};
    35	  cout << "program sort(input,output);" << endl;
    36	  cout << "var" << endl;
    37	
    38	  cin >> num;
    39	  for (  i = 0; i < num-1 ; i++)
    40		cout << var[i] << ",";
    41	  cout << var[i] << " : integer;" << endl;
    42	  cout << "begin" << endl;
    43	  cout << "readline(";
    44	  for ( i = 0 ; i < num -1;i++)
    45		cout << var[i] << ",";
    46	  cout << var[i] << ")" << endl;
    47	  
    48	  string str("a");
    49	  mateloop_sort(1,num, str);
    50	
    51	  cout << "end" << endl;
    52	
    53	  return 0;
    54	}
