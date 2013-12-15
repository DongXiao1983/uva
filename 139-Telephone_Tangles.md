	#include <iostream>
	#include <string>
	#include <vector>
	#include <cstdio>
	#include <cstdlib>
	
	using namespace std;
	struct record_t{
	  string number;
	  string country;
	  string price;
	};
	
	struct tel_t{
	  string number;
	  int    duration;
	};
	int main()
	{
	  string input;
	  vector<struct record_t> record;
	  vector<struct tel_t>    tel;
	  while(1){
	    getline(cin,input);
	    if ( input == "000000") break;
	    struct record_t r ;
	    r.number =  input.substr(0,    input.find_first_of(" "));
	    r.country = input.substr(input.find_first_of(" "),input.find_first_of("$"));
	    r.price   = input.substr(input.find_first_of("$"),input.length());
	    record.push_back(r);
	  }
	  while(1){
	    getline(cin,input);
	    if ( input == "#") break;
	    struct tel_t t;
	    t.number = input.substr(0, input.find_first_of(" "));
	    t.duration = atoi(input.substr(input.find_first_of(" ")+1, input.length()));
	  }
	}
