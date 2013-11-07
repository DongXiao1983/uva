#109 SCUD 
This is a cyfra problem.   
Step 1: cacluate each kindom's areas  
Step 2: Judge the bomb is in the kindom  
Step 3: Then add the kindom's areas  
Step 4: The bomb just only one change to attach a kindom.  

## Sample Code

    /*
       UVA 109 SCUD Busters
       AC by J_Dark
       ON 2013/5/7 2:20......1A...............
       Time 0.015s
    */
    #include <iostream>
    #include <cstdio>
    #include <algorithm>
    #include <vector>
    using namespace std;
    const int maxn = 110;
    /////////////////////////////////////////
    struct point{
    	double x, y;
    	point(double a, double b){
    		x = a;
    		y = b;
    	}
    };
    struct Kingdom{
    	double area;
    	bool use;
    	int top;
    	vector<int> CH;
    	Kingdom(){
    	   area = 0;
    	   top = 1;
    	   CH.clear();
    	   CH.resize(maxn);
    	   CH[0] = 0;
    	   CH[1] = 1;
    	   use = false;  //......
    	}
    };
    vector<point> P, M; //....  ..
    vector< vector<point> > kdNode; //........
    vector<Kingdom> KD;  //..
    int nodeNum;
    double ansArea=0;  //........
    /////////////////////////////////////////
    void Input(){
    	P.clear();
    	//M.clear();
    	//CH.clear();
    	double xx, yy;
    	if(nodeNum != -1){
       for(int i=0; i<nodeNum; i++){
    		  cin >> xx >> yy;
    	  P.push_back(point(xx, yy));
    	   }
    	   kdNode.push_back(P);
    	}
    	else{
    	   while(cin >> xx >> yy)
    	  M.push_back(point(xx, yy));
    	}
    }
    
    //...
    bool cmp(point a, point b){
    	if(a.y == b.y)  return a.x < b.x;
    	return a.y < b.y;
    }
    //....p2-pp.....p1-p2..
    bool turnRight(point p1, point p2, point pp){
    const double eps = 1e-20;
    	if((p2.x - p1.x)*(pp.y - p2.y) - (pp.x - p2.x)*(p2.y - p1.y) <= eps) return true;
    	return false;
    }
    //....
    double multi(point p0, point p1, point p2){
    	return (p1.x - p0.x)*(p2.y - p0.y) - (p2.x - p0.x)*(p1.y - p0.y);
    }
    
    void Compute(){
    	//.........
    	//cout << kdNode.size() << endl << endl;
    for(int k=0; k<kdNode.size(); k++){
    		sort(kdNode[k].begin(), kdNode[k].end(), cmp);
    		KD.push_back(Kingdom());
    		//...0............  ..1
    		for(int i=2; i<kdNode[k].size(); i++){
    			while( KD[k].top && turnRight(kdNode[k][KD[k].CH[KD[k].top-1]], kdNode[k][KD[k].CH[KD[k].top]], kdNode[k][i]) )
    			{
    			   KD[k].top--;
    			}
    			KD[k].CH[++KD[k].top] = i;
    		}
    
    		int len = KD[k].top;
    		//..........0fab.......  ..2
    		KD[k].CH[++KD[k].top] = kdNode[k].size()-2;
    		for(int i=kdNode[k].size()-3; i>=0; i--){
    			//KD[k].top!=len, .......1.......
    			while( KD[k].top!=len && turnRight(kdNode[k][KD[k].CH[KD[k].top-1]], kdNode[k][KD[k].CH[KD[k].top]], kdNode[k][i]) )
    			{
    			   KD[k].top--;
    			}
    			KD[k].CH[++KD[k].top] = i;
    		}
    
    		//..........
    for(int i=1; i<KD[k].top-1; i++){
    			KD[k].area += multi(kdNode[k][KD[k].CH[0]], kdNode[k][KD[k].CH[i]], kdNode[k][KD[k].CH[i+1]]);
    		}
    		//printf("KD[%d].area = %lf\n", k, KD[k].area);
    
    		//............
    		for(int m=0; m<M.size(); m++){
    		   if(KD[k].use)  break;  //...........
    		   for(int i=0; i<KD[k].top-1; i++){
    			   if(!turnRight(kdNode[k][KD[k].CH[i]], kdNode[k][KD[k].CH[(i+1)%KD[k].top]], M[m]))//..M[m]......
    				  KD[k].use = true; //.........
    			   else{
    			  KD[k].use = false; //.M[m].............
    				  break;
    			   }
    		   }
    		   if(KD[k].use){  //...........
    			   ansArea += KD[k].area;
    			   break;
    		   }
    		}
    	}
    	printf("%.2lf\n", ansArea/2);  //.......................2........2
    }
    
    
    /////////////////////////////////////////
    int main(){
    	while(cin >> nodeNum)
    	{
    	   Input();
    	   if(nodeNum == -1)  Compute();
    	}
    	return 0;
    }
