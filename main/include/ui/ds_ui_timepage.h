

#ifndef _DS_UI_TIMEPAGE_H_
#define _DS_UI_TIMEPAGE_H_

#include <string.h>
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "freertos/queue.h"
#include "esp_types.h"
#include "driver/periph_ctrl.h"
#include "esp_log.h"


void ds_ui_timepage_updatetime(void);
void ds_ui_timepage_show_time(uint8_t type);
void ds_ui_timepage_show_time_init();
void ds_ui_timepage_init(void);

#endif
