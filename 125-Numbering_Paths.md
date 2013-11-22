#include <iostream>
#include <cstring>
#include <string>

using namespace std;

int main()
{

  int    count;
  int    times = 0;
  while(  cin >> count ){
  int    street[30][30] = { 0 };
  string input;
  int    tail = 0;
  
  memset(street, 0 , sizeof(street));
  for ( int i = 0 ; i < count; i++){
    int start,end;
    cin >> start >> end;
    street[start][end]=1;
    tail = max(tail,max(start,end));
  }
  
  tail += 1;
  for ( int k = 0 ; k < tail ; k++ ){
    for ( int i = 0 ; i < tail; i ++ ) {
      for ( int j = 0 ; j < tail ; j++) {
        street[i][j] += street[i][k] * street[k][j];
      }
    }
  }
  
  for ( int k = 0 ; k < tail ; k++ ){
    if ( street[k][k]  ) {
      for ( int i = 0 ; i < tail ; i++){
	for ( int j = 0 ; j < tail ; j++ ){
	  if (street[i][k] * street[k][j] )
	    street[i][j] = -1;
	}
      }
    }
  }  

  cout << "matrix for city " << times++ << endl;
  for ( int i = 0 ; i < tail ; i++ ){
    for ( int j = 0 ; j < tail ;  j++ ){
      cout << street[i][j] << " ";
    }
    cout << endl;
  }
  }
}



