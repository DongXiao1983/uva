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
