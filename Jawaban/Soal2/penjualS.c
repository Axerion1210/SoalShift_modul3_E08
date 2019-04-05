#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include<pthread.h>
#include<sys/types.h>
#include<sys/wait.h>

#include <sys/ipc.h>
#include <sys/shm.h>

#define PORT 8080

key_t key = 1330;
//int *value;
int *stock;
pthread_t tid[3];


int server_fd, new_socket, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";
    char *tambah = "tambah";
    char *keluar = "keluar";
    char banyakStock[100];


void* terima(void *arg)
{
	unsigned long i=0;
	pthread_t id=pthread_self();
	if(pthread_equal(id,tid[0]))
	{   
        int flag = 1;
        while(flag){
            valread = read( new_socket , buffer, 1024);
            if(strcmp(buffer,tambah)==0){
                //printf("%s\n",buffer );
                
                *stock=*stock+1;
                //printf("Masuk bos\n, stock: %d", *stock);
                //send(new_socket , hello , strlen(hello) , 0 );
            }
            else if(strcmp(buffer,keluar)==0){
                flag=0;
            }
            //memset(buffer, 0, 1024*sizeof(buffer[0])); 
        }
	}
	return NULL;
}


void* kirim(void *arg)
{
	unsigned long i=0;
	pthread_t id=pthread_self();
	if(pthread_equal(id,tid[1])) 
	{   
        int flag = 1;
        while(flag){
            sleep(5);
            sprintf(banyakStock,"%d",*stock);
            send(new_socket , banyakStock , strlen(banyakStock) , 0 );
        }
	}
	return NULL;
}

int main(int argc, char const *argv[]) {
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
      
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );
      
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))<0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
    stock = shmat(shmid, NULL, 0);
    *stock=0;

    pthread_create(&(tid[0]),NULL,&terima,NULL);
    pthread_create(&(tid[1]),NULL,&kirim,NULL);
    
    pthread_join(tid[0],NULL);
    //pthread_join(tid[1],NULL);
    shmdt(stock);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}