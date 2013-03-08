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

using namespace std;
enum {
	PIPE_READ = 0, PIPE_WRITE,
};

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

	//char *line = NULL;
	size_t size;

	char *map[100][100];
	int i = 0;
	int j = 0;
	string line;
	while (getline(cin, line)) {
		if (line.find("Call graph node for function") != string::npos) {

			cout << line << endl; //debug;

			//Got a line that contains string "call graph node
			j = 0;

			//get the call functions
			while (getline(cin, line) && line.length() > 1) {
				size_t pos = line.find("function");

				if (pos != string::npos) {
					//get function name and remove quotes
					line.erase(0, pos + 9);
					line.replace(0,1,"");
					line.replace(line.length() - 1, 1, "");

					cout << line << endl; //debug
					//at some point store these in the hash table
					j++;
				}
			}
		}
	}
	//print something out
	/*
	 char* func_name;
	 char* func_loc;
	 char* pair_1;
	 char* pair_2;
	 int func_support;
	 double func_confidence;
	 printf("bug %s in %s. pair(%s %s), support: %d confidence: %.2f\%\n" , func_name, func_loc, pair_1, pair_2, func_support_ func_confidence);
	 */

	/* "That's all folks." */
	return 0;

}
