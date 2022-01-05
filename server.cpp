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
#define CLIENT_COUNT 10

pthread_t client_threads[CLIENT_COUNT] = {0};

void startup_server() {
	key_t key = 9874;
	int shmid = shmget(key, SHSIZE, IPC_CREAT | 0666);
	if (shmid < 0) { perror("shmget"); exit(1); }
  char* shm = (char*)shmat(shmid, NULL, 0);
	if (shm == (char *) -1) { perror("shmat"); exit(1); }
	char up_message[] = "server is on\n";
 	memcpy(shm, up_message, sizeof(up_message));	
}

void* client(void* args) {
	char* child_pipe = (char*) args;
	cout<<"Client registered - "<<child_pipe<< endl;
	free(child_pipe);
}

int create_pipe() {
	char *messages_from_server = "/tmp/registration";
  mkfifo(messages_from_server, 0666);
  int fd = open(messages_from_server, O_RDWR);
	return fd;
}


void start_to_listen() {

	key_t key = 9874;
	int shmid = shmget(key, SHSIZE, IPC_CREAT | 0666);
	if (shmid < 0) { perror("shmget"); exit(1); }
	char* shm = (char*)shmat(shmid, NULL, 0);
	if (shm == (char *) -1) { perror("shmat"); exit(1); }
	int fd = create_pipe();
	int connected_client_count = 0;

	char client_pid[BUFSIZE];
	char buf[BUFSIZE];
	int fd_child;
	while(read(fd, client_pid, 5)) {
		if (connected_client_count < CLIENT_COUNT) {
			strcat(shm,client_pid);
			strcat(shm,"\n");
			char* child_pipe = (char*)malloc(BUFSIZE);
			sprintf(child_pipe, "/tmp/client_%s", client_pid);
			pthread_create(&client_threads[connected_client_count], NULL, 
				              client, child_pipe);
      pthread_join(client_threads[connected_client_count], NULL);       	        
      read(fd,buf,BUFSIZE);
		} else  cout<<"Enough\n";		
	}
}
int main(int argc, char* argv[])
{
	      startup_server();
        start_to_listen();
}
