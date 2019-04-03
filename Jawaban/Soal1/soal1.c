
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

void merge(int arr[], int l, int m, int r) 
{ 
    int i, j, k; 
    int n1 = m - l + 1; 
    int n2 =  r - m; 
  
    /* create temp arrays */
    int L[n1], R[n2]; 
  
    /* Copy data to temp arrays L[] and R[] */
    for (i = 0; i < n1; i++) 
        L[i] = arr[l + i]; 
    for (j = 0; j < n2; j++) 
        R[j] = arr[m + 1+ j]; 
  
    /* Merge the temp arrays back into arr[l..r]*/
    i = 0; // Initial index of first subarray 
    j = 0; // Initial index of second subarray 
    k = l; // Initial index of merged subarray 
    while (i < n1 && j < n2) 
    { 
        if (L[i] <= R[j]) 
        { 
            arr[k] = L[i]; 
            i++; 
        } 
        else
        { 
            arr[k] = R[j]; 
            j++; 
        } 
        k++; 
    } 
  
    /* Copy the remaining elements of L[], if there 
       are any */
    while (i < n1) 
    { 
        arr[k] = L[i]; 
        i++; 
        k++; 
    } 
  
    /* Copy the remaining elements of R[], if there 
       are any */
    while (j < n2) 
    { 
        arr[k] = R[j]; 
        j++; 
        k++; 
    } 
} 
  
/* l is for left index and r is right index of the 
   sub-array of arr to be sorted */
void mergeSort(int arr[], int l, int r) 
{ 
    if (l < r) 
    { 
        // Same as (l+r)/2, but avoids overflow for 
        // large l and h 
        int m = l+(r-l)/2; 
  
        // Sort first and second halves 
        mergeSort(arr, l, m); 
        mergeSort(arr, m+1, r); 
  
        merge(arr, l, m, r); 
    } 
} 


void* factorial(void *arg)
{
	pthread_t id=pthread_self();
	
	for(int i = 1; i < pekerja ; i++){
		if(pthread_equal(id,tid[i])){
			ans[i]=1;	
			for(int j = 1; j <= masalah[i]; j++){
				ans[i]*=j;
			}
		}
	}
	return NULL;
}

int main(int argc, char *argv[])
{	
	pekerja=argc;

	for(int i = 1; i < argc; i++){
		char *p;
		int conv = strtol(argv[i], &p, 10);
		masalah[i]=conv;
	}
	for(int i=1; i < argc; i++){
	    pthread_create(&(tid[i]),NULL,&factorial,NULL);
	}
	for(int i=1; i<argc; i++){
        pthread_join(tid[i],NULL);
	}
	mergeSort(ans, 1, argc-1); 

	for(int i=1; i<argc; i++){
        printf("%d\n", ans[i]);
	}
	return 0;
}