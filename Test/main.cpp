#include <signal.h>
#include <semaphore.h>

#include "MyIoTClientListener.h"

sem_t sem_signal;

static void SigHandler(int sig)
{
    sem_post(&sem_signal);
}

void InitSignals()
{
    sem_init(&sem_signal, 0, 0);

    /*signal handlers*/
    signal(SIGQUIT, SigHandler);
    signal(SIGINT, SigHandler);
    signal(SIGTERM, SigHandler);
    signal(SIGHUP, SigHandler);
}

int main()
{
    InitSignals();

    do{
        Init();
        Run();

        sem_wait(&sem_signal); //等待

        Stop();
        Quit();
    }while(0);
    
    return 0;
}