#108 - Maximum Matrix Sum
This are an DP problem, the target is decrease the Dim of matrix.
There're orignl was the max sub-array sum, computer complex is O(N).

#Sample Code:
    
     1	#include <iostream>
     2	using namespace std;
     3	
     4	int maxSubArray(int *ar, int len)
     5	{
     6	  int max = ar[0];
     7	  int b   = ar[0];
     8	  int i ;
     9	  for ( i = 1; i < len ; i++){
    10		if ( b > 0)
    11		  b += ar[i];
    12		else
    13		  b = ar[i];
    14		if (b > max)
    15		  max = b;
    16	  }
    17		return max;
    18	}
    19	
    20	int max_matrix(int ar[][4], int len)
    21	{
    22	  
    23	  int sum[len][len];
    24	  int line[len];
    25	  int max = 0;
    26	  int curren = -10000;
    27	  for ( int i = 1 ; i < len ; i++ ){
    28		for ( int j = 0 ; j < len; j++ ){
    29		  sum[i][j] = sum [i][j] + sum[i-1][j];
    30		}
    31	  }
    32	
    33	  for ( int i = 0; i < len ; i++ ){
    34		for ( int j = i ; j < len ; j++ ){
    35		  for ( int k = 0 ; k < len ; k++) {
    36			line[k] = sum[j][k]-sum[i][k];
    37		  }
    38		  curren = maxSubArray(line,len);
    39	      if ( curren > max )
    40			max = curren;
    41	
    42		}
    43	
    44	  }
    45	
    46	  
    47	}
    48	int main()
    49	{
    50	  int ar[][4]={{2,-9,1,0},{-8,-5,13,7},{-1,9,19,2},{ 11,-9,6,10}};
    51	  //  cout << " max sum  " << maxSubArray(ar, 6) << endl;
    52	  cout << " max matric " << max_matrix(ar,4) << endl;
    53	}
