#include <iostream>
#include <string>
#include <cstring>
#include <stack>

using namespace std;
stack<string>  piple[52];
int adjust(string card, int start)
{
  if ( start == 0 )
	return 0;
  string left1 = piple[start-1].top();
  cout << left1;
  if ( left1[0] == card[0] || left1[1] == card[1]){
	piple[start].pop();
	piple[start-1].push(card);
    if (!piple[start].empty())
	  start = adjust(piple[start].top(),start);
	else
	  start = adjust(piple[start-1].top(), start-1);
  }
  else if ( start -3 >= 0 ){
	string left1 = piple[start-3].top();
	if ( left1[0] == card[0] || left1[1] == card[1]){
	piple[start].pop();
	piple[start-1].push(card);
    if (!piple[start].empty())
	  start = adjust(piple[start].top(),start);
	else
	  start = adjust(piple[start-1].top(), start-1);
	}
  }
  return start;
}
int main()
{
  string card;
  int start = 0;
  //memset(piple,0,sizeof(piple));
  while(cin >> card ){
	if (card == "#" ) break;
	piple[start].push(card);
	start = adjust(card, start) + 1;
  }
  for ( int i = 0 ; i < 52 ;i++ ){
	cout << piple[i].size() << " ";
  }
}
#include <iostream>
#include <string>
#include <cstring>
#include <stack>

using namespace std;
stack<string>  piple[52];
int adjust(string card, int start)
{
  if ( start == 0 )
	return 0;
  string left1 = piple[start-1].top();
  string left3 ="";
  if ( start-3>=0)
	left3 = piple[start-3].top();
  
  if (( left1[0] == card[0] || left1[1] == card[1])&&(start>=3)&&
	  (left3[0] == card[0] || left3[1] == card[1])){
	piple[start].pop();
	piple[start-3].push(card);
    if (!piple[start].empty())
	  start = adjust(piple[start].top(),start);
	else
	  start = adjust(piple[start-3].top(), start-3);
  }
  else if ( ( left1[0] == card[0] || left1[1] == card[1]) || ((start>=3)&&
															  (left3[0] == card[0] || left3[1] == card[1]) )){
	piple[start].pop();
	piple[start-1].push(card);
    if (!piple[start].empty())
	  start = adjust(piple[start].top(),start);
	else
	  start = adjust(piple[start-1].top(), start-1);
  }
  return start;
}
int main()
{
  string card;
  int start = 0;
  int i = 0;
  //memset(piple,0,sizeof(piple));
  while(cin >> card ){
	if (card == "#" ) break;
    for (  i = 0 ; i < 52 && !piple[i].empty() ;i++ );
	 piple[i].push(card);
	 adjust(card, i) ;
  }
  for ( int i = 0 ; i < 52 ;i++ ){
	cout << piple[i].size() << " ";
    while( !piple[i].empty()) {
	  cout << piple[i].top() << " ";
	  piple[i].pop();
	}
	cout << endl;
	  
  }
}
