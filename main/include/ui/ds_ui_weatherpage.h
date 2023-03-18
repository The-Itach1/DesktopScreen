

#ifndef _DS_UI_WEATHERPAGE_H_
#define _DS_UI_WEATHERPAGE_H_

#include <string.h>
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "freertos/queue.h"
#include "esp_types.h"
#include "driver/periph_ctrl.h"
#include "esp_log.h"

typedef enum{
    VALUE_CITY = 0,
    VALUE_WEATHER,
    VALUE_CODE,
    VALUE_TEMP,
}VALUE_TYPE_T;

void ds_ui_weather_set(VALUE_TYPE_T type,char *value);
void ds_ui_weather_init();
void ds_ui_weather_show(uint8_t type);

#endif
