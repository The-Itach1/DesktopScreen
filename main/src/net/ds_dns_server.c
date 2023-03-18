#include <string.h>
#include <sys/param.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
// #include "esp_event_loop.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "cJSON.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
#include "mbedtls/md5.h"

// static const char *TAG = "DNS_SERVER";

// static int create_udp_socket(int port)
// {
//     struct sockaddr_in saddr = { 0 };
//     int sock = -1;
//     int err = 0;

//     sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_IP);
//     if (sock < 0) {
//         ESP_LOGE(TAG,"Failed to create socket. Error %d", errno);
//         return -1;
//     }

//     // Bind the socket to any address
//     saddr.sin_family = PF_INET;
//     saddr.sin_port = htons(port);
//     saddr.sin_addr.s_addr = htonl(INADDR_ANY);
//     err = bind(sock, (struct sockaddr *)&saddr, sizeof(struct sockaddr_in));
//     if (err < 0) {
//         ESP_LOGE(TAG,"Failed to bind socket. Error %d", errno);
//         goto err;
//     }

//     // All set, socket is configured for sending and receiving
//     return sock;

// err:
//     close(sock);
//     return -1;
// }

// static void my_dns_server(void *pvParameters)
// {
// /* Wait for all the IPs we care about to be set
//     */
//     uint8_t data[128];
//     int len = 0;
//     struct sockaddr_in client = { 0 };
//     socklen_t  client_len=sizeof(struct sockaddr_in); 
//     uint32_t i = 0;

//     ESP_LOGI(TAG,"DNS server start ...");

//     int sock = create_udp_socket(53);

//     if (sock < 0) {
//         ESP_LOGE(TAG,"Failed to create IPv4 multicast socket");
//     }

//     while(1)
//     {

//         len=recvfrom(sock,data,100,0,(struct sockaddr *)&client,&client_len); //阻塞式

//         if((len < 0) || ( len > 100))
//         {
//             ESP_LOGE(TAG,"recvfrom error\n");
//             continue;
//         }

//         printf("DNS request:");
//         for(i = 0x4; i< len;i++)
//         {
//             if((data[i] >= 'a' && data[i] <= 'z') || (data[i] >= 'A' && data[i] <= 'Z') ||(data[i] >= '0' && data[i] <= '9'))
//                 printf("%c",data[i]);
//             else
//                 printf("_");

//         }
//         printf("\r\n");
//         //printf("%d\r\n",esp_get_free_heap_size()); //打印系统可用内存

//         //过滤掉一些乱糟糟的域名
//         if( strstr((const char *)data+0xc,"taobao")||
//             strstr((const char *)data+0xc,"qq")    || 
//             strstr((const char *)data+0xc,"sogou") ||
//             strstr((const char *)data+0xc,"amap")  ||
//             strstr((const char *)data+0xc,"alipay")||
//             strstr((const char *)data+0xc,"youku") ||
//             strstr((const char *)data+0xc,"iqiyi") ||
//             strstr((const char *)data+0xc,"baidu"))
//         {
//             continue;
//         }

//         data[2] |= 0x80;
//         data[3] |= 0x80;
//         data[7] =1;

//         data[len++] =0xc0;
//         data[len++] =0x0c;

//         data[len++] =0x00;
//         data[len++] =0x01;
//         data[len++] =0x00;
//         data[len++] =0x01;

//         data[len++] =0x00;
//         data[len++] =0x00;
//         data[len++] =0x00;
//         data[len++] =0x0A;

//         data[len++] =0x00;
//         data[len++] =0x04;

//         data[len++] =192;
//         data[len++] =168;
//         data[len++] =4;
//         data[len++] =1;

//         /*打印客户端地址和端口号*/
//         // inet_ntop(AF_INET,&client.sin_addr,(char *)data,sizeof(data));
//         // printf("client IP is %s, port is %d\n",data,ntohs(client.sin_port));

//         sendto(sock,data,len,0,(struct sockaddr*)&client,client_len);

//         vTaskDelay(10);
//     }

//     ESP_LOGE(TAG,"DNS　server stop ...");
//     shutdown(sock, 0);
//     close(sock);
//     vTaskDelete(NULL);
// }

void dns_server_start()
{
    // xTaskCreate(&my_dns_server, "dns_task", 2048, NULL, 5, NULL);
}


















// #define  BUFF_SIZE 1024
// /*
// #include <netdb.h>
// #include <sys/socket.h>
// */

// extern const uint8_t index_html_start[] asm("_binary_setting_html_start");
// extern const uint8_t index_html_end[]   asm("_binary_setting_html_end");


// static const char *HTTP_200 =   "HTTP/1.1 200 OK\r\n"
//                                 "Server: lwIP/1.4.0\r\n"
// 						        "Content-Type: text/html\r\n"
// 						        "Connection: Keep-Alive\r\n"
// 				                "Content-Length: %d \r\n\r\n";

// static const char *HTTP_400 =   "HTTP/1.0 400 BadRequest\r\n"
// 				                "Content-Length: 0\r\n"
// 				                "Connection: Close\r\n"
// 				                "Server: lwIP/1.4.0\r\n\n";

// int my_write(int fd,void *buffer,int length)
// {
//     int bytes_left;
//     int written_bytes;
//     char *ptr;

//     ptr=buffer;
//     bytes_left=length;
//     while(bytes_left>0)
//     {
// 		written_bytes=send(fd,ptr,bytes_left,0);
//         if(written_bytes<=0)
//         {
//                 if(errno==EINTR)
//                     written_bytes=0;
//                 else
//                     return(-1);
//         }
//         bytes_left-=written_bytes;
//         ptr+=written_bytes;
// 		vTaskDelay(10);
//     }
//     return(0);
// }

// void handle_http_request(void *pvParameters)
// {
//     char buff[BUFF_SIZE] = { 0 };  //数据缓冲器
//     int length = 0;

//     int fd = *(int*)pvParameters;
//     int bytes_recvd = 0;
// 	char *uri = NULL;

// 	ESP_LOGI(TAG,"Http Sub Task Run with socket: %d",fd);
	
// 	vTaskDelay(30);

// 	//读取HTTP请求头
//     bytes_recvd = recv(fd, buff, BUFF_SIZE - 1,0);
	
//     if (bytes_recvd <= 0) 
//     {
//         ESP_LOGE(TAG,"Recv requst header error!");
//         goto requst_error;
//     }

// 	//解析请求类型及请求URI
// 	uri = strstr(buff,"HTTP");
// 	if(uri == NULL)
// 	{
// 		ESP_LOGE(TAG,"Parase requst header error!");
//         goto requst_error;
// 	}
// 	uri[0] = 0; uri = NULL;

// 	uri = strstr(buff," ");
// 	if(uri == NULL)
// 	{
// 		ESP_LOGE(TAG,"Parase requst uri error!");
//         goto requst_error;
// 	}
// 	uri[0] = 0; uri ++;

//     ESP_LOGI(TAG,"the reqqust type is %s, uri is: %s",buff,uri);

// 	if(strcmp(buff,"GET") == 0) //响应GET请求
// 	{
// 		length = sprintf(buff,HTTP_200,index_html_end - index_html_start);
// 		my_write(fd,buff,length);
// 		my_write(fd,index_html_start, index_html_end - index_html_start);
// 	}
// 	else //其他请求不响应
// 	{
// 		my_write(fd,HTTP_400,strlen(HTTP_400));
// 	}

//     vTaskDelay(30);

// requst_error:
//     ESP_LOGI(TAG,"close socket %d",fd);
//     close(fd);
//     vTaskDelete(NULL);
// }


// void webserver(void *pvParameters)
// {
//     int sockfd,new_fd;/*socket句柄和建立连接后的句柄*/
// 	struct sockaddr_in my_addr;/*本方地址信息结构体，下面有具体的属性赋值*/
// 	struct sockaddr_in their_addr;/*对方地址信息*/
// 	socklen_t sin_size;

// 	struct timeval tv;//发送接收超时时间
// 	tv.tv_sec = 10;
//     tv.tv_usec = 0;

//     sin_size=sizeof(struct sockaddr_in);
// 	sockfd=socket(AF_INET,SOCK_STREAM,0);//建立socket 
// 	if(sockfd==-1)
//     {
// 		ESP_LOGE(TAG, "socket failed:%d",errno);
// 		goto web_err;;
// 	}
// 	my_addr.sin_family=AF_INET;/*该属性表示接收本机或其他机器传输*/
// 	my_addr.sin_port=htons(80);/*端口号*/
// 	my_addr.sin_addr.s_addr=htonl(INADDR_ANY);/*IP，括号内容表示本机IP*/
// 	bzero(&(my_addr.sin_zero),8);/*将其他属性置0*/

// 	if(bind(sockfd,(struct sockaddr*)&my_addr,sizeof(struct sockaddr))<0)//绑定地址结构体和socket
//     {
// 		ESP_LOGE(TAG,"bind error");
// 		goto web_err;
// 	}

//     listen(sockfd,8);//开启监听 ，第二个参数是最大监听数 
//     ESP_LOGI(TAG, "webserver start...");
//     while(1)
//     {
//         new_fd=accept(sockfd,(struct sockaddr*)&their_addr,&sin_size);//在这里阻塞知道接收到消息，参数分别是socket句柄，接收到的地址信息以及大小 
//         if(new_fd==-1)
//         {
//            ESP_LOGE(TAG,"accept failed");
//         }
//         else
//         {
//             ESP_LOGI(TAG,"Accept new socket: %d",new_fd);

// 			setsockopt(new_fd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
// 			setsockopt(new_fd, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof(tv));

//             int *para_fd = malloc(sizeof(int));
//             *para_fd = new_fd;
//             xTaskCreate(&handle_http_request, "socket_task", 1024*3, para_fd, 6, NULL);
//         }
// 		vTaskDelay(10);
// 	}

// web_err:
//     vTaskDelete(NULL);
// }

void web_server_start(void)
{
    // xTaskCreate(&webserver, "webserver_task", 2048, NULL, 5, NULL);
}