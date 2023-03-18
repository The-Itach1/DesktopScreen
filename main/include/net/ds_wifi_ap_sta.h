#ifndef _DS_WIFI_AP_STA_H_
#define _DS_WIFI_AP_STA_H_

typedef enum{
    AP_STA_START = 0,
    AP_STA_UPDATE,
    AP_STA_STOP,
}WIFI_SET_EVENT_E;

void ds_wifi_send_event(WIFI_SET_EVENT_E event);
void ds_wifi_ap_sta_init(void);

#endif

