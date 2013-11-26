#include <iostream>
#include <algorithm>
#include <string>
#include <cstring>
#include <vector>
using namespace std;
	
typedef struct item_t {
  int flag;
  int arg;
  int x_exp;
  int y_exp;
}item;
	
vector<item> param[2];

vector<item> answer;
	

void multip()
{
  vector<item>:: iterator it, jt, at;
  
  for ( it = param[0].begin(); it != param[0].end(); it++ ){
	for ( jt = param[1].begin(); jt != param[1].end(); jt++ ){
	  item ans ;
	  bool merge = false;
      memset(&ans, 0, sizeof(ans));
	  ans.flag = it->flag * jt->flag;
	  ans.arg  = it->arg * jt->arg;
	  ans.x_exp = it->x_exp + jt->x_exp;
	  ans.y_exp = it->y_exp + jt->y_exp;
	  for ( at = answer.begin(); at != answer.end(); at++){
		if ( at->x_exp == ans.x_exp && at->y_exp == ans.y_exp ) {
		  at->arg += ans.arg*ans.flag;
		  ( at->arg > 0 ) ? (at->flag = 1) :(at-> flag = -1);
		  merge = true;
		  break;
		}
	  }
	  if ( !merge ){
		for (at = answer.begin(); at != answer.end(); at++ ){
		  if ( ans.x_exp > at->x_exp ) 
			break;
		  else if (( ans.x_exp == at->x_exp ) && (ans.y_exp < at->y_exp))
			break;
		}
		answer.insert(at,ans);
	  }
	 
	}
  }
}



int main()
{
  string input;
  int    two = 0;
  while(two < 2){
    cin >> input;
    input.erase(remove(input.begin(),input.end(),' ' ), input.end());
	if (input[0] != '-')
	  input = "+" + input;
    bool new_item = true;
    bool at_flag  = true;
    bool at_x     = true;
    bool at_y     = true;
	item tmp;
    for(int i = 0 ; i < input.length(); i++ ){
      if ( new_item){
        memset(&tmp,0,sizeof(tmp));
      }
      new_item = 0;
	  if ( input[i] == '-' || input[i] == '+'){
		if (input[i] == '-' )tmp.flag = -1;
		if (input[i] == '+' )tmp.flag = 1;
		if (input[i+1] == 'x' || input[i+1] == 'y' )  tmp.arg = 1;
	    at_flag = true;
     }
	 else if ( input[i] >= '0' && input[i] <= '9' ){
		  if ( at_flag )
		    tmp.arg = tmp.arg * 10 + (input[i] - '0');
		  else if ( at_x )
		    tmp.x_exp = tmp.x_exp * 10 + (input[i] - '0');
		  else if ( at_y )
		    tmp.y_exp = tmp.y_exp * 10 + (input[i] - '0');
	}
	else if ( input[i] == 'x' ){
		  at_flag = false;
		  at_x    = true;
		  at_y    = false;
		  if ( input[i+1] <'0' || input[i+1] > '9' ) tmp.x_exp = 1;
	}
	else if ( input[i] == 'y' ){
		  at_flag = false;
		  at_x    = false;
   		  at_y    = true;
		  if ( input[i+1] <'0' || input[i+1] > '9' ) tmp.y_exp = 1;
    }
	  if ( input[i+1] == '+' ||  input[i+1] == '-' || input[i+1]=='\0'){
		param[two].push_back(tmp);
		new_item = 1;
	  }
   }
	two++;
}

  multip();
  cout << endl;
  for ( vector<item>::iterator it = answer.begin(); it != answer.end(); it++){
	
	cout << " ";
	cout << " ";
	if (it->arg !=1)
	  for(int v  = it->arg;v > 0; v/=10,cout << " ") ;
	
    if ( it->x_exp != 0){
      cout << " ";
	  if ( it->x_exp == 1)
		cout << " ";
	  else
		cout << it->x_exp;
	}
	
	if ( it->y_exp != 0) {
	  cout << " ";
	  if ( it->y_exp == 1)
		cout << " ";
	  else
		cout << it->y_exp;
	}
    cout << " ";
  }
  cout << endl;
  for ( vector<item>::iterator it = answer.begin(); it != answer.end(); it++){
    
	cout << ((it->flag == 1)?"+":"-");
	cout << " ";
	((it->arg == 1) ?"":cout << (it->arg));
	if (it->x_exp == 0 && it->y_exp == 0)
	  cout << 1;
	else {
	it->x_exp == 0 ? cout<<"":cout << "x";
    for(int v  = it->x_exp;v > 0; v/=10,cout << " ") ;
	it->y_exp == 0 ? cout <<"" :cout << "y";
    for(int v  = it->y_exp;v > 0; v/=10,cout << " ") ;
	}
    cout << " ";
  }
  cout << endl;
}
