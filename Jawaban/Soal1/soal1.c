
#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<math.h>
#include <errno.h>   // for errno
#include <limits.h>  // for INT_MAX
#include <stdlib.h>

pthread_t tid[100];
int ans[100];
int masalah[100];
int pekerja;
int antrian = 1;

void* factorial(void *arg)
{
	pthread_t id=pthread_self();
	int idx;
	for(int i = 1; i < pekerja ; i++){
		if(pthread_equal(id,tid[i])){
			ans[i]=1;	
            idx=i;
			for(int j = 1; j <= masalah[i]; j++){
				ans[i]*=j;
			}
            while(antrian != i);
            printf("%d! = %d\n", masalah[i],ans[i]);
            antrian++;
		}
	}
    
	return NULL;
}

int cmpfunc(const void * a, const void * b){
	return (*(int*)a - *(int*)b);
}

int main(int argc, char *argv[])
{	
	pekerja=argc;

	for(int i = 1; i < argc; i++){
		char *p;
		int conv = strtol(argv[i], &p, 10);
		masalah[i]=conv;
	}
    qsort(masalah,argc,sizeof(int), cmpfunc); 
	for(int i=1; i < argc; i++){
	    pthread_create(&(tid[i]),NULL,&factorial,NULL);
	}
	for(int i=1; i<argc; i++){
        pthread_join(tid[i],NULL);
	}
	return 0;
}
