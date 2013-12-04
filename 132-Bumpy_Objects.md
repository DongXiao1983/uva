	#include <iostream>
	#include <string>
	#include <vector>
	using namespace std;
	
	typedef struct point_t
	{
	  int x;
	  int y;
	}point;
	
	bool operator==(const point p1, const point p2){
	  return (p1.x == p2.x && p1.y == p2.y);
	}
	
	bool operator*(const point p1, const point p2){
	  return (p1.x * p2.x + p1.y * p2.y);
	}
	
	bool operator^(const point p1, const point p2){
	  return (p1.x * p2.y - p1.y * p2.x);
	}
	
	vector<point>  nodes;
	
	bool comare( const point p1, const point p2)
	{
	  float m1 = sqrt((float)(p1.x * p1.x + p1.y * p1.y));
	  float m2 = sqrt((float)(p2.x * p2.x + p2.y * p2.y));
	
	  float v1 = p1.x/m1 , v2 = p2.x/m2;
	  
	  return (v1 > v2 || ( v1 == v2 && m1 < m2 ) );
	}
	
	bool calc_convex_hull()
	{
	  if ( nodes.size() < 3 )
		return false;
	
	  vector<point>::iterator it;
	  point base = nodes.front();
	
	  for ( it = nodes.begin(); it != nodes.end(); it++ ){
		if (( it->y < base.y ) || ( it->y == base.y) && (it->x < base.x))
		  base = *it;
	  }
	
	  for ( it = nodes.begin(); it  != nodes.end();){
		if ( *it  == base )
		  it = nodes.erase(it);
		else{
		  it->x -= base.x;
		  it->y -= base.y;
		  it++;
		}
	  }
	
	  sort(nodes.begin(),nodes.end(),&compare);
	
	  nodes.erase(unique(nodes.begin(),nodes.end()), nodes.end());
	
	  if ( nodes.size() < 2)
		return false;
	
	  for ( vector<point>::reverse_iterator rit = sec.rbegin(); rit != src.rend()-1; rit++ ){
		vector<point>::reverse_iterator rnext = rit+1;
		rit->x -= rnext->x ; 
		rit->y -= rnext->y ;
	  }
	
	  for ( it = nodes.begin() + 1; it != nodes.end(); it++){
		for ( vector<point>::iterator ilast = it -1 ; ilast != nodes.begin();){
		  int cross = *it ^ *ilast;
	
		  if (( cross < 0 ) || ((cross == 0)&&(it->x * ilast->x > 0 && it->y * ilast->y > 0))){
			break;
		  }
		  it->x  += ilast->x it->y += ilast->y;
		  ilast = (i = nodes.erase(ilast)) -1;
		}
	  }
	
	  nodes.front().x += base.x;
	  nodes.front().y += base.y;
	
	  for ( it = nodes.begin()+1; it != nodes.end(); it++ ){
	    it->x += (it-1)->x;
		it->y += (it-1)->y;
	  }
	  nodes.push_back(base);
	  return (nodes.size() >= 3);
	}
	
	int main()
	{
	  int mass_x, mass_y;
	  int start, end;
	  cin >> mass_x >> mass_y;
	  while ( cin >> start >> end) { 
		if ( start == 0 && end == 0 )
		  break;
		point p ;
	    p.x = start;
		p.y = end;
		nodes.push_back(p);
	  };
	
	  for ( int i = 1 ; i < nodes.size(); i++ ){
		double k = ( nodes[i].y - nodes[0].y ) / ( nodes[i].x - nodes[0].x);
		cout << k << " ";
	  }
	
	  calc_convex_hull();
	
	  for ( int i = 0 ; i < nodes.size(); i++ ){
		int j = ( i + 1 ) % nodes.size();
		int k = ( j + 1 ) % nodes.size();
	
	
		point p1 = {nodes[j].x - nodes[i].x, nodes[j].y - nodes[i].y};
	
		point p2 = {nodes[k].x - nodes[j].x, nodes[k].y - nodes[j].y};
	
		if ( ( p1 ^ p2 ) == 0 ) {
		  nodes.erase(nodes.begin() + j );
		  i -- ;
		}
	  }
	  
	  int Nmin = nodes.size();
	
	  for ( int i = 0 ; i < nodes.size(); i++ ){
		int j = ( i + 1 ) % nodes.size();
		point v1 = {mass_x - nodes[i].x, mass_y - nodes[i].y};
		point v2 = {mass_y - nodes[j].x, mass_y - nodes[j].y};
	
		point s1 = {nodes[i].x - nodes[j].x, nodes[i].y - nodes[j].y };
		point s2 = {nodes[j].x - nodes[i].x, nodes[j].y - nodes[i].x };
	
		if ( ( s1 ^ v1 ) >= 0  && v1 * s1 >= 0 && v2 * s2 >= 0 ){
		  for ( int k = 0 ; k < nodes.size(); k++ ){
			s2.x = nodes[k].x - nodes[i].x;
			s2.y = nodex[k].y - nodes[i].y;
	
			if (( s2 ^ s1 ) == 0 )
			  nMax =  max(k, nMax);
		  }
		  Nmin = min(Nmin, nMax);
		}
		cout << name << " " << Nmin + 1 << endl;
	  }
	  return 0;
	}
