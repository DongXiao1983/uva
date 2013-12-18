	#include <iostream>
	#include <string>
	#include <cstring>
	#include <set>
	
	using namespace std;
	
	int map[50][50];
	char map_store[4][2500];
	char c ;
	int  n ;
	set<string> matrix; 
	void save()
	{
	  memset(map_store, 0, sizeof(map_store));
	  int k = 0;
	  int i = 0;
	  int j = 0;
	  for ( i = 0 ; i < n ; i++ )
		for (  j = 0 ; j < n ; j++ )
		  map_store[0][k++] = map[i][j] + '0';
	  k = 0;
	  for ( i = 0 ; i < n ; i ++ )
		for ( j = 0 ; j < n ; j++ )
		  map_store[1][k++] = map[n-i][n-j] + '0';
	
	  k = 0;
	  for ( i = 0 ; i < n ; i++ )
		for ( j = 0 ; j < n ; j ++ )
		  map_store[2][k++] = map[j][n-i] +'0';
	
	  k = 0;
	  for ( i = 0 ; i < n; i++ )
		for ( j = 0 ; j < n ; j++ )
		  map_store[3][k++] = map[i][n-j] + '0';
	}
	
	int main()
	{
	
	  while ( cin >> n){
		if (n == 0) break;
		memset(map,0,sizeof(map));
		matrix.clear();
		int judge = 0;
		int step  = 0;
		int st    = 1;
		int x     = 0 ;
		int y     = 0;
		while(cin >> x >> y >> c && st < 2 * n){
		  
		  if (c == '+') map[x][y] = 1;
		  if (c == '-') map[x][y] = 0;
		  save();
		  for ( int i = 0 ; i < 4 ; i++ ){
			if ( matrix.find(map_store[i]) != matrix.end()){
			  judge = 1;
			  break;
			}
		  }
		  for ( int j = 0  ; j < 4 ; j++ ){
			matrix.insert(map_store[j]);
		  }
		  if ( judge ){
			if (step == 0)
			  step = st + 1;
	
		  }
		  st++;
		}
		if ( judge ){
		  if ( step %2 )
			cout << "Player 2 wins on move "<< step << endl;
		  else
			cout << "Player 1 wins on move "<< step << endl;
		}
		else
		  cout << "Draw" << endl;
	  }
	  return 0;
	}
