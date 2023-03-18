
#include <string.h>
#include <stdio.h>

#include "ds_ft6336.h"
#include "ds_i2c.h"
#include "ds_gpio.h"
#include "ds_screen.h"
#include "ds_system_data.h"

//触摸芯片最大5组触摸点，FT6335最大支持双触
const uint16_t FT6236_TPX_TBL[5]=
{
	FT_TP1_REG,
	FT_TP2_REG,
	FT_TP3_REG,
	FT_TP4_REG,
	FT_TP5_REG
};

TouchPoint_T gTPS; 

//扫描触摸屏寄存器状态、数据
static void scan_ft6336()
{
	uint8_t i=0;
	uint8_t sta = 0;
	uint8_t buf[4] = {0};  
	uint8_t gestid=0;
	i2c_master_read_slave(0x02,&sta,1);//读取触摸点的状态  
    gTPS.touch_count=sta;	
	i2c_master_read_slave(0x01,&gestid,1);//读取触摸点的状态	
 	if(sta & 0x0f)	//判断是否有触摸点按下，0x02寄存器的低4位表示有效触点个数
 	{
 		gTPS.touch_sta = ~(0xFF << (sta & 0x0F));	//~(0xFF << (sta & 0x0F))将点的个数转换为触摸点按下有效标志
 		for(i=0;i<2;i++)	                                //分别判断触摸点1-5是否被按下
 		{   
 			if(gTPS.touch_sta & (1<<i))			    //读取触摸点坐标
 			{											    //被按下则读取对应触摸点坐标数据
				i2c_master_read_slave(FT6236_TPX_TBL[i],buf,4);	//读取XY坐标值
				gTPS.x[i]=((uint16_t)(buf[0]&0X0F)<<8)+buf[1];
				gTPS.y[i]=((uint16_t)(buf[2]&0X0F)<<8)+buf[3];
                // printf("%x %x %x %x x=%d y=%d\n",buf[0],buf[1],buf[2],buf[3],gTPS.x[i],gTPS.y[i]);
 				// if((buf[0]&0XC0)!=0X80)
 				// {
				// 	gTPS.x[i]=gTPS.y[i]=0;//必须是contact事件，才认为有效	
				// 	gTPS.touch_sta &=0xe0;	//清除触摸点有效标记
				// 	return;
				// }
 			}
 		}
 		gTPS.touch_sta |= TP_PRES_DOWN;     //触摸按下标记
 	}
 	else
 	{
 		if(gTPS.touch_sta &TP_PRES_DOWN) 	//之前是被按下的
 			gTPS.touch_sta &= ~0x80;        //触摸松开标记	
 		else
 		{
 			gTPS.x[0] = 0;
 			gTPS.y[0] = 0;
 			gTPS.touch_sta &=0xe0;	//清除触摸点有效标记
 		}
 	}
}

//转换为实际位置
static void count_position_ft6336(TP_POSITION_T *position){
	// printf("------count_position_ft6336 %d------\n",gTPS.touch_count);
    switch(gTPS.touch_count)
	{
		case 1:		
        	// printf("x=%d y=%d\n",gTPS.x[0],gTPS.y[0]);
			if((gTPS.x[0]!=0)&&(gTPS.y[0]!=0)
			&&(gTPS.x[0]<200)&&(gTPS.y[0]<200))//软件滤掉无效操作
			{
				//To 152x152
				gTPS.x[0]=gTPS.x[0]*152/200; 
				gTPS.y[0]=gTPS.y[0]*152/200;
				position->status = 1;
				position->x = gTPS.x[0];
				position->y = gTPS.y[0];
				/******调试使用****/		
                // printf("触摸点个数=%d\r\n",gTPS.touch_count);	//FT6336U最多支持两点触控
                // printf("x0:%d,y0:%d\r\n",gTPS.x[0],gTPS.y[0]);
				return;
			}
			break;
	    case 2:
			if((gTPS.x[0]!=0)&&(gTPS.y[0]!=0)
				&&(gTPS.x[1]!=0)&&(gTPS.y[1]!=0)
				&&(gTPS.x[0]<200)&&(gTPS.y[0]<200)
				&&(gTPS.x[1]<200)&&(gTPS.y[1]<200))//软件滤掉无效操作
			{
				//To 152x152
				gTPS.x[0]=gTPS.x[0]*152/200; 
				gTPS.y[0]=gTPS.y[0]*152/200;	
				gTPS.x[1]=gTPS.x[1]*152/200; 
				gTPS.y[1]=gTPS.y[1]*152/200;	
				/******调试使用****/
				// printf("触摸点个数：:%d\r\n",gTPS.touch_count);	//FT6336U最多支持两点触控
				// printf("x0:%d,y0:%d\r\n",gTPS.x[0],gTPS.y[0]);
				// printf("x1:%d,y1:%d\r\n",gTPS.x[1],gTPS.y[1]);
			}
			break;					
		default:
			break;						
	}
    for(int i=0;i<2;i++)
	{
		gTPS.x[i]=0;
		gTPS.y[i]=0;
	}
	position->status = 0;
	position->x = gTPS.x[0];
	position->y = gTPS.y[0];
}


// int count = 0;
//触摸中断处理
void get_ft6336_touch_sta(TP_POSITION_T *position){
	scan_ft6336();
	count_position_ft6336(position);
	// if(touch_status == 1){
	// 	touch_status = 0;
	// 	count ++;
	// 	if(count%2){
	//     	ds_screen_test();
	// 	}else{
    //     	ds_screen_test2();
	// 	}
	// }
}

//初始化
void init_ft6336(){
    uint8_t w_data,r_data = 0;
	memset(&gTPS,0,sizeof(TouchPoint_T));

	//GPIO初始化 INT中断和复位引脚
    ds_touch_gpio_init();
	//复位初始化
    ds_gpio_set_touch_rst(GPIO_RST_LOW);
    vTaskDelay(50 / portTICK_PERIOD_MS);
    ds_gpio_set_touch_rst(GPIO_RST_HIGHT);
    vTaskDelay(100 / portTICK_PERIOD_MS);
	//I2C初始化
    i2c_master_init();
    vTaskDelay(100 / portTICK_PERIOD_MS);

    w_data=0;
	//设置为正常操作模式
	i2c_master_write_slave(FT_DEVIDE_MODE,&w_data,1);	    
 	w_data=22;								
	//设置触摸有效值22 越小越灵敏
 	i2c_master_write_slave(FT_ID_G_THGROUP,&w_data,1);	    
	i2c_master_read_slave(FT_ID_G_THGROUP,&r_data,1);
    printf("init THGROUP = %d \n",r_data);
 	w_data=14;								
	//设置激活周期 不能小于12 最大14
 	i2c_master_write_slave(FT_ID_G_PERIODACTIVE,&w_data,1); 
	i2c_master_read_slave(FT_ID_G_PERIODACTIVE,&r_data,1);
    printf("init PERIODACTIVE = %d \n",r_data);
	w_data = 0;
	//中断产生方式 持续电平
	i2c_master_write_slave(FT_ID_G_MODE,&w_data,1);
	i2c_master_read_slave(FT_ID_G_MODE,&r_data,1);
    printf("init G_MODE = %d \n",r_data);
}