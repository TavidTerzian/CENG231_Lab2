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
    char* endptr; // strtol() returns endptr '\0' when successful
    int time = 0;
    // ensure user inputs argument
    if(argc != 2){
        printf("Error: Please provide a command line argument\n");
        exit(EXIT_FAILURE);
    }
    else{
        // ensure user inputted an int argument
        time = strtol(argv[1], &endptr, 10);
        if(*endptr != '\0'){
            fprintf(stderr, "Invalid argument supplied `%s is not an integer\n", argv[1]);
            exit(EXIT_FAILURE);
        }
    }
    pid_t pid;

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
        int nkill = kill(getppid(), SIGALRM);
        if(nkill == -1){
            printf("Error in killing child process\n");
            exit(EXIT_FAILURE);
        }
        // binds the ding function to signal handler - ding()
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
            int nkill = kill(pid, SIGALRM);
            if(nkill == -1){
                    printf("Error in killing parent process\n");
                    exit(EXIT_FAILURE);
            }
        }
        else 
        {
            printf("Did not receive acknowledge from %d\n");
        }
        break;
    }


    exit(0);
}
