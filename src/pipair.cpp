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

#include <vector>
#include <map>

using namespace std;
enum {
	PIPE_READ = 0, PIPE_WRITE,
};
struct FuncUse;

typedef map<string, int> msi;
typedef map<int, int> mii;
typedef vector<string> vs;
typedef vector<int> vi;
typedef map<int, FuncUse> mif;

bool containsElem(vs list, string value){
	for (vector<string>::iterator it = list.begin(); it != list.end(); it++){
		if (*it == value){
			return true;
		}
	}
	return false;
}

//Call graph node
struct CGN{
	string call_fun_name; //name of the call graph function
	vs function_uses; //list of all function uses in the call graph
	vs pairs; //the pairs made while adding the function use

	CGN(string name = ""): call_fun_name(name){}

	bool containsFunctionName(string fun_name){
		return containsElem(function_uses, fun_name);
	}

	bool containPairs(string pair){
		return containsElem(pairs, pair);
	}
	void addFunctionUse(string fun_name){
		function_uses.push_back(fun_name);
	}
	void addPairs(string pair){
		pairs.push_back(pair);
	}

	vs getPairs(){
		return pairs;
	}


};

//Function use struct
struct FuncUse{
	string function_name;
	int support_num;
	vs pairs; //links to pairs that uses the function name



	FuncUse(string name = "") : function_name(name), support_num(0){

	}

	void addCount(){
		support_num++;
	}

	void addPair(string other){
		other = function_name + " " + other;
		if (!containsElem(pairs, other)){
			pairs.push_back(other);
		}
	}

};


//Generate hash code from a string for faster access in map
int generateHash(string str) {
	long hash= 0;
	for(string::const_iterator it=str.begin(); it!=str.end(); ++it) {
		hash += *it;
	}
	return hash;
}

bool containsElem(msi m, string value){
	map<string, int>::iterator it = m.find(value);

	if (it != m.end()){
		//if found return true
		return true;
	}
	return false;
}



//TODO: code based from the T2 demo
int main(int argc, char *argv[]) {

	/* pipe to connect opt's stderr and our stdin */
	int pipe_callgraph[2];

	/* pid of opt */
	int opt_pid;

	int support = 3;
	double confidence = .65;
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

	/* we print w/e read from the pipe */



	size_t size;

	mif support_num;
	mif pair_support_num;
	vs stash;

	int str_hash;

	string line;
	bool foundNode = false;
	while (getline(cin, line)) {

		if (line.length() <= 1){
			foundNode = false;//find next node if line only contains a space

			//TODO:: update stuff
		}else if (foundNode){
			//get function name
			size_t pos = line.find("function");

			if (pos != string::npos) {
				//get function name and remove quotes
				line.erase(0, pos + 9);
				line.replace(0,1,"");
				line.replace(line.length() - 1, 1, "");

				//check if it doesnt contain the stuff in stash
				if (!containsElem(stash, line)){
					//update the support of the function use
					cout << line; //debug

					str_hash = generateHash(line); //create hash
					//update the support number
					if(support_num.find(str_hash) == support_num.end()){
						//if not found, make new FuncUse
						FuncUse use(line);
						support_num[str_hash] = use;
					}

					support_num[str_hash].addCount();

					//make pairs
					for (vs::iterator jt = stash.begin(); jt != stash.end(); jt++){
						string pr = *jt + " " + line;
						cout << " (" << pr << ")";

						//update the support number
						str_hash = generateHash(pr);
						//update the support number
						if(pair_support_num.find(str_hash) == pair_support_num.end()){
							//if not found, make new FuncUse
							FuncUse use(pr);
							pair_support_num[str_hash] = use;
						}
						pair_support_num[str_hash].addCount();


					}
					cout << endl;

					//add the function name on the stash
					stash.push_back(line);

				}

			}

		}else if (line.find("Call graph node for function") != string::npos) { //find node to extract function names
			foundNode = true;
			//get the call function name
			size_t pos = line.find("'");
			//Assume that it exist
			line.erase(0, pos + 1);
			pos = line.find("'");
			line.erase(pos, line.length() - 1);

			cout << "Call graph " <<  line << endl; //debug;


			stash.clear();

		}
	}

	//print something out

	for (mif::iterator it = support_num.begin(); it != support_num.end(); it++){
		if (it->second.support_num >= support){
			cerr << "Support of " << it->second.function_name << " - " << it->second.support_num << endl;
		}
	}


	for (mif::iterator it = pair_support_num.begin(); it != pair_support_num.end(); it++){
		if (it->second.support_num >= support){
			cerr << "Support of " << it->second.function_name << " - " << it->second.support_num << endl;
		}
	}

	/* "That's all folks." */
	return 0;

}

