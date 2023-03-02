#ifndef _DS_NVS_H_
#define _DS_NVS_H_

typedef enum{
    NVS_WIFI_INFO_ERROR =0,
    NVS_WIFI_INFO_NULL,
    NVS_WIFI_INFO_SAVE,
}NVS_WIFI_INFO_E;

void ds_nvs_init(void);

void ds_nvs_save_wifi_info(void);

NVS_WIFI_INFO_E ds_nvs_read_wifi_info(void);


#endif

