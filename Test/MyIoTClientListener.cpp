#include "MyIoTClientListener.h"

CIoTClient *g_IoTClient = NULL;
MyIoTClientListener *g_IoTListener = NULL;

IOT_SERVER Server;

const std::string deviceID = "test123465";
const std::string ssl_path = "/home/deepglint/testssl.crt";

void Init()
{
    //create listener
    g_IoTListener = new MyIoTClientListener();

    /**
     * create iot client
     * for normal connection: use tcp://ip:port
     * for encrypt connection: use ssl://ip:port
     */
    //Server.server = "tcp://123.59.135.207:1883";
    Server.server = "tcp://123.59.135.181:1883";
    Server.user = "dfcdn";
    Server.passwd = "dfcdn123";
    Server.topic_uplink = "topic/dfcdn";
    /**
     * create encrypt iot client
     */
  //  g_IoTClient = new CIoTClient(deviceID, ssl_path, Server, g_IoTListener);
    
    // /**
    //  * create normal iot client 
    //  */
    g_IoTClient = new CIoTClient(deviceID, Server, g_IoTListener);
}

void Run()
{
    g_IoTClient->Run();
}

void Stop()
{
    g_IoTClient->Stop();
}

void Quit()
{
    delete g_IoTClient;
    g_IoTClient = NULL;
}

MyIoTClientListener::MyIoTClientListener()
{

}

MyIoTClientListener::~MyIoTClientListener()
{

}

//IoT连接状态
void MyIoTClientListener::onIoTConnected()
{
    IoTUpDevInfo();
}

void MyIoTClientListener::onIoTDisConnected()
{

}

//上传设备信息
void MyIoTClientListener::IoTUpDevInfo()
{
    Json::Value jsonItem; //定义一个Json对象
    // message type
    jsonItem["msg_type"] = IOT_UP_DEV_INFO;
    // device Id
    jsonItem["deviceid"] = deviceID;
    // device name
    jsonItem["devicename"] = "FGWare2测试";
    // device model
    jsonItem["devmodel"] = "HaomuL";
    // app name
    jsonItem["appname"] = "AppName";
    // app version
    jsonItem["appver"] = "AppVersion";
    // os name
    jsonItem["osname"] = "OsName";
    // os version
    jsonItem["osver"] = "OsVersion";
    // device ip
    jsonItem["deviceip"] = "DeviceIP";
    // iot protocol version
    jsonItem["protover"] = 2;
    // 应用版本
    std::string jsoncstr = jsonItem.toStyledString();

    //上传消息
    g_IoTClient->IoTUpMqttMsg(jsoncstr);
}

//接收处理IoT消息
void MyIoTClientListener::onOTATrig(Json::Value& iotMessage)
{
    //接收到OTA升级消息
}

void MyIoTClientListener::onFaceSyncTrig(Json::Value& iotMessage)
{
    //接收到人脸同步消息
}

void MyIoTClientListener::onUserCmd(Json::Value& iotMessage)
{
    //接收到产品自定义消息

}