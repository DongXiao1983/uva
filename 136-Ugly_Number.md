	#include <iostream>
	#include <cstring>  
	
	using namespace std;
	
	int minn(int a , int b , int c)
	{
	  int ret;
	  ret = min (a,b);
	  ret = min (ret,c);
	  return ret;
	}
	
	int main()
	{
	  int count ;
	  int step = 1;
	  int ulgy[3000] = {0};
	  while(  cin >> count ){
	    memset(ulgy,0,sizeof(ulgy));
	    ulgy[0] = 1;
	    step = 1;
	    int try2 = 0;
	    int try3 = 0;
	    int try5 = 0;
	  while (step < count ){
	    int num =  minn ( ulgy[try2]*2, ulgy[try3]*3, ulgy[try5]*5);
	    step ++;
	    ulgy[step] = num;
	    for ( ;  ulgy[try2] * 2 <= ulgy[step] ; try2++);
	    for ( ;  ulgy[try3] * 3 <= ulgy[step] ; try3++);
	    for ( ;  ulgy[try5] * 5 <= ulgy[step] ; try5++);     
	  }
	  cout << "The "<<count<<"'th ulgy numbner is <"<<ulgy[step] <<">"<< endl;
	  }
	}
