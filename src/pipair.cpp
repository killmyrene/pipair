//============================================================================
// Name        : pipair.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <locale>
#include <functional>

#include <vector>
#include <map>

using namespace std;
enum {
	PIPE_READ = 0, PIPE_WRITE,
};
struct FuncUse;
struct CGN;

typedef map<string, int> msi;
typedef map<int, int> mii;
typedef vector<string> vs;
typedef vector<long> vi;
typedef map<long, FuncUse> mif;
typedef vector<CGN> vg;

bool containsElem(vs list, string value) {
	for (vs::iterator it = list.begin(); it != list.end(); it++) {
		if (*it == value) {
			return true;
		}
	}
	return false;
}

bool containsElem(vi list, long value) {
	for (vi::iterator it = list.begin(); it != list.end(); it++) {

		if (*it == value) {

			return true;
		}
	}
	return false;
}

//Call graph node
struct CGN {
	string call_fun_name; //name of the call graph function
	vs function_uses; //list of all function uses in the call graph
	vi pairs; //the hashed pairs

	CGN(string name = "") :
		call_fun_name(name) {
	}

	bool containsFunctionName(string fun_name) {
		return containsElem(function_uses, fun_name);
	}

	bool containPairs(long hash) {
		return containsElem(pairs, hash);
	}
	void addFunctionUse(string fun_name) {
		if (!containsElem(function_uses, fun_name)){
			function_uses.push_back(fun_name);
		}
	}
	void addPairs(long hash) {
		if(!containsElem(pairs, hash)){
			pairs.push_back(hash);
		}
	}

	vi getPairs() {
		return pairs;
	}

	//debugging purpose
	void toString(){
		cerr << "Call name: " << call_fun_name << " | Functions: ";
		for(vs::iterator i = function_uses.begin(); i != function_uses.end(); i++){
			cerr << *i << " ";
		}
		cerr << "| Pairs Size " << pairs.size() << endl;
	}


};

//Function use struct
struct FuncUse {
	string function_name;
	int support_num;
	vi pairs; //links to hashed pairs that uses the function name

	FuncUse(string name = "") :
		function_name(name), support_num(0) {
	}

	void addCount() {
		support_num++;
	}

	void addPair(long hash) {
		if (!containsElem(pairs, hash)) {
			pairs.push_back(hash);
		}
	}
	bool reachesSupportMinimun(int t_support) {
		return support_num >= t_support;
	}

	vi getPairs() {
		return pairs;
	}

};


//Generate hash code from a string for faster access in map
long  generateHash(string str) {

	long hash= 0;
	for(int i = 0; i < str.size(); i++){// string::const_iterator it=str.begin(); it!=str.end(); ++it) {
		hash += str[i] * (i + 5);
	}

	//random stuff to add

	hash *= str[0];


	return hash;
}
//Generate hash code from 2 strings by multiplying two hash from two strings together
//TODO: can potentially crash if the value overflows
long generateHashPair(string s1, string s2){
	long h1 = generateHash(s1);
	long h2 = generateHash(s2);
	return h1  * h2;
}

//TODO: code based from the T2 demo
int main(int argc, char *argv[]) {

	/* pipe to connect opt's stderr and our stdin */
	int pipe_callgraph[2];

	/* pid of opt */
	int opt_pid;

	int support = 3;
	double confidence = 65;
	char* filename = "";

	//std::stringstream s; //data conversion

	switch (argc) {
	case 4:
		confidence = atoi(argv[3]);
		//FOLLOW THROUGH
	case 3:
		support = atoi(argv[2]);
		//FOLLOW THROUGH
	case 2:
		filename = argv[1];
		break;
	default:
		printf("Input must be %s <bitcode_file> <T_SUPPORT> <T_CONFIDENCE>\n",
				argv[0]);
		return 0;
	}

	/*TODO: code below is based on the demo

	 /* create pipe and check if pipe succeeded */
	if (pipe(pipe_callgraph) < 0) {
		perror("pipe");
		return 1;
	}

	/* create child process */
	opt_pid = fork();
	if (!opt_pid) { /* child process, to spawn opt */

		/* close the read end, since opt only write */
		close(pipe_callgraph[PIPE_READ]);

		/* bind pipe to stderr, and check */
		if (dup2(pipe_callgraph[PIPE_WRITE], STDERR_FILENO) < 0) {
			perror("dup2 pipe_callgraph");
			return 1;
		}

		//close stdout
		fclose(stdout);

		/* print something to stderr */
		fprintf(stderr, "This is child, just before spawning opt with %s.\n",
				filename);

		/* spawn opt */
		if (execl("/usr/local/bin/opt", "opt", "-print-callgraph", filename,
				(char *) NULL) < 0) {
			perror("execl opt");
			return 1;
		}

		/* unreachable code */
		return 0;
	}

	/* parent process */

	/* close the write end, since we only read */
	close(pipe_callgraph[PIPE_WRITE]);

	/* since we don't need stdin, we simply replace stdin with the pipe */
	if (dup2(pipe_callgraph[PIPE_READ], STDIN_FILENO) < 0) {
		perror("dup2 pipe_callgraph");
		return 1;
	}

	string debug = "apr_array_make";

	/* we print w/e read from the pipe */

	//return 0;
	size_t size;

	mif support_num;
	mif pair_support_num;
	vs stash;
	vg calls;
	CGN call;

	long str_hash;
	long pair_hash;
	string line;
	bool foundNode = false;




	while (getline(cin, line)) {

		if (line.length() <= 1) {
			foundNode = false; //find next node if line only contains a space

			if ( call.function_uses.size() != 0){
				calls.push_back(call);
			}



			//TODO:: update stuff
		} else if (foundNode) {
			//get function name
			size_t pos = line.find("function");

			if (pos != string::npos) {
				//get function name and remove quotes
				line.erase(0, pos + 9);
				line.replace(0, 1, "");
				line.replace(line.length() - 1, 1, "");

				//check if it doesnt contain the stuff in stash
				if (!containsElem(stash, line)) {
					//update the support of the function use

					str_hash = generateHash(line); //create hash
					//update the support number
					if (support_num.find(str_hash) == support_num.end()) {
						//if not found, make new FuncUse
						FuncUse use(line);
						support_num[str_hash] = use;
					}
					support_num[str_hash].addCount();

				/*	if (str_hash == generateHash(debug)){
						cerr << "Name " << line << " " << support_num[str_hash].support_num << " " << str_hash << endl;
					}*/


					//make pairs
					for (vs::iterator jt = stash.begin(); jt != stash.end();
							jt++) {

						string pr;

						string first, second;



						//Sort two words
						if (strcmp(jt->c_str(), line.c_str()) < 0){
							//first word is less than the 2nd word
							pr = *jt + " " + line;

							first = *jt;
							second = line;
						}else{
							pr = line + " " + *jt;

							first = line;
							second = *jt;
						}

						//update the support number
						pair_hash =generateHashPair(first, second);
						//update the support number
						if (pair_support_num.find(pair_hash)
								== pair_support_num.end()) {
							//if not found, make new FuncUse
							FuncUse use(pr);
							pair_support_num[pair_hash] = use;
						}
						pair_support_num[pair_hash].addCount();

						long last_hash = generateHash(*jt);


						//store the pair hash
						support_num[last_hash].addPair(pair_hash);
						support_num[str_hash].addPair(pair_hash);

						call.addPairs(pair_hash);


						/*if (pair_hash == 8290602319970 || pair_hash == 7393051972856){
							cerr << "Pair of "<< pr << " " << pair_support_num[pair_hash].support_num <<  " " << pair_hash << endl;
						}*/


					}

					//add the function name on the stash
					stash.push_back(line);
					call.addFunctionUse(line);





				}

			}

			//find node to extract function names
		} else if (line.find("Call graph node for function") != string::npos) {


			foundNode = true;
			//get the call function name
			size_t pos = line.find("'");

			//Assume that it exist
			line.erase(0, pos + 1);
			pos = line.find("'");
			line.erase(pos, line.length() - 1);

			call = CGN(line);

			stash.clear();

		}

		line.clear();

	}


	//print something out
	for (mif::iterator it = support_num.begin(); it != support_num.end();
			it++) {
		FuncUse single = it->second;

		if (!single.reachesSupportMinimun(support)){
			continue;
		}
		//get all pairs associated with the function use
		vi p = it->second.getPairs();
		for (int i = 0; i < p.size(); i++){
			FuncUse pair = pair_support_num[p[i]];


			if (!pair.reachesSupportMinimun(support)){
				continue;
			}

			//calculate the confidence rate
			double conf_rate = pair.support_num * 1.0 / single.support_num;
			conf_rate *= 100;


		/*	if (single.function_name == debug){
				cerr <<pair.function_name << " " << pair.support_num << " at " << i << " " << p[i] << " conf_rate: " << conf_rate << endl;
			}*/


			if (conf_rate >= confidence && conf_rate < 100){
				//NOTE: there is never a conf_rating that is higher than 100%



				//search for the functions that doesnt contain the pair
				for (vg::iterator jt = calls.begin();  jt!= calls.end(); jt++){

					bool isContain = jt->containsFunctionName(single.function_name) &&
							!jt->containPairs(p[i]);


					if (single.function_name == debug && jt->containsFunctionName(single.function_name)){
						//jt->toString();
						//cerr <<"Bi " << isContain << endl;
					}

					if (isContain){

						string call_name = jt->call_fun_name;
						printf("bug: %s in %s pair: (%s) support: %d confidence: %.2f\%\n", single.function_name.c_str(), call_name.c_str(), pair.function_name.c_str(), pair.support_num, conf_rate);

						//printf("bug: %s in %s, pair: (%s), support: %d, confidence: %.2f\%\n", single.function_name.c_str(), call_name.c_str(), pair.function_name.c_str(), pair.support_num, conf_rate);
					}

				}


			}

		}

	}

	/*long hash = generateHash(debug);

	cerr << support_num[hash].function_name << " " <<support_num[hash].support_num << " hash " << hash << endl;


	long hash2 = generateHashPair(debug, "apr_array_push");
	cerr <<  pair_support_num[hash2].function_name << " " <<pair_support_num[hash2].support_num << " hash " << hash2 << endl;
*/


	/*

	//print out the single and double support
	for(mif::iterator it = support_num.begin(); it != support_num.end(); it++){
		FuncUse single= it->second;
		if (single.reachesSupportMinimun(support)){
			cerr << "Support for (" << single.function_name << ")-" << single.support_num << " hash " << it->first <<  endl;
		}
	}

	for(mif::iterator it = pair_support_num.begin(); it != pair_support_num.end(); it++){
		FuncUse doub= it->second;
		if (doub.reachesSupportMinimun(support)){
			cerr << "Support for (" << doub.function_name << ")-" << doub.support_num << " hash " << it->first <<  endl;
		}
	}
*/



	/* "That's all folks." */
	return 0;

}

