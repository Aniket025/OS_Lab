#include<iostream>
#include<stdlib.h>
#include<time.h>
#include<bits/stdc++.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>

using namespace std;

// function to generate 50 random numbers 
void generate( int arr[50] ){
	srand(getpid());	// the seed to random generation is process ID which is different for all processes and hence different random numbers
	for ( int i=0 ; i<50 ; i++ ) arr[i] = rand()%500+1;	// cap of 500
}

int main(){
	int pipe1[2],pipe2[2],pipe3[2],pipe4[2];
	int a[50],b[50],c[50],d[100],e[150];

	pid_t a1,b1,c1,d1;	//signed integer type to represent process id
	
	// if any pipe fails to build then mention it
	if ( pipe(pipe1) == -1 )cout << "error at pipe1" << endl;
	if ( pipe(pipe2) == -1 )cout << "error at pipe2" << endl;
	if ( pipe(pipe3) == -1 )cout << "error at pipe3" << endl;
	if ( pipe(pipe4) == -1 )cout << "error at pipe4" << endl;

	// if fork for process A fails then print error
	if ( (a1=fork()) == -1 )cout << "error at fork1" << endl;  
	if ( a1 == 0 ){
		///////////			Process A 			/////////////
		cout << "Running Process A" << endl;
		generate(a);
		sort(a,a+50);
		close(pipe1[0]);					// close the read end
		write(pipe1[1],a,sizeof(int)*50);	// write to the write end
		close(pipe1[1]);					// close the write end
	} else {
		wait (NULL);
		if ( (b1=fork()) == -1 )cout << "error at fork2" << endl;
		if (b1==0){
			///////////			Process B 			/////////////
			cout << "Running Process B" << endl;
			generate(b);
			sort(b,b+50);
			close(pipe2[0]);
			write(pipe2[1],b,sizeof(int)*50);
			close(pipe2[1]);	
		} else {
			wait (NULL);
			if ( (c1=fork())==-1 ) cout << "Error at fork3" << endl;
			if ( c1==0 ){
				///////////			Process C			/////////////
				cout << "Running Process C" << endl;
				generate(c);
				sort(c,c+50);
				close(pipe3[0]);
				write(pipe3[1],c,sizeof(int)*50);
				close(pipe3[1]);	
			} else {
				wait(NULL);
				if ( (d1=fork())==-1 ) cout << "Error at fork4" << endl;
				if ( d1==0 ){
					///////////			Process D 			/////////////
					cout << endl;
					cout << "Running Process D" << endl;
					close(pipe1[1]);						// close the write end of pipe from A
					read(pipe1[0], d, sizeof(int)*50);		// read array sent from A
					close(pipe1[0]);						// close the read end
					cout << "Array revieved from process A"<< endl;
					close(pipe2[1]);						// close the write end of pipe from B
					read(pipe2[0], d+50, sizeof(int)*50);	// read array sent from B
					close(pipe2[0]);						// close the read end
					cout << "Array revieved from process B"<< endl;
					sort(d,d+100);
					close(pipe4[0]);
					write(pipe4[1],d,sizeof(int)*100);
					close(pipe4[1]);
				} else {
					wait(NULL);
					///////////			Process E 			/////////////
					cout << endl;
					cout << "Running Process E" << endl;
					close(pipe4[1]);
					read(pipe4[0], e, sizeof(int)*100);
					close(pipe4[0]);
					cout << "array revieved from process D"<< endl;
					close(pipe3[1]);
					read(pipe3[0], e+100, sizeof(int)*50);
					close(pipe3[0]);
					cout << "array revieved from process C"<< endl;
					sort(e,e+150);
					int i=0;
					while(i<150) cout << e[i++] << " ";
					cout << endl;
				}
			}
		}
	}		
}
