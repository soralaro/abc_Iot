//
// Created by czx on 19-9-3.
//

#ifndef DGIOTSDK_DGIOTAPI_H
#define DGIOTSDK_DGIOTAPI_H

typedef char INT8_t;
typedef unsigned char UINT8_t;
typedef short INT16_t;
typedef unsigned short UINT16_t;
typedef int INT32_t;
typedef unsigned int UINT32_t;
typedef long long INT64_t;
typedef unsigned long long UINT64_t;
typedef float FLOAT32_t;
typedef double FLOAT64_;

typedef struct {
    INT8_t serial[64]; //设备序列号
    INT8_t brand[32]; //厂商品牌
    INT8_t mode[32]; //设备型号
    INT8_t software[64]; //软件版本号
    INT8_t firmware[64]; //固件版本号
    INT8_t signature[64]; //签名验证码
    INT8_t releasedate[64]; //软件发版日期
    INT8_t lan_ip[128]; //摄像机局域网IP
    INT8_t *config; //默认配置
} SDK_DEVICE_INFO;

typedef struct {
    INT8_t *data; //json格式配置字符串or其他回调数据
    INT32_t size; //数据长度
}SDK_DATA_INFO;

typedef union{
    SDK_DATA_INFO data_info; //j回调数据信息(json配置或其他格式数据)
} SDK_CALLBACK_MESSAGE_UNION;

typedef enum {
    MSG_RESET_DEVICE = 1, //重置默认登录初始化信息.重置后需要重新登录
    MSG_RESTART_SYSTEM = 2, //重启系统消息
    MSG_UPDATE_VIDEO_CONFIG = 3, //视频配置消息，未使用
    MSG_UPDATE_ALARM_CONFIG = 4, //报警配置消息，未使用
    MSG_UPDATE_OVERLAY_CONFIG = 5, //OSD配置消息，未使用
    MSG_UPDATE_JSON_CONFIG = 6, //json配置串消息
    MSG_COMMIT_CONFIG = 7, //上报配置消息
    MSG_UPDATE_FIRMWARE = 8, //固件升级消息
    MSG_PTZ_MOVE_CONTROL = 9, //ptz控制消息，详见3.4.1 PTZ控制命令
    MSG_UPDATE_PTZ_CONFIG = 10, //ptz配置串消息，包括增删改预置位，见3.4.2 预置位配置命令
    MSG_SENSOR_CONTROL = 11 //镜头控制消息，见3.4.3镜头控制命令
} SDK_MESSAGE_TYPE_EM;

typedef INT32_t (*DGSDK_Callback)(SDK_MESSAGE_TYPE_EM aType, SDK_CALLBACK_MESSAGE_UNION *aInfo, void*pUser);


INT32_t  DGSDK_login(SDK_DEVICE_INFO device, DGSDK_Callback callback, void*pUser,const char *config_json_file);
#endif //DGIOTSDK_DGIOTAPI_H
