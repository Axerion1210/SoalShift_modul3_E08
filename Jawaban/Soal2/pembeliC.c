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



#define PORT 8081

pthread_t tid[3];
int stock = 0;
 struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    char *kurang = "kurang";
    char *tes = "tes";
    char *keluar = "keluar";
    char buffer[1024] = {0};
void* kirim(void *arg)
{
	unsigned long i=0;
	pthread_t id=pthread_self();
	if(pthread_equal(id,tid[0]))
	{
		int flag = 1;
        int choice;
        while (flag){
            system("clear");
            printf("%s\n",buffer);
            printf("\n1. Beli gan!.\n-1. Quit\nChoice: ");
            scanf("%d", &choice);
            if(choice==1){
                send(sock, kurang, strlen(kurang) , 0 );
                valread = read( sock , buffer, 1024);
            }else if(choice == -1){
                flag=0;
                send(sock , keluar , strlen(keluar) , 0 );
            }
            fflush(stdout);
            memset(buffer, 0, 1024*sizeof(buffer[0])); 
       }
	}
	return NULL;
}

void* terima(void *arg)
{
	unsigned long i=0;
	pthread_t id=pthread_self();
	if(pthread_equal(id,tid[1])) //thread untuk menjalankan counter
	{
		int flag = 1;
        while (flag){
            valread = read( sock , buffer, 1024);
            if(valread!=0){
                system("clear");
                printf("%s\n",buffer);
                printf("1. Tambah Stock.\n-1. Quit\nChoice: ");    
                fflush(stdout);
            }
            memset(buffer, 0, 1024*sizeof(buffer[0])); 
        }
	}
	return NULL;
}


int main(int argc, char const *argv[]) {
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }
  
    memset(&serv_addr, '0', sizeof(serv_addr));
  
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
      
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }
  
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    

    pthread_create(&(tid[0]),NULL,&kirim,NULL);
    //pthread_create(&(tid[1]),NULL,&terima,NULL);
    
    pthread_join(tid[0],NULL);
    //pthread_join(tid[1],NULL);
    return 0;
}