     1	#include <iostream>
     2	using namespace std;
     3	
     4	int skew(double x, double y)
     5	{
     6	  const double sqrt3 = 0.866;
     7	  int lines = (int ) (1 + (y- 1 )/sqrt3);
     8	  if ( x < ((int)x+0.5) )
     9		return ((int)x * lines - lines / 2);
    10	  else
    11	    return ((int)x * lines);
    12	}
    13	
    14	int main()
    15	{
    16	  double width = 0.0;
    17	  double high  = 0.0;
    18	  while ( cin >> width >> high ){
    19		int grid = (int)width * (int) high;
    20		int skews = max(skew(width,high),skew(high,width));
    21		cout << max(grid, skews) << ( grid >= skews ? " grid":" skew" ) << endl;
    22	  }
    23	}
