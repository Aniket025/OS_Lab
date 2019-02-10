#include<cstring>
#include<iostream>
#include<string>
#include<unistd.h>
#include<sys/wait.h>
#include<fcntl.h>
#include <stdlib.h>
#include <stdio.h>

using namespace std;

void forkrun(){	
	while ( 1 ){
		string in;
		getline(cin,in);			// takes input of the argument
		if ( in == "quit" ) break;		// if input is quit then close straight away

		// copying string in to char use for use in function strtok
		char use[in.length()+1];
		strcpy(use,in.c_str());
		char *mainCopy = strdup(use);

		// *args for input to execvp 
		char *args[15], *temp, *infile, *outfile, *token, *pipe_use[15];

		// breaking of path and arguments into *args
		int i=0,infd=-1,outfd=-1,back=-1,pipetok=0;
		temp = strtok(use," ");
		while(temp!=NULL){
			if ( strcmp(temp,"<")==0 ){
				temp = strtok(NULL," ");
				infile = temp;
				infd=0;
			} else if ( strcmp(temp,">")==0 ){
				temp = strtok(NULL," ");
				outfile = temp;
				outfd=0;				
			} else {
				if ( strcmp(temp,"&")==0 ) back=0;
				args[i++]=temp;
			}
			if ( strcmp(temp,"|")==0 ) {pipetok=1;break;}
			temp = strtok(NULL," ");
		}
		args[i] = NULL;
		
		// forking for the child process
		int x;
		if ( (x=fork())==0 ){				// x is 0 when fork successfully creates a child process		
			int tok=0,num_pipe=0,pin=0,pout=0,pos;
			char *forPipe = strdup(mainCopy);
			if ( pipetok==1 ){
				token = strtok(forPipe, "|");
				while(token!= NULL) {
				        pipe_use[tok++] = token;
				        token = strtok(NULL, "|");
				}
				num_pipe = tok;
				int* pipes = (int* )malloc(sizeof(int)*(2*(num_pipe - 1)));
			    int i;
			    for(i = 0; i < 2*num_pipe - 3; i += 2) {
			            if(pipe(pipes + i) < 0 ) {             /* Create required number of pipes, each a combination of input and output fds */
			                    perror("Pipe not opened!\n");
			                    return;
			            }
			    }
			    for(i = 0; i < num_pipe ; i++) {
	                char* cmd_tokens[1024]; /* array of command tokens */
	                memset(cmd_tokens, 0, sizeof(cmd_tokens[0]) * 1024 );
	                pin =0; pout = 0, pos=0;
	                temp = strtok(pipe_use[i]," ");
					while(temp!=NULL){
						if ( strcmp(temp,"<")==0 ){
							temp = strtok(NULL," ");
							infile = temp;
							pin=1;
						} else if ( strcmp(temp,">")==0 ){
							temp = strtok(NULL," ");
							outfile = temp;
							pout=1;				
						} else {
							cmd_tokens[pos++]=temp;
						}
						temp = strtok(NULL," ");
					}
					cmd_tokens[pos] = NULL;
	                int pid = fork();
	                if(pid < 0) {
	                        perror("Fork Error!\n");
	                }
	                else if(pid == 0) {

	                        if(pout){
	                        	pout = open ( outfile ,O_WRONLY | O_TRUNC | O_CREAT, 0666 );
								dup2(pout,1);
	                        }
	                        else if(i < num_pipe - 1) dup2(pipes[2*i + 1], 1);

	                        if(pin) {
	                        	pin = open ( infile , O_RDONLY);
								dup2(pin,0);
	                        }
	                        else if(i > 0 ) dup2(pipes[2*i -2], 0);
	                 
	                        int j;
	                        for(j = 0; j < 2*num_pipe - 2; j++) close(pipes[j]);
	         
	                        if(execvp(cmd_tokens[0], cmd_tokens) < 0 ) {
	                                //perror("Execvp error!\n");
	                                break;
	                        }
	                }
		        }
		 
		        for(i = 0; i < 2*num_pipe - 2; i++) close(pipes[i]);

			} else {
				if ( infd!=-1 ) {
					infd = open ( infile , O_RDONLY);
					dup2(infd,0);
				}
				if ( outfd!=-1) {
					outfd = open ( outfile ,O_WRONLY | O_TRUNC | O_CREAT, 0666 );
					dup2(outfd,1);
				}
				x = execvp(args[0],args);
				if ( x==-1 ) {
					cout << "execvp error" <<endl; 	// if execvp is not able to execute the process then it returns -1
					break;
				}
			}	
			
		} else {
			if ( back != 0 ) wait(NULL);
		}
	}
}

int main(){	

	forkrun();   				// Main process
	return 0;
}
