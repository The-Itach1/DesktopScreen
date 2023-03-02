#ifndef _DS_SPIFFS_H_
#define _DS_SPIFFS_H_


#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_spiffs.h"


esp_err_t init_spiffs(void);
void ds_spiffs_deinit(void);
void ds_spiffs_test(void);


#endif

