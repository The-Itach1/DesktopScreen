#ifndef _DS_SYSTEM_DATA_H_
#define _DS_SYSTEM_DATA_H_

#include <stdio.h>
#include <string.h>

#define MAX_SETTING_SSID_LEN 32
#define MAX_SETTING_PSW_LEN 64

typedef enum{
    SETTING_DATA_INIT = 0,
    SETTING_DATA_HAS_WIFI_INFO,
}SETTING_DATA_E;

typedef struct
{
    char setting_ssid[MAX_SETTING_SSID_LEN];
    uint8_t setting_ssid_len;
    char setting_psw[MAX_SETTING_PSW_LEN];
    uint8_t setting_psw_len;
}SYSTRM_DATA_T;

//////////////////WIFI INFO MANAGE //////////////////////////////

void set_system_data_wifi_info(char * p_ssid,uint8_t p_ssidlen,char *p_psw ,uint8_t p_pswlen);

void print_system_data_wifi_info(void);

SETTING_DATA_E check_system_data_wifi_info(void);

SYSTRM_DATA_T get_system_data(void);

void ds_system_data_init(void);

#endif

