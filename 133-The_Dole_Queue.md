	#include <iostream>
	#include <iomanip>
	#include <cstring>
	
	using namespace std;
	#define N 20
	int queue[N];
	
	int main()
	{
	  int n, k ,m;
	 
	  while ( cin >> n >> k >> m){
		if ( n == 0 && k == 0 && m == 0 )
		  break;
		memset(queue,0,N);
		for ( int i = 1 ; i <= n ;  queue[i]=i, i++);
		
	    int step = 0;
	    int start_k = 0;
		int start_m = n+1;
	    while ( step < n){
		  int count_k = 0 , count_m = 0;
		  while(1){
			if ( queue[start_k] != 0 ) 
			  count_k++;
			if ( start_k > n)
			  start_k = 0;
			if ( count_k == k ){
			  cout << setw(3) << queue[start_k] ;
			  step++;
			  break;
			}
			start_k++;
		  }
	
		  while(1){
			if ( queue[start_m] != 0 ) 
			  count_m++;
			if ( start_m < 1)
			  start_m = n+1;
			if ( count_m == m ){
			  if ( queue[start_k] != queue[start_m] ){
				cout << setw(3) << queue[start_m];
			  step++;
			  }
			  break;
			  
			}
			start_m--;
		  }
		  queue[start_k] = 0;
		  queue[start_m] = 0;
		  if ( step != n )
			cout << ",";
		}
	
	  }
	}
