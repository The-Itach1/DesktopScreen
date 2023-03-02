/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_log.h"

#include "ds_timer.h"
#include "ds_spiffs.h"
#include "ds_system_data.h"
#include "ds_nvs.h"

static const char *TAG = "MAIN APP";

// 要创建的任务
void vTaskCode( void * pvParameters )
{
    for( ;; )
    { 
        printf("Task run ...\n");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

void app_main(void)
{
    printf("Hello world!\n");

    //打印几条日志
    ESP_LOGI(TAG, "this is info log");
    ESP_LOGE(TAG, "this is error log");
    static uint8_t ucParameterToPass;
    TaskHandle_t xHandle = NULL;
    // xTaskCreate(vTaskCode, 
    //                  "Task", 
    //                  2048, 
    //                  &ucParameterToPass, 
    //                  10, 
    //                  &xHandle );


    //ds_timer_init();

    //初始化和挂载 SPIFFS 文件系统。
    init_spiffs();
    //创建文件，写入文件，修改文件名称，关闭文件
    ds_spiffs_test();
    //卸载分区并禁用 SPIFFS
    ds_spiffs_deinit();


    char *ssid="Itach1";
    char *psw="123456789";
    //初始化wifi结构体数据
    set_system_data_wifi_info(ssid,strlen(ssid),psw,strlen(psw));
    //nvs初始化
    ds_nvs_init();
    //将wifi信息保存到nvs
    ds_nvs_save_wifi_info();
    //从nvs读取wifi信息。
    ds_nvs_read_wifi_info();

    /*打印芯片信息*/
    /* Print chip information */
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    printf("This is %s chip with %d CPU core(s), WiFi%s%s, ",
            CONFIG_IDF_TARGET,
            chip_info.cores,
            (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
            (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

    printf("silicon revision %d, ", chip_info.revision);

    printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
            (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

    printf("Minimum free heap size: %d bytes\n", esp_get_minimum_free_heap_size());


    // while(1){
    //     printf("system run ...\n");
    //     vTaskDelay(1000 / portTICK_PERIOD_MS);
    // }

}
