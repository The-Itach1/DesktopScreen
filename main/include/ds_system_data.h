#ifndef _DS_SYSTEM_DATA_H_
#define _DS_SYSTEM_DATA_H_

#include <stdio.h>
#include <string.h>
#include <stdbool.h>


#define MAX_SETTING_SSID_LEN 32
#define MAX_SETTING_PSW_LEN 64
#define MAX_SETTING_CITY_LEN 15

typedef enum{
    TP_ACTION_NULL = 0,
    //短按
    TP_ACTION_SHORT,  
    //长按
    TP_ACTION_LONG,        
    //滑动
    TP_ACTION_MOVE_LEFT,
    TP_ACTION_MOVE_RIGHT,
    TP_ACTION_MOVE_UP,
    TP_ACTION_MOVE_DOWN,
}TP_ACTION_E;

typedef enum{
    SETTING_NULL =0,
    SETTING_ING,
    SETTING_WAIT,
    SETTING_FINISH,
}SETTING_STATUS_E;

//AP模式状态
typedef enum{
    WIFI_AP_MODE_INIT = 0,
    WIFI_AP_MODE_CONNECT,
    WIFI_AP_MODE_DISCONNECT,
}WIFI_AP_MODE_STATUS_E;

//STA模式状态
typedef enum{
    WIFI_STA_MODE_INIT = 0,
    WIFI_STA_MODE_CONNECT_SUCCESS,
    WIFI_STA_MODE_CONNECT_FAIL,
    WIFI_STA_MODE_CONNECT_TIMEOUT,
}WIFI_STA_MODE_STATUS_E;

//触摸动作管理
typedef struct{
    int tp_interval_timecount;
    uint8_t status;
    uint16_t tp_start_x;
	uint16_t tp_start_y;
    uint16_t tp_stop_x;
	uint16_t tp_stop_y;
    TP_ACTION_E tp_action;
}TP_ACTION_MANAGE_T;

typedef struct
{
    //wiif配置相关
    char setting_ssid[MAX_SETTING_SSID_LEN];
    uint8_t setting_ssid_len;
    char setting_psw[MAX_SETTING_PSW_LEN];
    uint8_t setting_psw_len;
    bool has_wifi_info;

    //网络状态相关
    WIFI_AP_MODE_STATUS_E wifi_ap_status;
    WIFI_STA_MODE_STATUS_E wifi_sta_status;
    bool is_ap_sta_open;

    //http请求相关
    bool first_time_httpdata_request;
    //刷新http数据请求
    bool update_httpdata_request;

    //城市相关
    char setting_city[MAX_SETTING_CITY_LEN];
    uint8_t setting_city_len;

    //番茄时钟相关
    //工作时间 休息时间 轮次
    uint8_t tomato_work_time;
    uint8_t tomato_rest_time;
    uint8_t tomato_time_count;

    //触摸屏管理相关
    TP_ACTION_MANAGE_T tp_action_manage;
    //TP唤醒剩余时间，用于进入休眠计时
    uint32_t tp_wackup_timeleft;

    //系统时间相关
    uint8_t hour;
    uint8_t minute;
    uint8_t second;

    uint8_t tomato_left_time;

}SYSTRM_DATA_T;

////////////////////SYSTEM DATA////////////////////////////////
void ds_system_data_init(void);
SYSTRM_DATA_T get_system_data(void);

//////////////////WIFI INFO MANAGE //////////////////////////////
void set_system_data_wifi_info(char * p_ssid,char *p_psw);
void print_system_data_wifi_info(void);
bool has_wifi_info(void);

//////////////////NET STATUS/////////////////////////////////////
WIFI_STA_MODE_STATUS_E get_wifi_sta_status();
void set_wifi_sta_status(WIFI_STA_MODE_STATUS_E status);
WIFI_AP_MODE_STATUS_E get_wifi_ap_status();
void set_wifi_ap_status(WIFI_AP_MODE_STATUS_E status);
bool get_is_ap_sta_open();
void set_is_ap_sta_open(bool isopen);

////////////////HTTP//////////////////////
bool has_first_time_httpdata_request();
void set_has_first_time_httpdata_request(void);
bool has_update_httpdata_request();
void set_update_httpdata_request(bool update);

//////////////////SYSTEM SETTING////////////////////////////////
void set_system_data_city_info(char * p_city);
void set_system_data_tomato_info(char * work_time,char * rest_time,char * count_time);


////////////////////TP ACTION MANAGE ////////////////////////////
void clear_tp_action(void);
uint8_t get_tp_action_status(void);
TP_ACTION_E get_tp_action(void);
void check_tp_action(void);
void set_tp_action_manage_stop_point(uint16_t x,uint16_t y);
void set_tp_action_manage_start_point(uint16_t x,uint16_t y);
void count_tp_action_manage_time(void);
void reset_tp_action_manage(void);

void set_tp_wackup_timeleft(uint32_t timeleft);
void count_tp_wackup_timeleft();
uint32_t get_tp_wackup_timeleft();

////////////////TIME/////////////////////
void update_system_time(uint8_t hour,uint8_t minute,uint8_t second);
void update_system_time_second();
void update_system_time_minute();

#endif

