#include<stdio.h>
#include<string.h>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#include<termios.h>

pthread_t tid[10];
int hunger=200, hygiene=100 , health=300, food=10, foodstock = 10, bath=0;
int enemyhealth, attack = 20;
int clearstatus=0, win = 0, turn = 0, buy = 0;
int i,j,k;
int lose = 0;
int act = 0, menus = 1;
char nama[20];
key_t key = 100;
int *value;


void *clearScreen()
{
    while(!lose)
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
    while(!lose)
    {
        act = getAct() - '0';
        if(act==1 && menus == 1)
        {
            hunger += 15;
            if(hunger > 200)
                hunger = 200;
            food--;
            i = 1;
            system("clear");
            clearstatus = 0;
        }
        else if(act==2 && menus == 1)
        {
            if(bath==0)
            {
                j = 1;
                hygiene += 30;
                if(hygiene > 100)
                    hygiene = 100;
                bath = 20;
                system("clear");
                clearstatus = 0;
            }
        }
        else if(act==3 && menus == 1)
        {
            turn = 0;
            menus = 2;
            enemyhealth = 100;
            win = 0;
            system("clear");
            clearstatus = 0;
        }
        else if(act==4 && menus == 1)
        {
            menus = 3;
            system("clear");
            clearstatus = 0;
        }
        else if(act==5 && menus == 1)
        {
            printf("Exiting the game...\n");
            exit(0);
        }
        else if(act==1 && menus == 2)
        {
            enemyhealth -= attack;
            turn++;
            if(enemyhealth <= 0)
            {
                enemyhealth = 0;
                win = 1;
                sleep(2);
                continue;
            }
            system("clear");
            clearstatus = 0;
            sleep(1);

            turn++;
            system("clear");
            clearstatus = 0;
            sleep(1);

            health -= attack;
            turn++;
            if(health <= 0)
            {
                health = 0;
                lose = 3;
                continue;
            }
            system("clear");
            clearstatus = 0;
            sleep(1);

            turn++;
            system("clear");
            clearstatus = 0;
        }
        else if(act==2 && menus == 2)
        {
            menus = 1;
            system("clear");
            clearstatus = 0;
        }
        else if(act==1 && menus == 3)
        {
            buy = ((foodstock > 0) ? 1 : -1);
            system("clear");
            clearstatus = 0;
            if(buy == 1)
            {
                food++;
                value[0]--;
            }
            sleep(1);
            buy = 0;
            system("clear");
            clearstatus = 0;
        }
        else if(act==2 && menus == 3)
        {
            menus = 1;
            system("clear");
            clearstatus = 0;
        }
    }
}

void *hungry()
{
    while(!lose)
    {
        if(hunger>0 && menus != 2)
        {
            for(i=1;i<=10;i++)
            {
                if(menus == 2)
                    break;
                sleep(1);
                if(i==10)
                    hunger -= 5;
            }
        }
        if(hunger<=0)
        {
            lose = 1;
            menus = 1;
        }
    }
}

void *dirty()
{
    while(!lose)
    {
        if(hygiene>0 && menus != 2)
        {
            for(j=1;j<=30;j++)
            {
                if(menus == 2)
                    break;
                sleep(1);
                if(j==30)
                    hygiene -= 10;
            }
        }
        if(hygiene<=0)
        {
            lose = 2;
            menus = 1;
        }
    }
}

void *regen()
{
    while(!lose)
    {
        if(health>0 && health<300 && menus != 2)
        {
            for(k=1;k<=10;k++)
            {
                if(menus == 2)
                    break;
                sleep(1);
                if(k==10)
                    health += 5;
            }
        }
    }
}

void *bathready()
{
    while(!lose)
    {
        if(bath>0)
        {
            sleep(1);
            bath--;
        }
    }
}

void *modes()
{
    while(1)
    {
        if(!clearstatus && menus == 1)
        {
            if(!lose)
                printf("Standby Mode\n");
            printf("Health    : %d\n",health);
            printf("Hunger    : %d\n",hunger);
            printf("Hygiene   : %d\n",hygiene);
            printf("Food left : %d\n",food);
            if(!lose)
            {
                if(bath==0)
                    printf("Bath is ready\n");
                else if(bath>0)
                    printf("Bath will be ready in %d\n",bath);
                printf("\nChoices\n");
                printf("1. Eat\n2. Bath\n3. Battle\n4. Shop\n5. Exit\n");
            }
            else if(lose == 1)
                printf("\nYour monster died of starvation.\n");
            else if(lose == 2)
                printf("\nYour monster died of sickness.\n");
            if(lose > 0)
                exit(0);
            clearstatus = 1;
        }
        if(!clearstatus && menus == 2)
        {
            printf("Battle Mode : %s turn\n",((turn%4<2 ) ? "Your" : "Enemy's"));
            printf("%s's health : %d\n",nama,health);
            printf("Enemy's health : %d\n",enemyhealth);
            if(!win && !lose && turn%4==0)
            {
                printf("\nChoices\n");
                printf("1. Attack\n2. Run\n");
            }
            else if(win)
            {
                printf("\nYou win!!\n");
                sleep(1);
                menus = 1;
            }
            else if(lose == 3)
                printf("\n%s is killed\n",nama);
            if(lose > 0)
                exit(0);
            clearstatus = 1;
        }
        if(!clearstatus && menus == 3)
        {
            printf("Shops\n");
            printf("Shop food stock : %d\n",foodstock);
            printf("Your food stock : %d\n",food);
            if(!buy)
            {
                printf("\nChoices\n");
                printf("1. Buy\n2. Back\n");
            }
            else if(buy==1)
                printf("\nFood is bought!\n");
            else if(buy==-1)
                printf("\nOut of stock...\n");
            clearstatus = 1;
        }
    }
}

void *shareShop()
{
    int shmid1 = shmget(key, sizeof(int), IPC_CREAT | 0666);
    value = shmat(shmid1, NULL, 0);
    
    value[0] = foodstock;

    while(1)
        foodstock = value[0];
}

int main()
{
    system("clear");

    printf("Enter your monster's name: ");
    scanf("%s",nama);

    system("clear");

    pthread_create(&(tid[0]),NULL,modes,NULL);
    pthread_create(&(tid[1]),NULL,hungry,NULL);
    pthread_create(&(tid[2]),NULL,dirty,NULL);
    pthread_create(&(tid[3]),NULL,regen,NULL);
    pthread_create(&(tid[4]),NULL,bathready,NULL);
    pthread_create(&(tid[5]),NULL,clearScreen,NULL);
    pthread_create(&(tid[6]),NULL,waitInput,NULL);
    pthread_create(&(tid[7]),NULL,shareShop,NULL);

    pthread_join(tid[0],NULL);
    pthread_join(tid[1],NULL);
    pthread_join(tid[2],NULL);
    pthread_join(tid[3],NULL);
    pthread_join(tid[4],NULL);
    pthread_join(tid[5],NULL);
    pthread_join(tid[6],NULL);
    pthread_join(tid[7],NULL);
    
    exit(0);
}