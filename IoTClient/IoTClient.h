#ifndef __IOT_CLIENT_H__
#define __IOT_CLIENT_H__

#include <iostream>
#include "FGMutex.h"
#include "json/json.h"
#include "json/config.h"

namespace FGWare2{

enum{
    /* core message */
    IOT_UP_DEV_INFO = 1,
    IOT_UP_DEV_STATUS = 2,
    IOT_UP_EXT_DEV_STATUS = 3,
    IOT_UP_DEV_QUIT = 255,

    /* configure message */
    IOT_UP_CONFIG = 11,
    IOT_DOWN_CONFIG = 12,
    IOT_UP_CONFIG_ACK = 13,

    /* faceSync message*/
    IOT_UP_FACEDBHASH = 21,
    IOT_UP_FACESYNC_ERROR = 22,
    IOT_DOWN_FACESYNC_TRIG = 23,

    /* command message*/
    IOT_DOWN_USRCMD = 254,
    IOT_UP_CMDRSP = 254,

    IOT_CMD_FRPC_ON = 1,
    IOT_CMD_FRPC_OFF = 2,
    IOT_CMD_VIDEOPUSH_ON = 3,
    IOT_CMD_VIDEOPUSH_OFF = 4,
    IOT_CMD_IFR = 6,
    IOT_CMD_SFC = 7,

    /* other */
    IOT_DOWN_OTA_TRIG = 30,
    IOT_UP_USRDATA = 31,
    IOT_UP_DEVICELOG = 32,
};

// IOT ERROR NUM
enum{
    IOT_ERR_PASSTIME_NUM = 1,
    IOT_ERR_PASSTIME_END,
    IOT_ERR_PASSTIME_BEYOND,
    IOT_ERR_PASSTIME_FORMAT,
    IOT_ERR_IMAGE_FORMAT,
    IOT_ERR_FACE_DETECT_FAIL,
    IOT_ERR_IMAGE_TOO_LARGE,
    IOT_ERR_IMAGE_SEND_FAIL,
    IOT_ERR_CHANNEL_NOT_EXIST,
    IOT_ERR_FACE_NOT_EXIST,
    IOT_ERR_FACE_REMOVE_FAIL,
    IOT_ERR_IMAGE_SAVE_FAIL,
    IOT_ERR_FACE_UPDATE_FAIL,
    IOT_ERR_FACE_ADD_FAIL,
};

// IoT Server
typedef struct tagIoTServer
{
    std::string server;
    std::string user;
    std::string passwd;
    std::string topic_uplink;
} IOT_SERVER, *LP_IOT_SERVER;

// callback
class CIoTClientListener
{
    public:
        virtual void IoTUpDevInfo() = 0; //must impliment
        virtual void IoTUpDevFaceDBHash();
        virtual void IoTUpDevStatus();
        virtual void IoTUpChInfo();

        //config
        virtual bool HasConfigHash();
        virtual void IoTUpDevConfigHash();
        virtual void IoTUpDevConfigContent();
        virtual void SaveConfigHash(Json::Value& hash);
        virtual void SaveConfigContent(Json::Value& content);

        //IoT Msg Recv
        virtual void onOTATrig(Json::Value& iotMessage);
        virtual void onFaceSyncTrig(Json::Value& iotMessage);
        virtual void onUserCmd(Json::Value& iotMessage);

        //IoT Connect status
        virtual void onIoTConnected();
        virtual void onIoTDisConnected();
};

class CIoTClient{

    public:
        CIoTClient(const std::string& clientID, IOT_SERVER &server, CIoTClientListener *listener);
        CIoTClient(const std::string& clientID, const std::string &ssl_license_path, IOT_SERVER &server, CIoTClientListener *listener);
        ~CIoTClient();

        //start IoTClient thrd
        void Run();
        //stop IoTClient thrd
        void Stop();

        //send IoT Msg base method
        void IoTUpMqttMsg(std::string& msg);
        //send IoT FaceSync Error Msg
        void IoTUpFaceSyncError(std::string& faceID, int errNum);
        
        //动态切换IOT server
        void SetIoTServer(IOT_SERVER &server);
        void GetIoTServer(IOT_SERVER &server);

        //上行消息方法
        void IoTUpDevConfig();

    // protected:
        CIoTClientListener *m_Listener;
    
    private:
        CMutex m_Mutex;
};
};
#endif