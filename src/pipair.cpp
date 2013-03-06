//============================================================================
// Name        : pipair.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <string>
#include <stdlib.h>
#include <sstream>
using namespace std;

int main(int argc, char *argv[]) {
	int support = 3;
	double confidence = .65;
	string filename = "";

	std::stringstream s;  //data conversion


	switch(argc){
	case 4:
		s << argv[3];
		s >> confidence;

		s.str( std::string() ); //clear the sstream
		s.clear();

		//FOLLOW THROUGH
	case 3:
		s << argv[2];
		s >> support;
		//FOLLOW THROUGH
	case 2:
		filename = argv[1];
		break;
	default:
		cerr << "Input must be " << argv[0] << " <bitcode_file> <T_SUPPORT> <T_CONFIDENCE>" <<endl;
		return 0;
	}

	//DO stuff
	return 0;
}
