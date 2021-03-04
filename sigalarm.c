#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

static int alarm_fired = 0;

void ding(int sig)
{
    alarm_fired = 1;
}

int main(int argc, char * argv[])
{
    pid_t pid;
    int time = atoi(argv[1]);

    pid = fork();
    switch(pid) 
    {
    case -1:
        perror("fork failed");
        exit(1);
    case 0:
    //child
        sleep(time);
        printf("Waiting acknowledge from %d\n", pid);
        kill(getppid(), SIGALRM);
        
        
        (void) signal(SIGALRM, ding);
        pause();
        if (alarm_fired){
            printf("Received acknowledge from %d\n", pid);
        }
        else
        {
            printf("Did not receive acknowledge from %d\n");
        }
        break;
    default:
    //parent
        printf("Waiting on alarm signal from %d\n", pid);
        (void) signal(SIGALRM, ding);
        
        pause();
        if (alarm_fired){
            printf("Received alarm signal from %d\n", pid);
            alarm_fired = 0;
            kill(pid, SIGALRM);
        }
        else 
        {
            printf("Did not receive acknowledge from %d\n");
        }
        break;
    }


    exit(0);
}
