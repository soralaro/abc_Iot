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
INT32_t callback(SDK_MESSAGE_TYPE_EM aType, SDK_CALLBACK_MESSAGE_UNION *aInfo, void*pUser)
{
   return 0;
}
int main()
{
    InitSignals();
    SDK_DEVICE_INFO device_info;
    memset(&device_info,0, sizeof(device));
    stpcpy(device_info.serial,"test123465");
    stpcpy(device_info.lan_ip,"127.0.0.1");
    int user_id=1;
    do{
        DGSDK_login( device_info, callback, &user_id,"./config.json");

        sem_wait(&sem_signal); //等待
        DGSDK_logout();

    }while(0);
    
    return 0;
}