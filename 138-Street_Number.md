	#include <iostream>
	#include <iomanip>
	using namespace std;
	int main()
	{
	  int x , y;
	  for ( int i = 0,x0 = 3, y0 =2, x = x0 ,y = y0,t ;i < 10 ; i++,x = t){
	    t = x * x0 + 2 * y * y0;
	    y = x * y0 + y*x0;
	    cout << setw(10)<<y/2 << setw(10)<<(t-1)/2 << endl;
	  } 
	}
