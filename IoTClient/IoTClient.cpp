#include <pthread.h>
#include <cstring>
#include <unistd.h>
#include "IoTClient.h"
#include "glog/logging.h"

extern "C"
{
    #include "MQTTClient.h"
}

namespace FGWare2{

std::string m_IoTClientID;
std::string m_SubTopic;
std::string m_SSL_Path;
IOT_SERVER m_IoTServer;
bool m_bConnLost;
volatile bool m_bRunning;
void *m_IoTClient;
pthread_t process_thread_;
volatile MQTTClient_deliveryToken deliveredtoken;

void Process(void *_this);
static void *thrdFunc_Process(void *_this);
static void on_connectionLost(void *context, char *cause);
static void on_deliveryComplete(void *context, MQTTClient_deliveryToken token);
static int on_messageArrived(void *context, char *topicName, int topicLen, MQTTClient_message *m);
static void mqtt_log_callback(enum MQTTCLIENT_TRACE_LEVELS level, char *message);

CIoTClient::CIoTClient(const std::string& clientID, IOT_SERVER &server, CIoTClientListener *listener)
{
    m_bRunning = false;
    m_bConnLost = true;
    m_IoTServer = server;
    m_IoTClientID = clientID;
    m_SubTopic = "msg/" + m_IoTClientID;
    m_Listener = listener;
}

CIoTClient::CIoTClient(const std::string& clientID, const std::string &ssl_license_path, IOT_SERVER &server, CIoTClientListener *listener)
{
    m_bRunning = false;
    m_bConnLost = true;
    m_SSL_Path = ssl_license_path;
    m_IoTServer = server;
    m_IoTClientID = clientID;
    m_SubTopic = "msg/" + m_IoTClientID;
    m_Listener = listener;
}

CIoTClient::~CIoTClient(){
    if (NULL != m_IoTClient)
    {
        free(m_IoTClient);
        m_IoTClient = NULL;
    }
}

void CIoTClient::GetIoTServer(IOT_SERVER &server)
{
    m_Mutex.Enter();
    server = m_IoTServer;
    m_Mutex.Leave();
}

void CIoTClient::SetIoTServer(IOT_SERVER &server)
{
    DLOG(INFO) << "CIoTClient::SetIoTServer()" << std::endl;
    m_Mutex.Enter();
    m_IoTServer = server;
    m_bConnLost = true;
    m_Mutex.Leave();
}

void CIoTClient::Run()
{
    DLOG(INFO) << "CIoTClient::Run()" << std::endl;
    if (!m_bRunning){
        m_bRunning = true;
        pthread_create(&process_thread_, NULL, thrdFunc_Process, this);
    }
}

void *thrdFunc_Process(void *_this)
{
    Process(_this);
}

void Process(void *this_)
{
    while(m_bRunning)
    {
        if (NULL != m_IoTClient)
        {
            MQTTClient_destroy((MQTTClient*)&m_IoTClient);
            m_IoTClient = NULL;
        }

        int ret = MQTTClient_create((MQTTClient*)&m_IoTClient, m_IoTServer.server.c_str(), m_IoTClientID.c_str(), MQTTCLIENT_PERSISTENCE_NONE, NULL);
        if (ret != MQTTCLIENT_SUCCESS)
        {
            LOG(ERROR) << "Iot client failed to create, return code : " << ret << ", Server : " << m_IoTServer.server << std::endl;;
            usleep(2000 * 1000); // wait for 2 seconds
            continue;
        }

        MQTTClient_setCallbacks(*((MQTTClient*)&m_IoTClient), this_, on_connectionLost, on_messageArrived, on_deliveryComplete);
        
        /* set log */
        //MQTTClient_setTraceCallback(mqtt_log_callback);
        //MQTTClient_setTraceLevel(MQTTCLIENT_TRACE_PROTOCOL);

        /* set will */
        std::string willMsg = "{\"msg_type\":255" + std::string(",\"deviceid\":\"") + std::string(m_IoTClientID) + std::string("\"}");
        MQTTClient_willOptions will = MQTTClient_willOptions_initializer;
        will.topicName = m_IoTServer.topic_uplink.c_str();
        will.message = willMsg.c_str();
        will.qos = 1;
        will.retained = 0;

        /* Connect to the broker */
        MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
        conn_opts.keepAliveInterval = 20;
        conn_opts.cleansession = 1;
        conn_opts.username = m_IoTServer.user.c_str();
        conn_opts.password = m_IoTServer.passwd.c_str();
        /* set ssl options */
        if (!m_SSL_Path.empty()){
            LOG(INFO) << "IoT SSL加密开启" << std::endl;
            MQTTClient_SSLOptions ssl = MQTTClient_SSLOptions_initializer;
            ssl.trustStore = m_SSL_Path.c_str();
            DLOG(INFO) << "m_SSL_Path : " << m_SSL_Path << std::endl;
            conn_opts.ssl = &ssl;
        }
        conn_opts.will = &will;
        conn_opts.connectTimeout = 10;

        ret = MQTTClient_connect(*((MQTTClient*)&m_IoTClient), &conn_opts);
        if (ret != MQTTCLIENT_SUCCESS)
        {
            LOG(ERROR) << "Iot client failed to connect, return code : " << ret
                          << ", Server : " << m_IoTServer.server
                          << ", User : " << m_IoTServer.user
                          << ", Passwd : " << m_IoTServer.passwd
                          << ", topic_uplink : " << m_IoTServer.topic_uplink
                          << std::endl;
            sleep(5); // wait for 5 seconds
            continue;
        } 
        else 
        {
            //建立连接成功
            m_bConnLost = false;
            MQTTClient_subscribe(*((MQTTClient*)&m_IoTClient), m_SubTopic.c_str(), 1);
            CIoTClient *_this = (CIoTClient *)this_;
            _this->m_Listener->onIoTConnected();
            while(!m_bConnLost)
            {
                sleep(2);
            }
        }
    }
}

int ssl_error_cb(const char *str, size_t len, void *u)
{
    // DLOG(INFO) << "IoT SSL Error : " << std::string(str) << std::endl;
}

void mqtt_log_callback(enum MQTTCLIENT_TRACE_LEVELS level, char *message)
{
    printf("Trace : %d, %s\n", level, message);
}

// Asynchronous mode
void on_connectionLost(void *context, char *cause)
{
    CIoTClient *_this = (CIoTClient *)context;
    m_bConnLost = true;
    // LOG(WARNING) << "Iot client connection lost, and cause : " << cause;
    _this->m_Listener->onIoTDisConnected();
}

void on_deliveryComplete(void *context, MQTTClient_deliveryToken token)
{
    deliveredtoken = token;
}

/*
* on_messageArrived这个回调函数对返回值有要求，
* 如果成功，返回1,表示true，
* 如果失败，返回0,表示false
*/
int on_messageArrived(void *context, char *topicName, int topicLen, MQTTClient_message *m)
{
    CIoTClient *this_ = (CIoTClient *)context;
    DLOG(INFO) << "on_messageArrived !"<< std::endl;
    if (NULL == m || NULL == m->payload || NULL == topicName || 0 == m->payloadlen)
    {
        LOG(WARNING) << "receive message is NULL" << std::endl;
        if (topicName)
            MQTTClient_free(topicName);
        if (m)
            MQTTClient_freeMessage(&m);
        return 1;
    }

    Json::Reader reader;
    Json::Value root;
    int messageType = -1;
    char *message = (char *)(m->payload);
    if (reader.parse(message, root)) // reader将Json字符串解析到root，root将包含Json里所有子元素
    {
        messageType = root["msg_type"].asInt();
    }
    else
    {
        LOG(ERROR) << "can't parse message, and payload is " << message;
        if (topicName)
            MQTTClient_free(topicName);
        if (m)
            MQTTClient_freeMessage(&m);
        return 1;
    }

    if (messageType == IOT_DOWN_FACESYNC_TRIG) /* face sync */
    {
        DLOG(INFO) << "CIoTClient::Receive FaceSync Trig" << std::endl;
        root.removeMember("msg_type");
        this_->m_Listener->onFaceSyncTrig(root);
    }
    else if (messageType == IOT_DOWN_CONFIG)  /* config down */
    {
        DLOG(INFO) << "CIoTClient::Receive config" << std::endl;
        Json::Value jsonItem;
        jsonItem["msg_type"] = IOT_UP_CONFIG_ACK;
        jsonItem["deviceid"] = m_IoTClientID;
        jsonItem["status"] = "false";
        if (!root["config_hash"].empty()){
            this_->m_Listener->SaveConfigHash(root["config_hash"]);
            jsonItem["status"] = "true";
        }
        if (!root["config"].empty()){
            this_->m_Listener->SaveConfigContent(root["config"]);
            jsonItem["status"] = "true";
        }
        std::string jsoncstr = jsonItem.toStyledString();
        this_->IoTUpMqttMsg(jsoncstr);
    }
    else if (messageType == IOT_DOWN_USRCMD) /* user cmd */
    {
        DLOG(INFO) << "CIoTClient::Receive User Cmd" << std::endl;
        root.removeMember("msg_type");
        this_->m_Listener->onUserCmd(root);
    }
    else if (messageType == IOT_DOWN_OTA_TRIG){  /* ota trig */
        DLOG(INFO) << "CIoTClient::Receive ota trig" << std::endl;
        root.removeMember("msg_type");
        this_->m_Listener->onOTATrig(root);
    }

    if (topicName)
        MQTTClient_free(topicName);
    if (m)
        MQTTClient_freeMessage(&m);
    return 1;
}

void CIoTClient::IoTUpMqttMsg(std::string& msg)
{
    if (m_IoTClient != NULL && msg.c_str() != NULL && msg.size() > 0)
    {
        MQTTClient_deliveryToken token = 0;
        deliveredtoken = 100;
        MQTTClient_publish(*((MQTTClient*)&m_IoTClient), m_IoTServer.topic_uplink.c_str(), msg.size(), msg.c_str(), 1, 0, &token);
        // check deliveredtoken
        // if (deliveredtoken != token)
        // {
        //     std::cout << "mqtt msg send failed" << std::endl;
        // }
    }else{
        // LOG(ERROR) << "IoTUpMqttMsg Failed!" << std::endl;
    }
}

void CIoTClient::IoTUpFaceSyncError(std::string& face_id, int err_num)
{
    Json::Value jsonItem;
    jsonItem["msg_type"] = IOT_UP_FACESYNC_ERROR;
    jsonItem["deviceid"] = m_IoTClientID;
    jsonItem["faceid"] = face_id;
    jsonItem["errorNum"] = err_num;

    std::string jsoncstr = jsonItem.toStyledString();
    this->IoTUpMqttMsg(jsoncstr);
}

void CIoTClient::IoTUpDevConfig(){
    if (m_Listener->HasConfigHash()) {
        m_Listener->IoTUpDevConfigHash();
    }
    else
    {
        m_Listener->IoTUpDevConfigContent();
    }
}

void CIoTClient::Stop()
{
    // DLOG(INFO) << "CIoTClient::Stop()" << std::endl;

    if (m_bRunning){
        m_bConnLost = true;
        m_bRunning = false;
        m_Listener->onIoTDisConnected();
        MQTTClient_unsubscribe(*((MQTTClient*)&m_IoTClient), m_SubTopic.c_str());
        MQTTClient_disconnect(*((MQTTClient*)&m_IoTClient), 100L);
        MQTTClient_destroy((MQTTClient*)&m_IoTClient);
        m_IoTClient = NULL;
        pthread_join(process_thread_, NULL);
    }
}

void CIoTClientListener::IoTUpDevFaceDBHash(){}
void CIoTClientListener::IoTUpDevStatus(){}
void CIoTClientListener::IoTUpChInfo(){}

//config
bool CIoTClientListener::HasConfigHash(){return false;}
void CIoTClientListener::IoTUpDevConfigHash(){}
void CIoTClientListener::IoTUpDevConfigContent(){}
void CIoTClientListener::SaveConfigHash(Json::Value& hash){}
void CIoTClientListener::SaveConfigContent(Json::Value& content){}

//IoT Msg Recv
void CIoTClientListener::onOTATrig(Json::Value& iotMessage){}
void CIoTClientListener::onFaceSyncTrig(Json::Value& iotMessage){}
void CIoTClientListener::onUserCmd(Json::Value& iotMessage){}

//IoT Connect status
void CIoTClientListener::onIoTConnected(){}
void CIoTClientListener::onIoTDisConnected(){}

}