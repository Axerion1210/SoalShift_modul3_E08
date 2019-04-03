#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>


pthread_t tid[10]; //inisialisasi array untuk menampung thread dalam kasus ini ada 2 thread
int flag=1;


void* buatFile(void *arg)
{
	unsigned long i=0;
	pthread_t id=pthread_self();
	int iter;

	if(pthread_equal(id,tid[0])) // thread menampilkan gambar
	{
		system("mkdir -p /home/siung2/Documents/FolderProses1 && ps -aux | head -10 > /home/siung2/Documents/FolderProses1/SimpanProses1.txt");
		//flag=0;
	}
	else if(pthread_equal(id,tid[1])) // thread menutup gambar
	{	
		//while (flag==1);
		system("mkdir -p /home/siung2/Documents/FolderProses2 && ps -aux | head -10 > /home/siung2/Documents/FolderProses2/SimpanProses2.txt");		
	}
	return NULL;
}

void* kompresFile(void *arg)
{
	unsigned long i=0;
	pthread_t id=pthread_self();
	int iter;

	if(pthread_equal(id,tid[2])) // thread menampilkan gambar
	{
		system("cd /home/siung2/Documents/FolderProses1 && zip -m KompresProses1.zip SimpanProses1.txt");
		//flag=0;
	}
	else if(pthread_equal(id,tid[3])) // thread menutup gambar
	{	
		//while (flag==1);
		system("cd /home/siung2/Documents/FolderProses2 && zip -m KompresProses2.zip SimpanProses2.txt");
	}
	return NULL;
}

void* ekstrakFile(void *arg)
{
	unsigned long i=0;
	pthread_t id=pthread_self();
	int iter;

	if(pthread_equal(id,tid[4])) // thread menampilkan gambar
	{
		system("unzip -o /home/siung2/Documents/FolderProses1/KompresProses1.zip -d /home/siung2/Documents/FolderProses1");
		//flag=0;
	}
	else if(pthread_equal(id,tid[5])) // thread menutup gambar
	{	
		//while (flag==1);
		system("unzip -o /home/siung2/Documents/FolderProses2/KompresProses2.zip -d /home/siung2/Documents/FolderProses2");
	}
	return NULL;
}


int main(void)
{
	int i=0;
	int err;
	while(i<2) // loop sejumlah thread
	{
		err=pthread_create(&(tid[i]),NULL,&buatFile,NULL); //membuat thread
		if(err!=0) //cek error
		{
			//printf("\n can't create thread : [%s]",strerror(err));
		}
		else
		{
			//printf("\n create thread success\n");
		}
		i++;
	}
	pthread_join(tid[0],NULL);
	pthread_join(tid[1],NULL);
	while(i<4) // loop sejumlah thread
	{
		err=pthread_create(&(tid[i]),NULL,&kompresFile,NULL); //membuat thread
		if(err!=0) //cek error
		{
			//printf("\n can't create thread : [%s]",strerror(err));
		}
		else
		{
			//printf("\n create thread success\n");
		}
		i++;
	}
    
    pthread_join(tid[2],NULL);
	pthread_join(tid[3],NULL);

    printf("\n\nMenunggu 15 detik untuk mengekstrak kembali");
    int timer=0;
    while (timer < 15){
        printf(". ");
        fflush(stdout);
        ++timer;
        sleep(1);
    }
    printf("\n");
    
    while(i<6) // loop sejumlah thread
	{
		err=pthread_create(&(tid[i]),NULL,&ekstrakFile,NULL); //membuat thread
		if(err!=0) //cek error
		{
			//printf("\n can't create thread : [%s]",strerror(err));
		}
		else
		{
			//printf("\n create thread success\n");
		}
		i++;
	}
	pthread_join(tid[4],NULL);
	pthread_join(tid[5],NULL);
    exit(0);
	return 0;
}