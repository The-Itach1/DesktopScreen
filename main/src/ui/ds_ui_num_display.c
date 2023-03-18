#include "ds_screen.h"

// int num = 1;
// void partial_num(){
// 	unsigned int i;
// 	for(i=0;i<180;i++)	     
// 	{	
// 		if(num == 1){
// 			spi_send_data(gImage_num1[i]);  
// 		}else if(num == 2){
// 			spi_send_data(gImage_num2[i]);  
// 		}else if(num == 3){	
// 			spi_send_data(gImage_num1[i]);  
// 		}else{
// 			spi_send_data(gImage_num5[i]);  
// 		}
// 	}
// 	num ++;
// }

// void partial00_288(void)
// {
// 	unsigned int i;
// 	for(i=0;i<180;i++)	     
// 	{
// 		spi_send_data(0x00);  
// 	}  
// }



// void partial_time(){
// 	unsigned int i;
// 	for(i=0;i<36;i++)	     
// 	{
// 		spi_send_data(0x00);  
// 	}  
// }

// void partial_num_time(){
// 	unsigned int i;
// 	for(i=0;i<36;i++)	     
// 	{	
// 		spi_send_data(~gImage_num_time[i]);  
// 	}
// }


// void ds_ui_num_display(unsigned int x,unsigned int y,unsigned int size,){
//     init_display(); 
//     partial_display(0,num_size,hight,hight+num_size,partial00_288,partial_num);

// 	int num_size = 36;
// 	int hight = 40;
// 	init_display(); 
// 	partial_display(0,num_size,hight,hight+num_size,partial00_288,partial_num);
// 	// init_display(); 
// 	partial_display(num_size,num_size*2,hight,hight+num_size,partial00_288,partial_num);
	
// 	// init_display(); 
// 	partial_display(num_size*2,num_size*2+8,hight,hight+num_size,partial_time,partial_num_time);
	

// 	init_display(); 
// 	partial_display(num_size*2+8,num_size*3+8,hight,hight+num_size,partial00_288,partial_num);
// 	init_display(); 
// 	partial_display(num_size*3+8,num_size*4+8,hight,hight+num_size,partial00_288,partial_num);
// 	deep_sleep();
	
// }

















/***************** partial display 0~9 picture*************************/
// void partial00(void)
// {
// 	unsigned int i;
// 	for(i=0;i<256;i++)	     
// 	{
// 		spi_send_data(0x00);  
// 	}  
// }
// void partial01(void)
// {
// 	unsigned int i;
// 	for(i=0;i<256;i++)	     
// 	{
// 		spi_send_data (~gImage_num1[i]);
// 		driver_delay_xms(2);  
// 	}	
// }

// void partial02(void)
// {
//   unsigned int i;
// 		for(i=0;i<256;i++)	     
// 			{
// 			spi_send_data (~gImage_num2[i]);
// 			// driver_delay_xms(2);  
// 			}	
// 	}
// void partial03(void)
// {
// 	  unsigned int i;
// 		for(i=0;i<256;i++)	     
// 			{
// 			spi_send_data (~gImage_num3[i]);
// 			driver_delay_xms(2);  
// 			}	
// 	}
// void partial04(void)
// {
// 	  unsigned int i;
// 		for(i=0;i<256;i++)	     
// 			{
// 			spi_send_data (~gImage_num4[i]);
// 			driver_delay_xms(2);  
// 			}	
// 	}
// void partial05(void)
// {
// 	  unsigned int i;
// 		for(i=0;i<256;i++)	     
// 			{
// 			spi_send_data (~gImage_num5[i]);
// 			driver_delay_xms(2);  
// 			}	
// 	}
// void partial06(void)
// {
// 	  unsigned int i;
// 		for(i=0;i<256;i++)	     
// 			{
// 			spi_send_data (~gImage_num6[i]);
// 			driver_delay_xms(2);  
// 			}	
// 	}
// void partial07(void)
// {
// 	  unsigned int i;
// 		for(i=0;i<256;i++)	     
// 			{
// 			spi_send_data (~gImage_num7[i]);
// 			driver_delay_xms(2);  
// 			}	
// 	}
// void partial08(void)
// {
// 	  unsigned int i;
// 		for(i=0;i<256;i++)	     
// 			{
// 			spi_send_data (~gImage_num8[i]);
// 			driver_delay_xms(2);  
// 			}	
// 	}
// void partial09(void)
// {
// 	  unsigned int i;
// 		for(i=0;i<256;i++)	     
// 			{
// 			spi_send_data (~gImage_num9[i]);
// 			driver_delay_xms(2);  
// 			}	
// 	}