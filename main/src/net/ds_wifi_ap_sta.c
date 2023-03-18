#include "freertos/FreeRTOS.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "driver/gpio.h"

#include <string.h>
#include "ds_wifi_ap_sta.h"
#include "ds_system_data.h"
#include "ds_http_server.h"
#include "ds_dns_server.h"
#include "ds_pwm.h"
#include "ds_ui_page_manage.h"

#define TAG  "app_sta_ap"

esp_netif_t *sta;
esp_netif_t *ap;

xQueueHandle wifi_event_queue;

static void wifi_event_handler(void* arg, esp_event_base_t event_base,
                                    int32_t event_id, void* event_data)
{
    static int s_retry_num = 0;
    ESP_LOGI(TAG, "event_base = %s event_id = %d",event_base,event_id);
    //STA event
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        esp_wifi_connect();
        set_wifi_sta_status(WIFI_STA_MODE_INIT);
        ESP_LOGI(TAG, "retry to connect to the AP");
        // s_retry_num++;
        // if(s_retry_num >= 200){
        //     set_wifi_sta_status(WIFI_STA_MODE_CONNECT_TIMEOUT);
        //     s_retry_num = 0;
        //     ESP_LOGI(TAG, "connect to the AP timeout");
        // }else{
        //     esp_wifi_connect();
        //     set_wifi_sta_status(WIFI_STA_MODE_INIT);
        //     ESP_LOGI(TAG, "retry to connect to the AP");
        // }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "connect success ! got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        send_beep_event(BEEP_SHORT_500MS);
        ds_ui_page_manage_send_action(PAGE_TYPE_MEMU);
        set_wifi_sta_status(WIFI_STA_MODE_CONNECT_SUCCESS);
    }

    //AP event
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STACONNECTED) {
        wifi_event_ap_staconnected_t* event = (wifi_event_ap_staconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" join, AID=%d", MAC2STR(event->mac), event->aid);
        set_wifi_ap_status(WIFI_AP_MODE_CONNECT);
        send_beep_event(BEEP_SHORT_500MS);
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_AP_STADISCONNECTED) {
        wifi_event_ap_stadisconnected_t* event = (wifi_event_ap_stadisconnected_t*) event_data;
        ESP_LOGI(TAG, "station "MACSTR" leave, AID=%d", MAC2STR(event->mac), event->aid);
        set_wifi_ap_status(WIFI_AP_MODE_DISCONNECT);
    }
}

static void ds_wifi_ap_sta_start(void)
{
    sta = esp_netif_create_default_wifi_sta();
    ap = esp_netif_create_default_wifi_ap();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL));
    ESP_ERROR_CHECK(esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL));
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_APSTA) );
    wifi_config_t sta_config = {
        .sta = {
            .ssid = CONFIG_ESP_WIFI_SSID,
            .password = CONFIG_ESP_WIFI_PASSWORD,
            .bssid_set = false,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .capable = true,
                .required = false
            },
        }
    };

    if(has_wifi_info()){
        memcpy(sta_config.sta.ssid,get_system_data().setting_ssid,32);
        memcpy(sta_config.sta.password,get_system_data().setting_psw,64);
    }

    wifi_config_t ap_config = {
        .ap = {
            .ssid = CONFIG_ESP_AP_WIFI_SSID,
            .ssid_len = strlen(CONFIG_ESP_AP_WIFI_SSID),
            .channel = 11,
            .password = CONFIG_ESP_AP_WIFI_PASSWORD,
            .max_connection = CONFIG_ESP_AP_MAX_STA_CONN,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

static void ds_wifi_ap_sta_update_info(){
    wifi_config_t sta_config = {
        .sta = {
            .ssid = CONFIG_ESP_WIFI_SSID,
            .password = CONFIG_ESP_WIFI_PASSWORD,
            .bssid_set = false,
            .threshold.authmode = WIFI_AUTH_WPA2_PSK,
            .pmf_cfg = {
                .capable = true,
                .required = false
            },
        }
    };
    if(has_wifi_info()){
        memcpy(sta_config.sta.ssid,get_system_data().setting_ssid,32);
        memcpy(sta_config.sta.password,get_system_data().setting_psw,64);
    }
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &sta_config));
}

static void ds_wifi_ap_sta_stop(void){
    ESP_LOGI(TAG, "ESP_WIFI_MODE_AP STOP");
    ESP_ERROR_CHECK(esp_event_handler_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler));
    ESP_ERROR_CHECK(esp_event_handler_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler));
    esp_netif_destroy_default_wifi(sta);
    esp_netif_destroy_default_wifi(ap);
    ESP_ERROR_CHECK(esp_wifi_stop() );
    ESP_ERROR_CHECK(esp_wifi_deinit() );
}

static void wifi_net_task(void* arg)
{
    //http服务器初始化
    // dns_server_start();
    // web_server_start();
    for(;;) {
        WIFI_SET_EVENT_E evt;
        xQueueReceive(wifi_event_queue, &evt, portMAX_DELAY);
        printf("get wifi set event %d\n",evt);
        switch (evt)
        {
        case AP_STA_START:
            set_is_ap_sta_open(true);
            set_wifi_sta_status(WIFI_STA_MODE_INIT);
            set_wifi_ap_status(WIFI_AP_MODE_DISCONNECT);
            ds_wifi_ap_sta_start();
            break;
        case AP_STA_UPDATE:
            ds_wifi_ap_sta_update_info();
            break;
        case AP_STA_STOP:
            set_is_ap_sta_open(false);
            ds_wifi_ap_sta_stop();
            break;
        default:
            break;
        }
    }
}

void ds_wifi_send_event(WIFI_SET_EVENT_E event){
	WIFI_SET_EVENT_E evt;
    evt = event;
	xQueueSend(wifi_event_queue, &evt, 0);
}

void ds_wifi_ap_sta_init(){
    wifi_event_queue = xQueueCreate(10, sizeof(WIFI_SET_EVENT_E));
    xTaskCreate(wifi_net_task, "wifi_net_task", 4096, NULL, 10, NULL);    
}
