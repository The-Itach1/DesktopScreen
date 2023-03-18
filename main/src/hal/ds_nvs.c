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

void ds_nvs_save_wifi_info(char * p_ssid,char *p_psw){
    set_system_data_wifi_info(p_ssid,p_psw);
    esp_err_t err;
    nvs_handle_t nvs_handle;
    err = nvs_open("wificonfig", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGI(TAG,"Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return ;
    }
    wifi_config_flag = NVS_WIFI_INFO_HAS_SAVE;
    ESP_ERROR_CHECK(nvs_set_u8(nvs_handle, "wifi_flag", (uint8_t)wifi_config_flag));
    ESP_ERROR_CHECK(nvs_set_str(nvs_handle, "ssid", get_system_data().setting_ssid));
    ESP_ERROR_CHECK(nvs_set_str(nvs_handle, "psw",  get_system_data().setting_psw));
    ESP_ERROR_CHECK(nvs_commit(nvs_handle));
    nvs_close(nvs_handle);
}

NVS_WIFI_INFO_E ds_nvs_read_wifi_info(){
    esp_err_t err;
    nvs_handle_t nvs_handle;
    err = nvs_open("wificonfig", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGI(TAG,"Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return NVS_WIFI_INFO_ERROR;
    }
    uint8_t flag;
    err = nvs_get_u8(nvs_handle, "wifi_flag", &flag);
    if (err != ESP_OK) {
        ESP_LOGI(TAG,"WiFi info key null");
        nvs_close(nvs_handle);
        return NVS_WIFI_INFO_NULL;
    }
    wifi_config_flag = flag;
    if(wifi_config_flag == NVS_WIFI_INFO_HAS_SAVE){
        ESP_LOGI(TAG,"has WiFi config info");
        char ssid[32];
        char psw[64];
        size_t ssid_len = sizeof(ssid);
        size_t psw_len = sizeof(psw);
        ESP_ERROR_CHECK(nvs_get_str(nvs_handle, "ssid", ssid, &ssid_len));
        ESP_ERROR_CHECK(nvs_get_str(nvs_handle, "psw", psw, &psw_len));
        set_system_data_wifi_info(ssid,psw);
        print_system_data_wifi_info();
    }else{
        ESP_LOGI(TAG,"WiFi info null");
    }
    nvs_close(nvs_handle);
    return wifi_config_flag;
}

void ds_nvs_clean_wifi_info(){
    esp_err_t err;
    nvs_handle_t nvs_handle;
    err = nvs_open("wificonfig", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGI(TAG,"Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return ;
    }
    wifi_config_flag = NVS_WIFI_INFO_NULL;
    ESP_ERROR_CHECK(nvs_set_u8(nvs_handle, "wifi_flag", (uint8_t)wifi_config_flag));
    ESP_ERROR_CHECK(nvs_commit(nvs_handle));
    nvs_close(nvs_handle);
}

void ds_nvs_save_city(char * city){
    esp_err_t err;
    nvs_handle_t nvs_handle;
    err = nvs_open("cityconfig", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGI(TAG,"Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return ;
    }
    ESP_ERROR_CHECK(nvs_set_str(nvs_handle, "city", city));
    ESP_ERROR_CHECK(nvs_commit(nvs_handle));
    nvs_close(nvs_handle);
}

char * ds_nvs_read_city(){
    esp_err_t err;
    nvs_handle_t nvs_handle;
    err = nvs_open("cityconfig", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGI(TAG,"Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return NULL;
    }
    size_t city_len;
    if ( (err = nvs_get_str(nvs_handle,  "city", NULL, &city_len)) == ESP_OK) {
        char *city = (char *)malloc(city_len);
        if ( (err = nvs_get_str(nvs_handle, "city", city, &city_len)) == ESP_OK) {
            printf("city = %s\n", city);
            nvs_close(nvs_handle);
            return city;
        }
    }
    nvs_close(nvs_handle);
    return NULL;
}

void ds_nvs_save_tomato_time(uint8_t worktime,uint8_t resettime,uint8_t roundtimes){
    esp_err_t err;
    nvs_handle_t nvs_handle;
    err = nvs_open("tomatoconfig", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGI(TAG,"Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return ;
    }
    ESP_ERROR_CHECK(nvs_set_u8(nvs_handle, "worktime", worktime));
    ESP_ERROR_CHECK(nvs_set_u8(nvs_handle, "resettime", resettime));
    ESP_ERROR_CHECK(nvs_set_u8(nvs_handle, "roundtimes", roundtimes));
    ESP_ERROR_CHECK(nvs_commit(nvs_handle));
    nvs_close(nvs_handle);
}

void ds_nvs_read_tomato_time(uint8_t *worktime,uint8_t *resettime,uint8_t *roundtimes){
    esp_err_t err;
    nvs_handle_t nvs_handle;
    *worktime = 20;
    *resettime = 5;
    *roundtimes = 3;
    err = nvs_open("tomatoconfig", NVS_READWRITE, &nvs_handle);
    if (err != ESP_OK) {
        ESP_LOGI(TAG,"Error (%s) opening NVS handle!\n", esp_err_to_name(err));
        return;
    }
    err = nvs_get_u8(nvs_handle, "worktime", worktime);
    err = nvs_get_u8(nvs_handle, "resettime", resettime);
    err = nvs_get_u8(nvs_handle, "roundtimes", roundtimes);
    printf("tomato time = %d %d %d\n",*worktime,*resettime,*roundtimes);
    nvs_close(nvs_handle);
    return;
}

void ds_nvs_test_func(){
    uint8_t work,rest,times;
    ds_nvs_read_tomato_time(&work,&rest,&times);
    printf("%d %d %d\n",work,rest,times);
    work = 30;
    rest = 10;
    times = 5;
    ds_nvs_save_tomato_time(work,rest,times);
    ds_nvs_read_tomato_time(&work,&rest,&times);
    printf("%d %d %d\n",work,rest,times);
}