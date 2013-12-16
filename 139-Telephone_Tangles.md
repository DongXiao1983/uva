	#include <iostream>
	#include <string>
	#include <vector>
	#include <cstdio>
	#include <cstdlib>
	#include <iomanip>
	
	using namespace std;
	typedef struct record_t{
	  string number;
	  string country;
	  int     price;
	}record;
	
	typedef struct tel_t{
	  string number;
	  int    duration;
	}tele;
	
	void print_out(string tel, string contry, string num, int dur, float price, float total)
	{
	  cout << left;
	  cout << setw(17) << tel;
	  cout << setw(12) << contry;
	  cout << right;
	  cout << setw(8) << num;
	  cout << setw(4) << dur;
	  cout << setw(6) << price;
	  cout << setw(7) << total;
	  cout << endl;
	}
	
	
	int main()
	{
	  string input;
	  vector<record>  rec;
	  vector<tele>    tel;
	  while(1){
	    getline(cin,input);
	    if ( input == "000000") break;
	    record r;
	    r.number =  input.substr(0,    input.find_first_of(" "));
	    r.country = input.substr(input.find_first_of(" ")+1,input.find_first_of("$")-input.find_first_of(" ")-1);
	    r.price   = 0;
	    string t  = input.substr(input.find_first_of("$")+1,input.length()-input.find_first_of("$")-1);
	   
	    for ( int i = 0 ; i < t.length(); r.price = 10 *r.price + (t[i++]-'0'));
	    
	    rec.push_back(r);
	  }    
	  
	  string aa ;
	  int    bb ;
	  while(cin >> aa  ){
	    if ( aa == "#" ) break;
	    cin >> bb;
	    tele t;
	    t.number = aa;
	    t.duration = bb;
	    tel.push_back(t);
	  }
	
	  vector<record>::iterator ir;
	  vector<tele>::iterator   it;
	  bool tag = false;
	  for ( it = tel.begin();it!=tel.end();it++, tag=false){
	    for( ir = rec.begin(),tag=true; ir != rec.end(); ir++ ) {
	      if ( it->number[0] != '0' ){
		print_out(it->number,"Local",it->number,it->duration, 0.00,0.00);
		tag = false;
		break;
	      }
	      else {
		string s = it->number.substr(0,ir->number.length());
		if (s == ir->number){
		  string x = it->number.substr(ir->number.length(),it->number.length()-ir->number.length());
		  print_out(it->number, ir->country ,x,it->duration,ir->price/100.00, it->duration * ir->price/100.00);
	
		  tag = false;
		  break;
		}
	      }
	     }
	    if (tag){
	      print_out(it->number,"Unkown" ,"",it->duration,0, -1.00);
	      
	    }
	     
	  }
	
	}
