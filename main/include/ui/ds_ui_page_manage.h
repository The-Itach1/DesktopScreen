

#ifndef _DS_UI_PAGE_MANAGE_H_
#define _DS_UI_PAGE_MANAGE_H_

#include "ds_system_data.h"

#define EPD_2IN9BC_WIDTH       200
#define EPD_2IN9BC_HEIGHT      200
#define IMAGE_SIZE (((EPD_2IN9BC_WIDTH % 8 == 0) ? (EPD_2IN9BC_WIDTH / 8 ) : (EPD_2IN9BC_WIDTH / 8 + 1)) * EPD_2IN9BC_HEIGHT)


typedef enum{
	PAGE_TYPE_MEMU = 0,
	PAGE_TYPE_TIME,
	PAGE_TYPE_WEATHER,
	PAGE_TYPE_WORD,
	PAGE_TYPE_TOMATO,
	PAGE_TYPE_FANS,
	PAGE_TYPE_SETTING,
	PAGE_TYPE_CXK,
}PAGE_TYPE_E;

typedef struct
{
    PAGE_TYPE_E now_show_page;
}PAGE_MANAGE_T;

PAGE_TYPE_E ds_ui_get_now_show_page(void);
void ds_ui_page_manage_send_event(TP_ACTION_E key,uint8_t touch_x,uint8_t touch_y);
void ds_ui_page_manage_send_action(PAGE_TYPE_E action);
void ds_ui_page_manage_init(void);


#endif
