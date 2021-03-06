#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <libgen.h>
#include <stddef.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <pthread.h>
using namespace std;

#define SHSIZE 100
#define BUFSIZE 100
#define CLINET_COUNT 10


int connect_to_pipe() {
        char *registration_pipe = "/tmp/registration";
        mkfifo(registration_pipe, 0666);
        int fd = open(registration_pipe, O_RDWR);
        if (fd == -1) {
                cout<<"Cannot open registration pipe. error "<< strerror(errno);
                exit(1);
        }
        return fd;
}

void registration() {
        int pid = getpid();
        int reg_fd = connect_to_pipe();

        char pid_str[BUFSIZE];
        sprintf(pid_str, "%d", pid);
        write(reg_fd, pid_str, strlen(pid_str));
        cout<<"Client registered with ID: "<< pid << endl;
}

void client_list(){
	key_t key = 9874;
	int shmid = shmget(key,SHSIZE,IPC_CREAT | 0666);
	if(shmid < 0) { perror("shmget"); exit(1); }
	
	char* shm = (char*)shmat(shmid,NULL,0);
	if(shm == (char*)-1) { perror("shmat"); exit(1); }
	cout << shm << endl;
}

void msg(char* text, char* id){
	key_t key = 9874;
	int shmid = shmget(key,SHSIZE,IPC_CREAT | 0666);
	if(shmid < 0) { perror("smhget"); exit(1); }
	char* shm = (char*)shmat(shmid,NULL,0);
	if(shm == (char*)-1) { perror("shmat"); exit(1); }
	if(strstr(shm,id) == NULL){
		cout<<"Client doesn't exist\n";
		return;
	}
	char* messages_pipe = "/tmp/messages";
	mkfifo(messages_pipe,0666);
	int fd = open(messages_pipe,O_RDWR);
	if(fd < 0)
		cout<<"ERROROROROROROOR\n";
	write(fd,id,strlen(id));
	write(fd,text,strlen(text));
}

int main()
{
        registration();       
        int n = 1;
        while(n){
        	cin>>n;
        	if(n == 1)
        		client_list();
        	if(n == 2){
        		char id[BUFSIZE];
        		char text[BUFSIZE];
        		cin>>id>>text;
        		msg(text,id);
        	}	
        }
}
