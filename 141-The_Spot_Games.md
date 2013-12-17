	#include <iostream>
	#include <vector>
	using namespace std;
	int matrix[50][50];
	
	typedef step_t{
	  int m1[50][50];
	  int m2[50][50];
	  int m3[50][50];
	  int m4[50][50];
	  int times = 0;
	}step;
	
	vector<step> s;
	int main()
	{
	  int n;
	  int times = 1;
	  int x, y;
	  char spot;
	  
	  while(cin >> n){
	    if ( n == 0) break;
	
	    while ( cin >> x >> y >> spot ){
	      step s ;
	      if ( spot == '+') matrix[x][y] = 1;
	      if ( spot == '-') matrix[x][y] = 0;
	      vector<step>::iterator it;
	      for ( it = s.begin(); it != s.end(); it++){
		if ((it->m1 == matrix ) || (it->m2 == matrix) || (it->m3 == matrix) || (it->m4 == matrix)){
		  cout << "Player " << it->times-1 << " is winer" << endl;
		  break;
		}
		s.push_back(s);
	      }
	    }
	
	  }
	}
