     1	#include <iostream>
     2	#include <vector>
     3	
     4	using namespace std;
     5	
     6	int main()
     7	{
     8	  int n, k;
     9	  while ( cin >> n >> k && n*k != 0){
    10		vector<int> circle;
    11		int index = 0;
    12		for ( int i = 0 ; i < n ; circle.push_back(++i) );
    13		
    14		
    15		while ( circle.size() != 1 ) {
    16		  int victim = ( index + k - 1 ) % circle.size();
    17		  int bury   = (victim + k  ) % circle.size();
    18		  circle[victim] = circle[bury];
    19		  circle.erase(circle.begin() + bury);
    20		  if ( bury < victim ) (victim = (victim-1) %circle.size());
    21		  index = (victim + 1) % circle.size();
    22		}
    23		cout  << circle[0] << endl;
    24	  }
    25	}
