#include <iostream>
#include <string>
#include <algorithm>
#include <cstring>
using namespace std;

#define MAX 1000
char  input[1000];
int   pank[31] = { 0};
int main()
{
  while (cin.getline(input, MAX, '\n')){
    memset(pank,0,31);
    cout << input <<  endl;
    int count = 0;
    int i = 0;
    for ( i =0 ; input[i] == ' ' ; i++ ) ;
    for ( i; i < strlen(input) ; i++){
      if ( input[i] == ' '){
	count++;
      }
      else {	
	  pank[count] = 10 * pank[count] + (input[i]-'0');
      }
    }

    for ( int i = 0 ; i <= count ; i++ )
       cout << pank[i] << " ";
  }
  return 0;
}
