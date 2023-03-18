#include <string.h>
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "ds_screen.h"
#include "ds_spi.h"
#include "ds_ui_page_manage.h"
#include "ds_system_data.h"
#include "ds_data_num.h"
#include "ds_data_icon.h"

#include "ds_paint.h"
#include "ds_data_icon.h"
#include "ds_ui_mainpage.h"


typedef struct 
{
    int fans;
    int aqifans;
    uint8_t updateing;
    uint8_t ten;
    uint8_t utils;
}FANS_PAGE_T;

FANS_PAGE_T g_fans_page;


void int_to_char(char *result,int val){
    int depth=1;
    int len = 1;
    int temp=val;
    while(temp>=10){
        temp=temp/10;
        depth = depth * 10;
        len ++;
    }
    for(int index = 0;index < len;index ++){
        temp = val/depth;
        result[index] = temp + '0';
        val = val - temp * depth;
        depth = depth / 10;
    }
    result[len] = '\0';
}

int page_count = 0;

void ds_ui_fans_show_init(uint8_t type){
    g_fans_page.updateing = 1;

    uint8_t *m_custom_image;
    if ((m_custom_image = (uint8_t *)malloc(IMAGE_SIZE)) == NULL) {
      printf("Failed to apply for black memory...\r\n");
      return;
    }
    Paint_NewImage(m_custom_image, EPD_2IN9BC_WIDTH, EPD_2IN9BC_HEIGHT, 0, WHITE);
    Paint_SelectImage(m_custom_image);
    Paint_Clear(WHITE);

    Paint_DrawString_piture(0,0,gImage_back,icon_back_len,36, WHITE, BLACK);

    if(page_count == 0){
        page_count = 1;
        Paint_DrawString_CN(24, 70,"The_Itach1", WHITE, BLACK);
        char result[20];
        int_to_char(result,g_fans_page.fans);
        Paint_DrawString_piture(20,90,gImage_fans,icon_fans_len,32, WHITE, BLACK);
        Paint_DrawString_CN(60, 120, result, WHITE, BLACK);
    }else{
        page_count = 0;
        Paint_DrawString_CN(60, 110,"道之若极,", WHITE, BLACK);
        char result[20];
        // int_to_char(result,g_fans_page.aqifans);
        // Paint_DrawString_piture(20,90,gImage_fans,icon_fans_len,32, WHITE, BLACK);
        Paint_DrawString_CN(60, 135, "行必有格.", WHITE, BLACK);
    }

    ds_screen_full_display(ds_paint_image_new);
    // if(type == 0)
    //     ds_screen_partial_display(0,0,ds_paint_image_new,199,199); 
    //     // ds_screen_partial_display(0,199,0,199 ,ds_ui_back_main_page,ds_paint_image_new); 
    // else
    //     ds_screen_partial_display(0,0,ds_paint_image_new,199,199); 
    //     // ds_screen_partial_display(0,199,0,199 ,ds_paint_last_page,ds_paint_image_new); 

    // ds_screen_full_display(ds_paint_image);
    ds_paint_image_copy();
    free(m_custom_image);

    g_fans_page.updateing = 0;
}

void ds_ui_fans_set_fans_num(int fans){
    g_fans_page.fans = fans;
}

void ds_ui_fans_set_aqifans_num(int fans){
    g_fans_page.aqifans = fans;
}

void ds_ui_fans_init(){
    memset(&g_fans_page,0,sizeof(FANS_PAGE_T));
    g_fans_page.updateing = 0;
    g_fans_page.fans = 100000;
    g_fans_page.aqifans = 100000;

    char result[20];
    int_to_char(result,12345);
}