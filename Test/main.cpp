#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <semaphore.h>

#include "dgIoTApi.h"
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
    SDK_DEVICE_INFO device;
    memset(&device,0, sizeof(device));
    stpcpy(device.serial,"test123465");
    do{
       // Init();
       // Run();
        DGSDK_login( device,nullptr, nullptr,"./config.json");

        sem_wait(&sem_signal); //等待

    }while(0);
    
    return 0;
}