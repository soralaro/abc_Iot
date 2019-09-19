//
// Created by czx on 19-9-3.
//
#include <iostream>
#include <fstream>
#include <string>
#include <json/json.h>
#include "dgIoTApi.h"
#include "IoTClient.h"
#include "glog/logging.h"



class MyIoTClientListener : public FGWare2::CIoTClientListener{
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

    std::string deviceID;
    const std::string devmodel="DC31-";//”DGSDK";
    const std::string SDK_VERSION = "1.0.0";
    std::string deviceIP ;

};
FGWare2::CIoTClient *g_IoTClient = nullptr;
MyIoTClientListener *g_IoTListener = nullptr;
static const std::string ssl_path = "./testssl.crt";

static FGWare2::IOT_SERVER Server;


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
    jsonItem["msg_type"] = FGWare2::IOT_UP_DEV_INFO;
    // device Id
    jsonItem["deviceid"] = deviceID;
    // device name
    jsonItem["devicename"] = "DGSDK测试";
    // device model
    jsonItem["devmodel"] =devmodel;
    // app name
    jsonItem["appname"] = "AppName";
    // app version
    jsonItem["appver"] = SDK_VERSION;
    // os name
    jsonItem["osname"] = "OsName";
    // os version
    jsonItem["osver"] = "OsVersion";
    // device ip
    jsonItem["deviceip"] = deviceIP;
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








INT32_t  DGSDK_login(SDK_DEVICE_INFO device, DGSDK_Callback callback, void*pUser,const char *config_json_file)
{
    //create listener
    if(g_IoTClient!= nullptr)
    {
        LOG(ERROR) <<"repeat login!"<< std::endl;
        return false;
    }



    std::ifstream json_file(config_json_file, std::ios::binary);

    if (!json_file.is_open())
    {
        LOG(ERROR) <<"Can not open config json file : "<< config_json_file<< std::endl;
        return false;
    }


    Json::Reader json_reader;
    Json::Value json_value;

   // const char* str="{\"server\":\"tcp://xxx.xxx.xxx.xxx:xxxx\"}";
    if (!json_reader.parse(json_file, json_value))
    {
        LOG(ERROR) <<"config_json_file: "<< config_json_file <<" faile to parse!"<<std::endl;
        json_file.close();
        return false;
    }
    json_file.close();



    /**
     * create iot client
     * for normal connection: use tcp://ip:port
     * for encrypt connection: use ssl://ip:port
     */
    LOG(ERROR) <<"config_json_file:"<<config_json_file;
    if(json_value["server"].isNull())
    {
        LOG(ERROR) << "config_json_file:" << config_json_file << " server is null";
        return false;
    }
    if(json_value["user"].isNull())
    {
        LOG(ERROR) << "config_json_file:" << config_json_file << " user is null";
        return false;
    }
    if(json_value["passwd"].isNull())
    {
        LOG(ERROR) << "config_json_file:" << config_json_file << " passwd is null";
        return false;
    }
    if(json_value["topic_uplink"].isNull())
    {
        LOG(ERROR) << "config_json_file:" << config_json_file << " passwd is null";
        return false;
    }
    Server.server=json_value["server"].asCString();
    Server.user=json_value["user"].asCString();
    Server.passwd =  json_value["passwd"].asCString();
    Server.topic_uplink =  json_value["topic_uplink"].asCString();
    /**
     * create encrypt iot client
     */
    //  g_IoTClient = new CIoTClient(deviceID, ssl_path, Server, g_IoTListener);

    // /**
    //  * create normal iot client
    //  */
    g_IoTListener = new MyIoTClientListener();
    g_IoTListener->deviceID=device.serial;
    g_IoTListener->deviceIP=device.lan_ip;
    g_IoTClient = new FGWare2::CIoTClient(deviceID, Server, g_IoTListener);
    g_IoTClient->Run();
}

void   DGSDK_logout()
{
    g_IoTClient->Stop();
    delete g_IoTListener;
    g_IoTListener= nullptr;
    delete g_IoTClient;
    g_IoTClient= nullptr;
}
