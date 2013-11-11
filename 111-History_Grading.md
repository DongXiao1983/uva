###Problem Link []http://uva.onlinejudge.org/index.php?option=com_onlinejudge&Itemid=8&category=3&page=show_problem&problem=47
It was Longest increase substring problem.
The DP algorithm is O(n^2)
There is a better algoritm is O(nlogn), there different is to find out the max value.

     1	#include <iostream>
     2	#include <algorithm>
     3	using namespace std;
     4	
     5	int main()
     6	{
     7	  int array[10] = {0};
     8	  int dp[10]    = {1,1,1,1,1,1,1,1,1,1};
     9	  int count, i = 0;
    10	  int mark = 1;
    11	  cin >> count;
    12	  while ( i < count ){
    13		cin >> array[i++];
    14	  }
    15	#if 0
    16	  int current = 0;
    17	  for ( int i = 0 ; i < count ; i++ ){
    18		mark = 1;
    19		if (dp[i] >  mark )
    20		  continue;
    21		current = dp[i];
    22		for ( int j = i+1 ; j < count ; j++){
    23		  if ( array[j] > current && dp[j] < mark){
    24			dp[j] = ++mark;
    25			current = array[j];
    26		  }
    27		}
    28	  }
    29	
    30	#endif
    31	  dp[0]=1;
    32	  for ( int i = 2 ; i < count ; i++ ){
    33		dp[i] = 1;
    34		for ( int j = i;j>=0 ; j-- ){
    35		  if ( array[i] > array[j] )
    36			dp[i] = max(dp[i],dp[j]+1);
    37		}
    38	  }
    39	  for ( int i = 0 ; i < count ; i++)
    40		cout << dp[i] << "  ";
    41	  cout << endl;
    42	  return 0;
    43	}
