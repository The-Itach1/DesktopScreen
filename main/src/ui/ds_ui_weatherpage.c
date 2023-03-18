#include <string.h>
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "ds_screen.h"
#include "ds_spi.h"
#include "ds_ui_page_manage.h"
#include "ds_data_icon.h"
#include "ds_ui_weatherpage.h"
#include "ds_system_data.h"
#include "ds_data_page.h"

#include "ds_data_weather.h"
#include "ds_data_num.h"
#include "ds_paint.h"
#include "ds_ui_mainpage.h"


typedef struct 
{
    uint8_t init_status;
    uint8_t updateing;

    //0-3晴 4-8多云 9-阴 10-13-小雨 14-19暴雨 20-25雪 >26 大风
    uint8_t code;

    char city[20];
    char weather_text[20];
    char weather_code[2];
    char temperature_array[3];

}WEATHER_PAGE_T;

//200*200 像素屏幕适配，偏移值
static int offset_v = 20; 
static int offset_h = 20; 
WEATHER_PAGE_T g_weather_page;


void ds_ui_weather_set(VALUE_TYPE_T type,char *value){
    if(type == VALUE_CITY)
        sprintf(g_weather_page.city,"%s",value);
    else if(type == VALUE_WEATHER)
        sprintf(g_weather_page.weather_text,"%s",value);
    else if(type == VALUE_CODE)
        sprintf(g_weather_page.weather_code,"%s",value);
    else if(type == VALUE_TEMP)
        sprintf(g_weather_page.temperature_array,"%s",value);
    if(strlen(g_weather_page.weather_code) == 2)
        g_weather_page.code = (g_weather_page.weather_code[1] - '0')*10+g_weather_page.weather_code[0] - '0';
    else
        g_weather_page.code = g_weather_page.weather_code[0] - '0';
    // printf("value:%s code:%d",value,g_weather_page.code);
}

void ds_ui_weather_show(uint8_t type){

    g_weather_page.updateing = 1;
	int vertical = 80 +offset_v; //垂直位置
    int horizontal = 30 + offset_h;  //水平位置
    uint8_t *m_custom_image;
    if ((m_custom_image = (uint8_t *)malloc(IMAGE_SIZE)) == NULL) {
      printf("Failed to apply for black memory...\r\n");
      return;
    }
    Paint_NewImage(m_custom_image, EPD_2IN9BC_WIDTH, EPD_2IN9BC_HEIGHT, 0, WHITE);
    Paint_SelectImage(m_custom_image);
    Paint_Clear(WHITE);

    Paint_DrawString_piture(0,0,gImage_back,icon_back_len,36, WHITE, BLACK);

    Paint_DrawString_CN(horizontal, vertical,g_weather_page.city, WHITE, BLACK);
    vertical = 40 +offset_v;
    horizontal = 90 + offset_h;
    switch (g_weather_page.code)
    {
    case 0:
    case 1:
    case 2:
    case 3:
        Paint_DrawString_piture(horizontal,vertical,gImage_num_weather_icon[0],288,48, WHITE, BLACK);
        break;
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
        Paint_DrawString_piture(horizontal,vertical,gImage_num_weather_icon[1],288,48, WHITE, BLACK);
        break;
    case 9:
        Paint_DrawString_piture(horizontal,vertical,gImage_num_weather_icon[2],288,48, WHITE, BLACK);
        break;
    case 10:
    case 11:
    case 12:
    case 13:
        Paint_DrawString_piture(horizontal,vertical,gImage_num_weather_icon[2],288,48, WHITE, BLACK);
        break;
    case 14:
    case 15:
    case 16:
    case 17:
    case 18:
    case 19:
        Paint_DrawString_piture(horizontal,vertical,gImage_num_weather_icon[3],288,48, WHITE, BLACK);
        break;
    case 20:
    case 21:
    case 22:
    case 23:
    case 24:
    case 25:
        Paint_DrawString_piture(horizontal,vertical,gImage_num_weather_icon[4],288,48, WHITE, BLACK);
        break;
    default:
        Paint_DrawString_piture(horizontal,vertical,gImage_num_weather_icon[5],288,48, WHITE, BLACK);
        break;
    }
    vertical = 120 +offset_v;
    horizontal = 50 + offset_h;
    Paint_DrawString_CN(horizontal, vertical,g_weather_page.temperature_array, WHITE, BLACK);
    vertical = 85 +offset_v;
    horizontal = 80 + offset_h;
    Paint_DrawString_piture(horizontal,vertical,gImage_template,icon_template_len,32, WHITE, BLACK);
    vertical = 145 +offset_v;
    horizontal = 35 + offset_h;
    if(strlen(g_weather_page.weather_text)/3 == 4){
        Paint_DrawString_CN(horizontal, vertical,g_weather_page.weather_text, WHITE, BLACK);
    }else{
        Paint_DrawString_CN(horizontal, vertical,g_weather_page.weather_text, WHITE, BLACK);
    }
    ds_screen_full_display(ds_paint_image_new);
    
    // ds_screen_partial_display(0,0,ds_paint_image_new,199,199); 
    // ds_screen_partial_display(0,199,0,199 ,ds_ui_back_main_page,ds_paint_image_new); 

    // ds_screen_full_display(ds_paint_image);

    free(m_custom_image);

    g_weather_page.updateing = 0;
}

void ds_ui_weather_init(){
    memset(&g_weather_page,0,sizeof(WEATHER_PAGE_T));
    g_weather_page.updateing = 0;
    strcpy(g_weather_page.temperature_array,"26");
    strcpy(g_weather_page.weather_code,"0");
    strcpy(g_weather_page.weather_text,"网络异常");
    strcpy(g_weather_page.city,"初始");
}