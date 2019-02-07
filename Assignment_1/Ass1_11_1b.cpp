#include<cstring>
#include<iostream>
#include<string>
#include<unistd.h>
#include<sys/wait.h>

using namespace std;

void forkrun(){	
	while ( 1 ){
		string in;
		getline(cin,in);			// takes input of the argument
		if ( in == "quit" ) break;		// if input is quit then close straight away

		// copying string in to char use for use in function strtok
		char use[in.length()+1];
		strcpy(use,in.c_str());

		// *args for input to execvp 
		char *args[15];

		// breaking of path and arguments into *args
		int i=0;
		args[i]=strtok(use," ");
		while(args[i++]!=NULL){
			args[i] = strtok(NULL," ");
		}
		
		// forking for the child process
		int x;
		if ( (x=fork())==0 ){				// x is 0 when fork successfully creates a child process
			x = execvp(args[0],args);
			if ( x==-1 ) {
				cout << "execvp error" <<endl; 	// if execvp is not able to execute the process then it returns -1
				break;
			}
		} else {
			wait(NULL);
		}
	}
}

int main(){	

	forkrun();   				// Main process
	return 0;
}
