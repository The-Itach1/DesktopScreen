#ifndef _DS_NVS_H_
#define _DS_NVS_H_

typedef enum{
    NVS_WIFI_INFO_ERROR =0,
    NVS_WIFI_INFO_NULL,
    NVS_WIFI_INFO_HAS_SAVE,
}NVS_WIFI_INFO_E;

///////////////////////////////WIFI///////////////////////
void ds_nvs_save_wifi_info(char * p_ssid,char *p_psw);
NVS_WIFI_INFO_E ds_nvs_read_wifi_info(void);
void ds_nvs_clean_wifi_info(void);

///////////////////////////////CITY////////////////////////
void ds_nvs_save_city(char * city);
char * ds_nvs_read_city();

///////////////////////////////TOMATO//////////////////////
void ds_nvs_save_tomato_time(uint8_t worktime,uint8_t resettime,uint8_t roundtimes);
void ds_nvs_read_tomato_time(uint8_t *worktime,uint8_t *resettime,uint8_t *roundtimes);


#endif

