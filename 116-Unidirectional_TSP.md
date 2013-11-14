### 116 - Unidirectional TSP
#### Link http://uva.onlinejudge.org/index.php?option=com_onlinejudge&Itemid=8&category=3&page=show_problem&problem=52

It was a classical problem.
     1	#include <cstdio>
     2	#include <cstring>
     3	#include <iostream>
     4	using namespace std;
     5	
     6	#define INF 1 << 30;
     7	
     8	int row, col;
     9	int matrix[11][101] = { 0 } ;
    10	int vis[11][101]    = { 0 } ;
    11	int dp[11][101]     = { 0 } ;
    12	int path[11][101]   = { 0 } ;
    13	int dpf(int i, int j)
    14	{
    15	  if ( j >= col )
    16		return 0;
    17	  if ( vis[i][j] == 1)
    18		return dp[i][j];
    19	  vis[i][j] = 1;
    20	  
    21	  int dir[3] = {-1,0,1};
    22	  int min = INF;
    23	  int tmp;
    24	  for ( int k = 0; k < 3 ; k++ ){
    25		int r = ( row + i + dir[k] ) % row ;
    26		tmp = dpf(r,j+1) + matrix[i][j];
    27		if (tmp < min ){
    28		  min = tmp;
    29		  path[i][j] = r;
    30		}
    31	  }
    32	  dp[i][j] = min;
    33	
    34	  return min;
    35	}
    36	
    37	int main()
    38	{
    39	  int i,j, start = 1, ans = INF;
    40	
    41	  while(~scanf("%d%d", &row, &col)){
    42		memset(matrix,0,sizeof(matrix));
    43		memset(vis,0,sizeof(vis));
    44		memset(dp,0,sizeof(dp));
    45		ans = INF;
    46		for ( i = 0 ; i < row  ; i++ ){
    47		  for ( j = 0 ; j < col ;j++){
    48			scanf("%d", &matrix[i][j]);
    49		  }
    50		}
    51		for ( i = 0 ; i < row ; i++) {
    52		  int tmp = dpf(i,0);
    53		  if ( tmp  <  ans ) {
    54			ans = tmp;
    55			start = i;
    56		  }
    57		}
    58	
    59		
    60		
    61	    cout << start+1 ;
    62		for ( i = 0 ; i < col-1; i++ ){
    63		  
    64		  printf( " %d", path[start][i]+1);
    65		  start = path[start][i];
    66		}
    67		cout << endl << ans << endl;
    68	  }
    69	  
    70	}
