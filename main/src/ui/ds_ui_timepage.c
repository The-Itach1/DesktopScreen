#include <string.h>
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "ds_screen.h"
#include "ds_spi.h"
#include "ds_ui_page_manage.h"
#include "ds_ui_timepage.h"
#include "ds_system_data.h"
#include "ds_data_num.h"
#include "ds_data_icon.h"

typedef struct 
{
    uint8_t init_status;
    uint8_t hour;
    uint8_t minute;
    uint8_t hour_last;
    uint8_t minute_last;
    uint8_t temperature; 
    uint8_t humidity;
    uint8_t updateing;

    //局部刷新次数
    uint8_t partial_update_time;
}TIME_PAGE_T;

//200*200 像素屏幕适配，偏移值
static int offset_v = 29; 
static int offset_h = 24; 

TIME_PAGE_T g_time_page;

static void ds_ui_timepage_num_set(const uint8_t *data){
	unsigned int i;
	for(i=0;i<192;i++){	
		spi_send_data(~data[i]);
	}
}

static void ds_ui_timepage_num_clear(const uint8_t *data)
{
	unsigned int i;
    if(g_time_page.init_status == 0){
        for(i=0;i<192;i++){
            spi_send_data(0xff);  
        }  
    }else{
        for(i=0;i<192;i++)	     {
		    spi_send_data(~data[i]);  
	    }  
    }
}

void ds_ui_timepage_show_time_init(){
    g_time_page.hour = get_system_data().hour;
    g_time_page.minute = get_system_data().minute;
    g_time_page.updateing = 1;
    int num_size = 32;
    int num_size_y = 48;
	int vertical = 35 + offset_v; //垂直位置
    int horizontal = 8 + offset_h;  //水平位置
    int now_index;

    ds_screen_partial_data_init();

    //小时 十位
    now_index = g_time_page.hour / 10;
    ds_screen_partial_data_add(horizontal+0,horizontal+num_size,vertical,vertical+num_size_y,gImage_num_size48[now_index]);
    //小时 个位
    now_index = g_time_page.hour % 10;
    ds_screen_partial_data_add(horizontal+num_size,horizontal+num_size*2,vertical,vertical+num_size_y,gImage_num_size48[now_index]);
    //首次刷新才显示 :号
    ds_screen_partial_data_add(horizontal+num_size*2,horizontal+num_size*2+8,vertical,vertical+num_size_y,gImage_time_symbol);

    //分钟 十位
    now_index = g_time_page.minute / 10;
    ds_screen_partial_data_add(horizontal+num_size*2+8,horizontal+num_size*3+8,vertical,vertical+num_size_y,gImage_num_size48[now_index]);
    // //分钟 个位
    now_index = g_time_page.minute % 10;
    ds_screen_partial_data_add(horizontal+num_size*3+8,horizontal+num_size*4+8,vertical,vertical+num_size_y,gImage_num_size48[now_index]);

    //返回
    //size 36*36
    num_size = 36;
    vertical = 0;
    ds_screen_partial_data_add(0,num_size,vertical,vertical+num_size,gImage_back);

    //温湿度图标
    //size 56*56
    num_size = 56;
    vertical = 85 + offset_v;
    horizontal = 5  + offset_h;
    ds_screen_partial_data_add(horizontal,horizontal+num_size,vertical,vertical+num_size,gImage_TH);

    //温度
    vertical = 95 + offset_v;
    num_size = 16;
    num_size_y = 24;
    horizontal = 70 + offset_h;
    //溫度 十位
    now_index = g_time_page.temperature / 10;
    ds_screen_partial_data_add(horizontal,horizontal+num_size,vertical,vertical+num_size_y,gImage_num_size24[now_index]);
    //溫度 个位
    now_index = g_time_page.temperature % 10;
    ds_screen_partial_data_add(horizontal+num_size,horizontal+2*num_size,vertical,vertical+num_size_y,gImage_num_size24[now_index]);

    //湿度
    vertical = 125 + offset_v;
    num_size = 16;
    horizontal = 70 + offset_h;
    //溫度 十位
    now_index = g_time_page.humidity / 10;
    ds_screen_partial_data_add(horizontal,horizontal+num_size,vertical,vertical+num_size_y,gImage_num_size24[now_index]);
    //溫度 个位
    now_index = g_time_page.humidity % 10;
    ds_screen_partial_data_add(horizontal+num_size,horizontal+2*num_size,vertical,vertical+num_size_y,gImage_num_size24[now_index]);

    //温度符号
    //size 36*36
    num_size = 32;
    vertical = 85 + offset_v;
    horizontal = 105 + offset_h;
    ds_screen_partial_data_add(horizontal,horizontal+num_size,vertical,vertical+num_size,gImage_template);

    //湿度符号
    vertical = 115 + offset_v;
    ds_screen_partial_data_add(horizontal,horizontal+num_size,vertical,vertical+num_size,gImage_humidity);

    ds_screen_partial_data_copy();

    g_time_page.hour_last = g_time_page.hour;
    g_time_page.minute_last = g_time_page.minute;
    g_time_page.init_status = 1;
    g_time_page.updateing = 0;
}


//type = 0-全部刷新 1-部分刷新
void ds_ui_timepage_show_time(uint8_t type){
    g_time_page.updateing = 1;
    g_time_page.init_status = type;
    int num_size = 32;
    int num_size_y = 48;
	int vertical = 35 + offset_v; //垂直位置
    int horizontal = 8 + offset_h;  //水平位置
    int last_index,now_index;

    //小时 十位
    now_index = g_time_page.hour / 10;
    ds_screen_partial_display_bydata(horizontal+0,vertical,ds_ui_timepage_num_set,gImage_num_size48[now_index],num_size_y,num_size);
    // //小时 个位
    now_index = g_time_page.hour % 10;
    ds_screen_partial_display_bydata(horizontal+num_size,vertical,ds_ui_timepage_num_set,gImage_num_size48[now_index],num_size_y,num_size);
    //分钟 十位 10 *8 = 80  14*8 112
    now_index = g_time_page.minute / 10;
    ds_screen_partial_display_bydata(horizontal+num_size*2+8,vertical,ds_ui_timepage_num_set,gImage_num_size48[now_index],num_size_y,num_size);
    // //分钟 个位
    now_index = g_time_page.minute % 10;
    ds_screen_partial_display_bydata(horizontal+num_size*3+8,vertical,ds_ui_timepage_num_set,gImage_num_size48[now_index],num_size_y,num_size);
    refresh_part();
	deep_sleep();
    g_time_page.hour_last = g_time_page.hour;
    g_time_page.minute_last = g_time_page.minute;
    g_time_page.init_status = 1;
    g_time_page.updateing = 0;
}

void ds_ui_timepage_updatetime(){
    g_time_page.hour = get_system_data().hour;
    g_time_page.minute = get_system_data().minute;
    if(g_time_page.updateing == 0){
        if(ds_ui_get_now_show_page() == PAGE_TYPE_TIME){
            if(g_time_page.partial_update_time > 5){
                g_time_page.partial_update_time = 0;
                ds_ui_timepage_show_time_init();
            }else{
                g_time_page.partial_update_time ++;
                ds_ui_timepage_show_time(1);
            }
        }
    }
}

void ds_ui_timepage_init(){
    memset(&g_time_page,0,sizeof(TIME_PAGE_T));
    g_time_page.updateing = 0;
    g_time_page.hour = 1;
    g_time_page.minute = 37;
    g_time_page.temperature = 26;
    g_time_page.humidity = 88;
}