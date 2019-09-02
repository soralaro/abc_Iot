#ifndef __MY_IOTCLIENT_H__
#define __MY_IOTCLIENT_H__

#include "IoTClient.h"

using namespace FGWare2;

void Init();
void Run();
void Stop();
void Quit();

class MyIoTClientListener : public CIoTClientListener
{
    public:
        MyIoTClientListener();
        ~MyIoTClientListener();
    
    public:
   
        //上传设备信息
        void IoTUpDevInfo();
       
        //接收处理IoT消息
        void onOTATrig(Json::Value& iotMessage);
        void onFaceSyncTrig(Json::Value& iotMessage);
        //自定义协议部分
        void onUserCmd(Json::Value& iotMessage);

        //IoT连接状态
        void onIoTConnected();
        void onIoTDisConnected();
};

#endif // __MY_IOTCLIENT_H__