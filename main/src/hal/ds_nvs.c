/* Non-Volatile Storage (NVS) Read and Write a Value - Example

   For other examples please check:
   https://github.com/espressif/esp-idf/tree/master/examples

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_log.h"

#include "ds_nvs.h"
#include "ds_system_data.h"

static const char *TAG = "ds_nvs";

NVS_WIFI_INFO_E wifi_config_flag = NVS_WIFI_INFO_NULL;

//设置wifi信息。
void ds_nvs_save_wifi_info(){
    esp_err_t err;
    nvs_handle_t nvs_handle;
    err = nvs_open("wificonfig", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGI(TAG,"Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return ;
    }
    wifi_config_flag = NVS_WIFI_INFO_SAVE;
    ESP_ERROR_CHECK(nvs_set_u8(nvs_handle, "wifi_flag", wifi_config_flag));
    ESP_ERROR_CHECK(nvs_set_str(nvs_handle, "ssid", get_system_data().setting_ssid));
    ESP_ERROR_CHECK(nvs_set_str(nvs_handle, "psw",  get_system_data().setting_psw));
    ESP_ERROR_CHECK(nvs_commit(nvs_handle));
    nvs_close(nvs_handle);
}

//从nvs读取wifi信息。
NVS_WIFI_INFO_E ds_nvs_read_wifi_info(){
    esp_err_t err;
    nvs_handle_t nvs_handle;
    err = nvs_open("wificonfig", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGI(TAG,"Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return NVS_WIFI_INFO_ERROR;
    }
    uint8_t wifi_config = 0;
    ESP_ERROR_CHECK(nvs_get_u8(nvs_handle, "wifi_flag", &wifi_config));
    wifi_config_flag = wifi_config;
    if(wifi_config_flag == NVS_WIFI_INFO_SAVE){
        ESP_LOGI(TAG,"has wifi config info");
        char ssid[32];
        char psw[64];
        size_t ssid_len = sizeof(ssid);
        size_t psw_len = sizeof(psw);
        ESP_ERROR_CHECK(nvs_get_str(nvs_handle, "ssid", ssid, &ssid_len));
        ESP_ERROR_CHECK(nvs_get_str(nvs_handle, "psw", psw, &psw_len));
        set_system_data_wifi_info(ssid,ssid_len,psw,psw_len);
        print_system_data_wifi_info();
    }else{
        ESP_LOGI(TAG,"wifi config info null");
    }
    nvs_close(nvs_handle);
    return wifi_config_flag;
}
//nvs初始化
void ds_nvs_init(){
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    ESP_ERROR_CHECK( err );
}