
#include <string.h>
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "ds_screen.h"
#include "ds_ui_mainpage.h"
#include "ds_system_data.h"
#include "ds_spi.h"

#include "ds_data_page.h"
#include "ds_wifi_ap_sta.h"

void ds_ui_back_main_page(void){
    unsigned int i;
	for(i=0;i<5000;i++){
		spi_send_data(gImage_main_page[i]);  
	} 
}

void ds_screen_display_main(){
	if(get_wifi_sta_status() == WIFI_STA_MODE_CONNECT_SUCCESS){
		ds_screen_full_display_bydata(ds_screen_full_display_data,gImage_main_page_wifi);
	}else
		ds_screen_full_display_bydata(ds_screen_full_display_data,gImage_main_page);
}  

void ds_screen_setting(){
	ds_screen_full_display_bydata(ds_screen_full_display_data,gImage_setting_page);
	
}

void ds_screen_cxk(){
	ds_screen_full_display_bydata(ds_screen_full_display_data,gImage_cxk_page);
	
}



