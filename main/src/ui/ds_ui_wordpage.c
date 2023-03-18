

#include <string.h>
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "ds_screen.h"
#include "ds_spi.h"
#include "ds_ui_page_manage.h"
#include "ds_data_icon.h"
#include "ds_ui_wordpage.h"
#include "ds_system_data.h"
#include "ds_data_page.h"

#include "ds_paint.h"
#include "ds_data_icon.h"
#include "ds_ui_mainpage.h"

typedef struct 
{
    uint8_t init_status;
    uint8_t updateing;
    uint8_t num;
}WORD_PAGE_T;

WORD_PAGE_T g_word_page;


typedef struct
{
    char word[2][10];
}WORD_T;

typedef struct
{
    WORD_T array[10];
}WORD_ARRAY_T;

//200*200 像素屏幕适配，偏移值
static int offset_v = 20; 
static int offset_h = 20; 

WORD_ARRAY_T word_array;



void ds_ui_wordpage_show(uint8_t type){
    g_word_page.updateing = 1;

    uint8_t *m_custom_image;
    printf("IMAGE_SIZE %d\n",IMAGE_SIZE);
    if ((m_custom_image = (uint8_t *)malloc(IMAGE_SIZE)) == NULL) {
      printf("Failed to apply for black memory...\r\n");
      return ;
    }
    Paint_NewImage(m_custom_image, EPD_2IN9BC_WIDTH, EPD_2IN9BC_HEIGHT, 0, WHITE);
    Paint_SelectImage(m_custom_image);
    Paint_Clear(WHITE);

    Paint_DrawString_piture(0,0,gImage_back,icon_back_len,36, WHITE, BLACK);

    int len_en = strlen( word_array.array[g_word_page.num].word[0]);
    int len_cn = strlen( word_array.array[g_word_page.num].word[1]);

    int start_en = (150 - (len_en*10))/2;
    int start_cn = (150 - (len_cn/3*20))/2;
    if(g_word_page.num  == 0){
        //此处偏移
        start_en -= 15;
    }
    Paint_DrawString_CN(start_en+offset_h, 80+offset_v, word_array.array[g_word_page.num].word[0], WHITE, BLACK);
    Paint_DrawString_CN(start_cn+offset_h, 110+offset_v, word_array.array[g_word_page.num].word[1], WHITE, BLACK);

    ds_screen_full_display(ds_paint_image_new);

    // if(type == 0)
    //     ds_screen_partial_display(0,0,ds_paint_image_new,199,199); 
    //     // ds_screen_partial_display(0,199,0,199 ,ds_ui_back_main_page,ds_paint_image_new); 
    // else
    //     ds_screen_partial_display(0,0,ds_paint_image_new,199,199); 
    //     // ds_screen_partial_display(0,199,0,199 ,ds_paint_last_page,ds_paint_image_new); 

    ds_paint_image_copy();
    free(m_custom_image);

    g_word_page.num ++;
    if(g_word_page.num > 3){
        g_word_page.num = 0;
    }
    g_word_page.updateing = 0;

}


void ds_ui_wordpage_init(){
    memset(&g_word_page,0,sizeof(WORD_PAGE_T));
    g_word_page.updateing = 0;

    strcpy(word_array.array[0].word[0],"ikun");
    strcpy(word_array.array[0].word[1],"小黑子");

    strcpy(word_array.array[1].word[0],"embeded");
    strcpy(word_array.array[1].word[1],"嵌入式");

    strcpy(word_array.array[2].word[0],"likeit");
    strcpy(word_array.array[2].word[1],"点赞");


}