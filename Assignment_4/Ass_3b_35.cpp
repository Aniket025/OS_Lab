#include <bits/stdc++.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>

using namespace std;

#define MAXVALUE 1000

typedef struct
{
	int stat;
	pthread_t thread;
	int id;
}thread_details;

int current_thread;
thread_details status[MAXVALUE];
int no;

int compare(const void *a, const void *b){
	return (*(int*)a-*(int*)b);
}

void sig_handler(int signo){
	if(signo==SIGUSR1){
		pause();
	}
	else{
		//Do nothing
	}
}

void* worker(void *pv){
	int id  = *(int *)pv;
	status[id].stat = 1;
	int *arr= (int *) malloc(1000*sizeof(int));
	for(int i=0; i<1000;i++){
		arr[i]=rand()%10000+1;
		//cout << arr[i] << " ";
	}
	qsort(arr, 1000, sizeof(int), compare);
	cout << "Sorted numbers from thread " << id << " is:\n" ;
	for(int i=0; i<1000; i++){
			cout << arr[i] << " ";
	}
	cout << "\n\n";
	int waittime = rand()%10+1;
	while(waittime){
		waittime = sleep(waittime);
	}
	status[id].stat = 2;
}

void* scheduler(void* param){
	queue<int> processing;
	for(int i=0;i<no;i++)
		processing.push(i);
	int temp;
	current_thread = processing.front();
	cout << "Scheduler Started" << endl;
	pthread_kill(status[current_thread].thread, SIGUSR2);
	sleep(1);  // The time quantum
	while(!processing.empty() && processing.size()!=1){
		if(processing.size()==1)
			break;
		else{
			if(status[current_thread].stat==2){
				// Do nothing
			}
			else{
				pthread_kill(status[current_thread].thread, SIGUSR1);
				processing.push(current_thread);
			}
			temp = current_thread;
			processing.pop();
			if(processing.empty())
				pthread_exit(NULL);
			else{
				current_thread = processing.front();
				pthread_kill(status[current_thread].thread, SIGUSR2);
				sleep(1);
			}
		}
	}
	if(!processing.empty()){
		temp = processing.front();
		current_thread = temp;
		pthread_kill(status[current_thread].thread, SIGUSR2);
		while(status[current_thread].stat!=2);
	}
	else
		pthread_exit(NULL);
}

void* reporter(void* param){
	int temp = current_thread;
	int prints[no];
	for(int i=0; i<no;++i){
		prints[i]=0;
	}
	int count =0;
	while(1){
		if(temp!=current_thread){
			cout << "Thread number " << temp << " has been suspended and number " << current_thread << " started" << endl;
			temp = current_thread;
		}
		for(int i=0; i<no;i++){
			if(prints[i]==0 && status[i].stat==2){
				cout << "Thread number " << i << " has been terminated\n";
				++count;
				prints[i]=1;
			}
		}
		if(count==no){
			cout << "Completed all threads\n";
			pthread_exit(NULL);
		}
	}
}

/*void sig_usr2(int signo){
	if(signo==SIGUSR2){
		cout << "Thread "<< pthread_self() << " woken up"<< endl;
	}
	signal(SIGUSR2, sig_usr2);
}*/

int main(){
	cout << "No of threads to be created" << endl;
	cin >> no;
	signal(SIGUSR2, sig_handler);
	signal(SIGUSR1, sig_handler);
	pthread_t schedule, report;
	for(int i=0; i<no; i++){
		status[i].id = i;
		pthread_create(&status[i].thread, NULL, worker, &status[i].id);
		pthread_kill(status[i].thread, SIGUSR1);
	}
	pthread_create(&report, NULL, reporter, NULL);
	pthread_create(&schedule, NULL, scheduler, NULL);

	for(int i=0; i<no; i++)
		pthread_join(status[i].thread, NULL);

	pthread_join(schedule, NULL);
	pthread_join(report, NULL);

	return(0);
}
