
#include "sdkconfig.h"
#include "esp_system.h"
#include "esp_log.h"

#include "ds_system_data.h"
#include "ds_ui_page_manage.h"
#include "ds_ui_tomatopage.h"
#include "ds_ui_weatherpage.h"
#include "ds_nvs.h"

static const char *TAG = "ds_system_data";

SYSTRM_DATA_T g_system_data;

void ds_system_data_init(){
    memset(&g_system_data,0,sizeof(SYSTRM_DATA_T));
    g_system_data.first_time_httpdata_request = true;
    g_system_data.update_httpdata_request = false;
    g_system_data.has_wifi_info = false;
    g_system_data.hour = 10;
    g_system_data.minute = 10;
}

SYSTRM_DATA_T get_system_data(){
    return g_system_data;
}

//设置wifi信息
void set_system_data_wifi_info(char * p_ssid,char *p_psw){
    int p_ssidlen = strlen(p_ssid);
    int p_pswlen = strlen(p_psw);
    if(p_pswlen >= MAX_SETTING_SSID_LEN || p_ssidlen >= MAX_SETTING_PSW_LEN){
        ESP_LOGE(TAG, "MAX_SETTING_SSID_PWD_LEN ERROR");
        return;
    }
    g_system_data.has_wifi_info = true;
    g_system_data.setting_ssid_len = p_ssidlen;
    g_system_data.setting_psw_len = p_pswlen;
    memcpy(g_system_data.setting_ssid,p_ssid,p_ssidlen);
    memcpy(g_system_data.setting_psw,p_psw,p_pswlen);
    g_system_data.setting_ssid[p_ssidlen] = '\0';
    g_system_data.setting_psw[p_pswlen] = '\0';
}

//是否已经存储到wifi信息
bool has_wifi_info(){
    return g_system_data.has_wifi_info;
}

//打印wifi信息
void print_system_data_wifi_info(){
    printf("wifi_ssid:");
    for(int i = 0;i<g_system_data.setting_ssid_len;i++){
        printf("%c",g_system_data.setting_ssid[i]);
    }
    printf("\r\nwifi_password:");
    for(int i = 0;i<g_system_data.setting_psw_len;i++){
        printf("%c",g_system_data.setting_psw[i]);
    }
    printf("\r\n");
}

//获取sta模式状态
WIFI_STA_MODE_STATUS_E get_wifi_sta_status(){
    return g_system_data.wifi_sta_status;
}

void set_wifi_sta_status(WIFI_STA_MODE_STATUS_E status){
    g_system_data.wifi_sta_status = status;
}

//获取ap模式状态
WIFI_AP_MODE_STATUS_E get_wifi_ap_status(){
    return g_system_data.wifi_ap_status;
}

void set_wifi_ap_status(WIFI_AP_MODE_STATUS_E status){
    g_system_data.wifi_ap_status = status;
}

void set_is_ap_sta_open(bool isopen){
    g_system_data.is_ap_sta_open = isopen;
}

bool get_is_ap_sta_open(){
    return g_system_data.is_ap_sta_open;
}

void set_has_first_time_httpdata_request(){
    g_system_data.first_time_httpdata_request = false;
}

bool has_first_time_httpdata_request(){
    return g_system_data.first_time_httpdata_request;
}

void set_update_httpdata_request(bool update){
    g_system_data.update_httpdata_request = update;
}

bool has_update_httpdata_request(){
    return g_system_data.update_httpdata_request;
}

//设置城市
void set_system_data_city_info(char * p_city){
    int p_citylen = strlen(p_city);
    g_system_data.setting_city_len = p_citylen;
    memcpy(g_system_data.setting_city,p_city,p_citylen);
    g_system_data.setting_city[p_citylen] = '\0';
    printf("setting weather city = %s\n",g_system_data.setting_city);
    ds_nvs_save_city(p_city);
}

static uint8_t check_chartime_to_uint(char *time,int len){
    uint8_t tens;  //十位
    uint8_t units; //个位
    if(len == 2){
        tens = time[0] - '0';
        units = time[1] - '0';
    }else if(len == 1){
        tens = 0;
        units = time[0] - '0';
    }else{
        tens = 0;
        units = 0;
    }
    return tens*10+units;
}

void set_system_data_tomato_info(char * work_time,char * rest_time,char * count_time){

    int work_len = strlen(work_time);
    int rest_len = strlen(rest_time);
    int count_len = strlen(count_time);
    if(work_len > 2 || rest_len > 2|| count_len > 2){
        ESP_LOGE(TAG, "TOAMTO_TIME_SET_DATA_LEN ERROR");
        return;
    }   
    g_system_data.tomato_work_time = check_chartime_to_uint(work_time,work_len);
    g_system_data.tomato_rest_time = check_chartime_to_uint(rest_time,rest_len);
    g_system_data.tomato_time_count = check_chartime_to_uint(count_time,count_len);
    ds_ui_tomatopage_time_set(g_system_data.tomato_work_time ,g_system_data.tomato_rest_time,g_system_data.tomato_time_count);
    printf("\r\n set_system_data_tomato_info %d %d :%d\n",g_system_data.tomato_work_time,g_system_data.tomato_rest_time,g_system_data.tomato_time_count);                 
}

void reset_tp_action_manage(){
    memset(&g_system_data.tp_action_manage,0,sizeof(TP_ACTION_MANAGE_T));
    //set check on
    g_system_data.tp_action_manage.status = 1;
}

// 10ms once count
void count_tp_action_manage_time(){
    g_system_data.tp_action_manage.tp_interval_timecount ++;
    // 100s
    if(g_system_data.tp_action_manage.tp_interval_timecount > 10000){
        g_system_data.tp_action_manage.tp_interval_timecount = 10000;
    }
}

void set_tp_action_manage_start_point(uint16_t x,uint16_t y){
    g_system_data.tp_action_manage.status ++;
    if(g_system_data.tp_action_manage.status >= 10){
        g_system_data.tp_action_manage.status = 10;
    }
    g_system_data.tp_action_manage.tp_start_x = x;
    g_system_data.tp_action_manage.tp_start_y = y;
}

void set_tp_action_manage_stop_point(uint16_t x,uint16_t y){
    g_system_data.tp_action_manage.tp_stop_x = x;
    g_system_data.tp_action_manage.tp_stop_y = y;
}

void check_tp_action(){
    g_system_data.tp_action_manage.status = 0;
    TP_ACTION_MANAGE_T *m_manage = &g_system_data.tp_action_manage;
    // if(abs(m_action_manage->tp_start_x - m_action_manage->tp_stop_x) > ){

    // }
    // ESP_LOGI(TAG,"timecount %d",m_manage->tp_interval_timecount);
    ESP_LOGI(TAG, "start %d %d stop %d %d",m_manage->tp_start_x,m_manage->tp_start_y,m_manage->tp_stop_x,m_manage->tp_stop_y);
    if(m_manage->tp_stop_x == 0 && m_manage->tp_stop_y == 0 && m_manage->tp_start_x !=0 && m_manage->tp_start_y !=0){
        ESP_LOGI(TAG,"action TP_ACTION_SHORT");
        g_system_data.tp_action_manage.tp_action = TP_ACTION_SHORT;
        ds_ui_page_manage_send_event(g_system_data.tp_action_manage.tp_action,g_system_data.tp_action_manage.tp_start_x,g_system_data.tp_action_manage.tp_start_y);
        return;
    }
    if(abs(m_manage->tp_start_x - m_manage->tp_stop_x) > abs(m_manage->tp_start_y - m_manage->tp_stop_y)){
        //左右滑动
        if(abs(m_manage->tp_start_x - m_manage->tp_stop_x) > 10){
            if(m_manage->tp_start_x > m_manage->tp_stop_x){
                ESP_LOGI(TAG,"action TP_ACTION_MOVE_LEFT");
                g_system_data.tp_action_manage.tp_action = TP_ACTION_MOVE_LEFT;
            }else{
                ESP_LOGI(TAG,"action TP_ACTION_MOVE_RIGHT");
                g_system_data.tp_action_manage.tp_action = TP_ACTION_MOVE_RIGHT;
            }
            ds_ui_page_manage_send_event(g_system_data.tp_action_manage.tp_action,g_system_data.tp_action_manage.tp_start_x,g_system_data.tp_action_manage.tp_start_y);
            return;
        }
    }else{
        //上下滑动
        if(abs(m_manage->tp_start_y - m_manage->tp_stop_y) > 10){
            if(m_manage->tp_start_y > m_manage->tp_stop_y){
                ESP_LOGI(TAG,"action TP_ACTION_MOVE_UP");
                g_system_data.tp_action_manage.tp_action = TP_ACTION_MOVE_UP;
            }else{
                ESP_LOGI(TAG,"action TP_ACTION_MOVE_DOWN");
                g_system_data.tp_action_manage.tp_action = TP_ACTION_MOVE_DOWN;
            }
            ds_ui_page_manage_send_event(g_system_data.tp_action_manage.tp_action,g_system_data.tp_action_manage.tp_start_x,g_system_data.tp_action_manage.tp_start_y);
            return;
        }
    }

    //300ms
    if(m_manage->tp_interval_timecount < 30){
        if((m_manage->tp_stop_x + m_manage->tp_stop_y + m_manage->tp_start_x + m_manage->tp_start_y) == 0)
            return;
        ESP_LOGI(TAG,"action TP_ACTION_SHORT");
        g_system_data.tp_action_manage.tp_action = TP_ACTION_SHORT;
        ds_ui_page_manage_send_event(g_system_data.tp_action_manage.tp_action,g_system_data.tp_action_manage.tp_start_x,g_system_data.tp_action_manage.tp_start_y);
        return;
    }
    //1.5s
    if(m_manage->tp_interval_timecount > 150){
        if(m_manage->tp_start_x != 0 && m_manage->tp_start_y != 0 &&m_manage->tp_stop_x != 0 &&m_manage->tp_stop_y != 0){
            return;
        }
        ESP_LOGI(TAG,"action TP_ACTION_LONG");
        g_system_data.tp_action_manage.tp_action = TP_ACTION_LONG;
        ds_ui_page_manage_send_event(g_system_data.tp_action_manage.tp_action,g_system_data.tp_action_manage.tp_start_x,g_system_data.tp_action_manage.tp_start_y);
        return;
    }
}

uint8_t get_tp_action_status(){
    return g_system_data.tp_action_manage.status;
}

TP_ACTION_E get_tp_action(){
    return g_system_data.tp_action_manage.tp_action;
}

void clear_tp_action(){
    g_system_data.tp_action_manage.tp_action = TP_ACTION_NULL;
}

void set_tp_wackup_timeleft(uint32_t timeleft){
    //600/1s 10min
    g_system_data.tp_wackup_timeleft = timeleft;
}

void count_tp_wackup_timeleft(){
    if(g_system_data.tp_wackup_timeleft > 0){
        g_system_data.tp_wackup_timeleft --;
    }
}

uint32_t get_tp_wackup_timeleft(){
    return g_system_data.tp_wackup_timeleft;
}

void update_system_time(uint8_t hour,uint8_t minute,uint8_t second){
    ESP_LOGI(TAG,"update system time %d:%d:%d\n",hour,minute,second);
    g_system_data.hour = hour;
    g_system_data.minute = minute;
    g_system_data.second = second;
}

void update_system_time_second(){
    if(g_system_data.second >= 59){
        g_system_data.second = 0;
        if(g_system_data.minute >= 59){
            g_system_data.minute = 0;
            if(g_system_data.hour >= 23){
                g_system_data.hour = 0;
            }else
                g_system_data.hour ++;
        }else
            g_system_data.minute ++;
    }else{
        g_system_data.second ++;
    }
}

void update_system_time_minute(){
    if(g_system_data.minute >= 59){
        g_system_data.minute = 0;
        if(g_system_data.hour >= 23){
            g_system_data.hour = 0;
        }else
            g_system_data.hour ++;
    }else
        g_system_data.minute ++;
}