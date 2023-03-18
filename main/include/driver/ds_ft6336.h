#ifndef _DS_FT6336_H_
#define _DS_FT6336_H_

#include <stdio.h>
#include <stdbool.h>

#define GPIO_RST_LOW 0   //复位引脚低电平
#define GPIO_RST_HIGHT 1 //复位引脚高电平

#define TP_PRES_DOWN 0x80  //触屏被按下	
#define TP_COORD_UD  0x40  //触摸坐标更新标记

//FT6236 部分寄存器定义 
#define FT_DEVIDE_MODE 			0x00   		//FT6236模式控制寄存器
#define FT_REG_NUM_FINGER       0x02		//触摸状态寄存器

#define FT_TP1_REG 				0X03	  	//第一个触摸点数据地址
#define FT_TP2_REG 				0X09		//第二个触摸点数据地址
#define FT_TP3_REG 				0X0F		//第三个触摸点数据地址
#define FT_TP4_REG 				0X15		//第四个触摸点数据地址
#define FT_TP5_REG 				0X1B		//第五个触摸点数据地址  

#define	FT_ID_G_LIB_VERSION		0xA1		//版本		
#define FT_ID_G_MODE 			0xA4   		//FT6236中断模式控制寄存器
#define FT_ID_G_THGROUP			0x80   		//触摸有效值设置寄存器
#define FT_ID_G_PERIODACTIVE	0x88   		//激活状态周期设置寄存器  
#define Chip_Vendor_ID          0xA3        //芯片ID(0x36)
#define ID_G_FT6236ID			0xA8		//0x11

typedef struct
{
	uint8_t status;
	uint16_t x;
	uint16_t y;
}TP_POSITION_T;

//触摸点相关数据结构体定义
typedef struct			
{
	//bit7:按下1/松开0; 
	//bit6:0没有按键按下/1有按键按下;
	//bit5:保留；
	//bit4-bit0触摸点按下有效标志，有效为1，分别对应触摸点5-1；
	uint8_t touch_sta;	 //触摸情况，
	uint8_t touch_count; //触摸点数
	uint16_t x[5];		 //最多支持5点触摸，需要使用5组坐标存储触摸点数据
	uint16_t y[5];
	bool update;
}TouchPoint_T;

void get_ft6336_touch_sta(TP_POSITION_T *position);
void init_ft6336(void);

#endif

