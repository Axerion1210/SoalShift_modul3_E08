#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>

int WakeUp_Status = 0, Spirit_Status = 100;
int cbangun=0, cspirit=0, flag=0;
int fbangun=0, fspirit=0;
int finish = 0;
char pilih[20];
pthread_t tid[2];

void *status()
{
    while(finish == 0)
    {
        if(flag == 1)
        {
            printf("Agmal WakeUp_Status = %d\n",WakeUp_Status);
            printf("Iraj Spirit_Status = %d\n",Spirit_Status);
            flag = 0;
        }
    }
    return NULL;
}

void *bangun()
{
    while(finish==0)
    {
        if(cspirit >= 3)
        {
            cspirit -= 3;
            printf("Fitur Agmal Ayo Bangun disabled 10 s\n");
            fbangun = 1;
            sleep(10);
            printf("Fitur Agmal Ayo Bangun enabled\n");
            fbangun = 0;
        }
        else if(flag == 2)
        {
            WakeUp_Status += 15;
            if(WakeUp_Status >= 100)
                finish = 1;
            flag = 0;
        }
    }
    return NULL;
}

void *tidur()
{
    while(finish == 0)
    {
        if(cbangun >= 3)
        {
            cbangun -= 3;
            printf("Fitur Iraj Ayo Tidur disabled 10 s\n");
            fspirit = 1;
            sleep(10);
            printf("Fitur Iraj Ayo Tidur enabled\n");
            fspirit = 0;
        }
        else if(flag == 3)
        {
            Spirit_Status -= 20;
            if(Spirit_Status <= 0)
                finish = 2;
            flag = 0;
        }
    }
    return NULL;
}

int main()
{
    pthread_create(&(tid[0]), NULL, &status, NULL);
    pthread_create(&(tid[1]), NULL, &bangun, NULL);
    pthread_create(&(tid[2]), NULL, &tidur, NULL);

    do
    {
        scanf("%[^\n]s",pilih);
        getchar();
        if(strcmp(pilih,"All Status")==0)
        {
            flag = 1;
        }
        else if(strcmp(pilih,"Agmal Ayo Bangun")==0)
        {
            if(fbangun == 1)
                printf("Fitur Agmal Ayo Bangun masih disabled!!\n");
            else if(cspirit < 3)
            {
                cbangun++;
                flag = 2;
            }
        }
        else if(strcmp(pilih,"Iraj Ayo Tidur")==0)
        {
            if(fspirit == 1)
                printf("Fitur Iraj Ayo Tidur masih disabled!!\n");
            else if(cbangun < 3)
            {
                cspirit++;
                flag = 3;
            }
        }
    }while (finish == 0);

    if(finish == 1)
        printf("Agmal terbangun, mereka bangun pagi dan berolahraga\n");
    else if(finish == 2)
        printf("Iraj ikut tidur, dan bangun kesiangan bersama Agmal\n");

    pthread_join(tid[0],NULL);
    pthread_join(tid[1],NULL);
    pthread_join(tid[2],NULL);

    exit(0);
    return 0;
}
