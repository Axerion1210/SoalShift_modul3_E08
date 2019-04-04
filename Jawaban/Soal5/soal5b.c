#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<termios.h>

int clearstatus = 0;
int act = 0, foodstock, restock = 0;
pthread_t tid[3];
key_t key = 100;
int *value;

void *clearScreen()
{
    while(1)
    {
        if(clearstatus)
        {
            sleep(1);
            system("clear");
            clearstatus = 0;
        }
    }
}

int getAct()
{
    int ch;
    struct termios oldt, newt;
    
    tcgetattr ( STDIN_FILENO, &oldt );
    newt = oldt;
    newt.c_lflag &= ~( ICANON | ECHO );
    tcsetattr ( STDIN_FILENO, TCSANOW, &newt );
    ch = getchar();
    tcsetattr ( STDIN_FILENO, TCSANOW, &oldt );
    
    return ch;
}

void *waitInput()
{
    while(1)
    {
        act = getAct() - '0';
        if(act == 1)
        {
            restock = 1;
            system("clear");
            clearstatus = 0;
            value[0] = 10;
            sleep(1);
            restock = 0;
            system("clear");
            clearstatus = 0;
        }
        else if(act == 2)
        {
            printf("Closing the shop...\n");
            exit(0);
        }
    }
}

void *shop()
{
    while(1)
    {
        if(!clearstatus)
        {
            printf("Shop\n");
            printf("Food stock : %d\n",foodstock);
            if(!restock)
            {
                printf("\nChoices\n");
                printf("1. Restock\n2. Exit\n");
            }
            else
            {
                printf("\nFood is restocked!\n");
            }
            clearstatus = 1;
        }
    }
}

void *shareShop()
{
    int shmid1 = shmget(key, sizeof(int), IPC_CREAT | 0666);
    value = shmat(shmid1, NULL, 0);

    while(1)
        foodstock = value[0];
}

int main()
{
    pthread_create(&(tid[0]),NULL,clearScreen,NULL);
    pthread_create(&(tid[1]),NULL,shop,NULL);
    pthread_create(&(tid[2]),NULL,waitInput,NULL);
    pthread_create(&(tid[3]),NULL,shareShop,NULL);

    system("clear");
    clearstatus = 0;

    pthread_join(tid[0],NULL);
    pthread_join(tid[1],NULL);
    pthread_join(tid[2],NULL);
    pthread_join(tid[3],NULL);

    exit(0);
}