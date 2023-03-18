/* HTTP GET Example using plain POSIX sockets

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "esp_netif.h"
#include "esp_tls.h"
#include "esp_http_client.h"
#include "cJSON.h"

#include "ds_http_request.h"

#include "ds_ui_timepage.h"
#include "ds_ui_fans.h"
#include "ds_ui_weatherpage.h"
#include "ds_nvs.h"
#include "ds_system_data.h"

static const char *TAG = "HTTP_CLIENT";

int fans_type = 0;

xQueueHandle http_request_event_queue;

/*
{
    "sysTime2":"2022-07-10 10:12:43",
    "sysTime1":"20220710101243"
}
*/
static void cjson_time_info(char *text)
{
    cJSON *root,*psub;
    char time[20];
    //截取有效json
    char *index=strchr(text,'{');
    strcpy(text,index);
    root = cJSON_Parse(text);
    if(root!=NULL)
    {
        psub = cJSON_GetObjectItem(root, "sysTime1");
        sprintf(time,"%s",psub->valuestring);
        ESP_LOGI(TAG,"sysTime:%s",time);
    }
    cJSON_Delete(root);

    int len = strlen(time);
    if(len < 11){
        return;
    }
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
    hour = (time[8] - '0')*10+time[9] - '0';
    minute = (time[10] - '0')*10+time[11] - '0';
    second = (time[12] - '0')*10+time[13] - '0';
    update_system_time(hour,minute,second);
}

//天气解析结构体
typedef struct 
{
    char city[20];
    char weather_text[20];
    char weather_code[2];
    char temperatur[3];
}weather_info;

weather_info weathe;
/*
{
    "results":[
        {
            "location":{
                "id":"WS0E9D8WN298",
                "name":"广州",
                "country":"CN",
                "path":"广州,广州,广东,中国",
                "timezone":"Asia/Shanghai",
                "timezone_offset":"+08:00"
            },
            "now":{
                "text":"多云",
                "code":"4",
                "temperature":"31"
            },
            "last_update":"2022-07-10T11:00:02+08:00"
        }
    ]
}
*/
void cjson_weather_info(char *text)
{
    cJSON *root,*psub;
    cJSON *arrayItem;
    //截取有效json
    char *index=strchr(text,'{');
    strcpy(text,index);
    root = cJSON_Parse(text);
    if(root!=NULL)
    {
        psub = cJSON_GetObjectItem(root, "results");
        arrayItem = cJSON_GetArrayItem(psub,0);

        cJSON *locat = cJSON_GetObjectItem(arrayItem, "location");
        cJSON *now = cJSON_GetObjectItem(arrayItem, "now");
        if((locat!=NULL)&&(now!=NULL))
        {
            psub=cJSON_GetObjectItem(locat,"name");
            sprintf(weathe.city,"%s",psub->valuestring);
            ds_ui_weather_set(VALUE_CITY,psub->valuestring);
            ESP_LOGI(TAG,"city:%s",weathe.city);

            psub=cJSON_GetObjectItem(now,"text");
            sprintf(weathe.weather_text,"%s",psub->valuestring);
            ds_ui_weather_set(VALUE_WEATHER,psub->valuestring);
            ESP_LOGI(TAG,"weather:%s",weathe.weather_text);
            
            psub=cJSON_GetObjectItem(now,"code");
            sprintf(weathe.weather_code,"%s",psub->valuestring);
            ds_ui_weather_set(VALUE_CODE,psub->valuestring);
            ESP_LOGI(TAG,"%s",weathe.weather_code);

            psub=cJSON_GetObjectItem(now,"temperature");
            sprintf(weathe.temperatur,"%s",psub->valuestring);
            ds_ui_weather_set(VALUE_TEMP,psub->valuestring);
            ESP_LOGI(TAG,"temperatur:%s",weathe.temperatur);
        }
    }
    cJSON_Delete(root);
}

/*
{
    "code":0,
    "message":"0",
    "ttl":1,
    "data":{
        "mid":383943678,
        "following":13,
        "whisper":0,
        "black":0,
        "follower":14233
    }
}
*/
void cjson_fans_info(char *text)
{
    cJSON *root,*psub,*ppsub;
    int fans = 0;
    //截取有效json
    char *index=strchr(text,'{');
    strcpy(text,index);
    root = cJSON_Parse(text);
    if(root!=NULL)
    {
        psub = cJSON_GetObjectItem(root, "data");
        if(psub!=NULL && psub->type == cJSON_Object){
            ppsub = cJSON_GetObjectItem(psub, "follower");
            if(ppsub != NULL && ppsub->type == cJSON_Number){
                fans = ppsub->valueint;
                ESP_LOGI(TAG,"fans:%d",fans);
            }
        }
    }
    if(fans_type == 0){
        ds_ui_fans_set_fans_num(fans);
        fans_type = 1;
    }else{
        ds_ui_fans_set_aqifans_num(fans);
        fans_type = 0;
    }
    cJSON_Delete(root);
}

/*
{
    "cip":"121.32.92.51",
    "cid":"440106",
    "cname":"广东省广州市天河区"
}
*/
static void cjson_city_info(char *text)
{
    cJSON *root,*psub;
    char name[20];
    char cid[20];
    //截取有效json
    char *index=strchr(text,'{');
    strcpy(text,index);
    root = cJSON_Parse(text);
    if(root!=NULL)
    {
        psub = cJSON_GetObjectItem(root, "cname");
        sprintf(name,"%s",psub->valuestring);
        ESP_LOGI(TAG,"name:%s",name);

        psub = cJSON_GetObjectItem(root, "cid");
        sprintf(cid,"%s",psub->valuestring);
        ESP_LOGI(TAG,"cid:%s",cid);
    }
    cJSON_Delete(root);
}

//事件回调
static esp_err_t _http_time_event_handle(esp_http_client_event_t *evt)
{
    static char* output_buffer; // Buffer to store response of http request from event handler
    static int output_len; // Stores number of bytes read
    switch(evt->event_id) {
        case HTTP_EVENT_ON_DATA://接收数据事件
            ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            if (!esp_http_client_is_chunked_response(evt->client)) {
                if (output_buffer == NULL) {
                    output_buffer = (char *) malloc(esp_http_client_get_content_length(evt->client));
                    output_len = 0;
                    if (output_buffer == NULL) {
                        ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
                        return ESP_FAIL;
                    }
                }
                memcpy(output_buffer + output_len, evt->data, evt->data_len);
                printf("%.*s\n", output_len, output_buffer);
                cjson_time_info((char*)output_buffer);
                output_len += evt->data_len;
            }
            break;
        case HTTP_EVENT_ERROR:
            break;
        case HTTP_EVENT_ON_CONNECTED:
            break;
        case HTTP_EVENT_HEADERS_SENT:
            break;
        case HTTP_EVENT_ON_HEADER:
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            if (output_buffer != NULL) {
                // Response is accumulated in output_buffer. Uncomment the below line to print the accumulated response
                // ESP_LOG_BUFFER_HEX(TAG, output_buffer, output_len);
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            int mbedtls_err = 0;
            esp_err_t err = esp_tls_get_and_clear_last_error(evt->data, &mbedtls_err, NULL);
            if (err != 0) {
                ESP_LOGI(TAG, "Last esp error code: 0x%x", err);
                ESP_LOGI(TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
            }
            if (output_buffer != NULL) {
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
    }
    return ESP_OK;
}

static esp_err_t _http_weather_event_handle(esp_http_client_event_t *evt)
{
    static char* output_buffer; // Buffer to store response of http request from event handler
    static int output_len; // Stores number of bytes read
    switch(evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADERS_SENT:
            ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_DATA:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            /*
             *  Check for chunked encoding is added as the URL for chunked encoding used in this example returns binary data.
             *  However, event handler can also be used in case chunked encoding is used.
             */
            if (!esp_http_client_is_chunked_response(evt->client)) {
                // If user_data buffer is configured, copy the response into the buffer
                if (evt->user_data) {
                    memcpy(evt->user_data + output_len, evt->data, evt->data_len);
                    printf("%.*s\n", output_len, (char*)evt->user_data);
                    cjson_weather_info((char*)evt->user_data);
                } else {
                    if (output_buffer == NULL) {
                        output_buffer = (char *) malloc(esp_http_client_get_content_length(evt->client));
                        output_len = 0;
                        if (output_buffer == NULL) {
                            ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
                            return ESP_FAIL;
                        }
                    }
                    memcpy(output_buffer + output_len, evt->data, evt->data_len);
                    printf("%.*s\n", output_len, output_buffer);
                    cjson_weather_info((char*)output_buffer);
                }
                output_len += evt->data_len;
            }

            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            if (output_buffer != NULL) {
                // Response is accumulated in output_buffer. Uncomment the below line to print the accumulated response
                // ESP_LOG_BUFFER_HEX(TAG, output_buffer, output_len);
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            int mbedtls_err = 0;
            esp_err_t err = esp_tls_get_and_clear_last_error(evt->data, &mbedtls_err, NULL);
            if (err != 0) {
                ESP_LOGI(TAG, "Last esp error code: 0x%x", err);
                ESP_LOGI(TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
            }
            if (output_buffer != NULL) {
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
    }
    return ESP_OK;
}

static esp_err_t _http_fans_event_handle(esp_http_client_event_t *evt)
{
    static char* output_buffer; // Buffer to store response of http request from event handler
    static int output_len; // Stores number of bytes read
    switch(evt->event_id) {
        case HTTP_EVENT_ON_DATA://接收数据事件
            ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            if (!esp_http_client_is_chunked_response(evt->client)) {
                if (output_buffer == NULL) {
                    output_buffer = (char *) malloc(esp_http_client_get_content_length(evt->client));
                    output_len = 0;
                    if (output_buffer == NULL) {
                        ESP_LOGE(TAG, "Failed to allocate memory for output buffer");
                        return ESP_FAIL;
                    }
                }
                memcpy(output_buffer + output_len, evt->data, evt->data_len);
                printf("%.*s\n", output_len, output_buffer);
                cjson_fans_info((char*)output_buffer);
                output_len += evt->data_len;
            }
            break;
        case HTTP_EVENT_ERROR:
            ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADERS_SENT:
            ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            if (output_buffer != NULL) {
                // Response is accumulated in output_buffer. Uncomment the below line to print the accumulated response
                // ESP_LOG_BUFFER_HEX(TAG, output_buffer, output_len);
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            int mbedtls_err = 0;
            esp_err_t err = esp_tls_get_and_clear_last_error(evt->data, &mbedtls_err, NULL);
            if (err != 0) {
                ESP_LOGI(TAG, "Last esp error code: 0x%x", err);
                ESP_LOGI(TAG, "Last mbedtls failure: 0x%x", mbedtls_err);
            }
            if (output_buffer != NULL) {
                free(output_buffer);
                output_buffer = NULL;
            }
            output_len = 0;
            break;
    }
    return ESP_OK;
}

static esp_err_t _http_city_event_handle(esp_http_client_event_t *evt)
{
    switch(evt->event_id) {
        case HTTP_EVENT_ON_DATA://接收数据事件
            ESP_LOGI(TAG, "HTTP_EVENT_ON_DATA, len=%d", evt->data_len);
            if (!esp_http_client_is_chunked_response(evt->client)) {
                printf("%.*s\n", evt->data_len, (char*)evt->data);
                cjson_city_info((char*)evt->data);
            }
            break;
        case HTTP_EVENT_ERROR:
            ESP_LOGI(TAG, "HTTP_EVENT_ERROR");
            break;
        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_CONNECTED");
            break;
        case HTTP_EVENT_HEADERS_SENT:
            ESP_LOGI(TAG, "HTTP_EVENT_HEADER_SENT");
            break;
        case HTTP_EVENT_ON_HEADER:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_HEADER, key=%s, value=%s", evt->header_key, evt->header_value);
            break;
        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "HTTP_EVENT_DISCONNECTED");
            break;
        case HTTP_EVENT_ON_FINISH:
            ESP_LOGI(TAG, "HTTP_EVENT_ON_FINISH");
            break;
    }
    return ESP_OK;
}

void http_time_get(){
    //http client配置
    esp_http_client_config_t config = {
        .method = HTTP_METHOD_GET, //get请求
        .url = "http://quan.suning.com/getSysTime.do",
        .event_handler = _http_time_event_handle,//注册时间回调
        .skip_cert_common_name_check = true,
    };
	esp_http_client_handle_t time_client = esp_http_client_init(&config);//初始化配置
	esp_err_t err = esp_http_client_perform(time_client);//执行请求

	if(err == ESP_OK)
	{
		ESP_LOGI(TAG, "Status = %d, content_length = %d",
				esp_http_client_get_status_code(time_client),//状态码
				esp_http_client_get_content_length(time_client));//数据长度
	}
	esp_http_client_cleanup(time_client);//断开并释放资源
}

void http_weather_get(){
    char count_url[130];
    char *city = ds_nvs_read_city();
    //判断是否有存储城市信息
    if(city != NULL){
        char url[] = "http://api.seniverse.com/v3/weather/now.json?key=SmazqPcltzTft-X3v&location=";
        uint8_t city_len = strlen(city);
        char citydata[30];
        memcpy(citydata,city,city_len);
        citydata[city_len] = '\0';
        strcat(url,citydata);
        strcat(url,"&language=zh-Hans&unit=c"); 
        printf("%s\n",url);
        strcpy(count_url,url);
        free(city);
    }else{
        strcpy(count_url,"http://api.seniverse.com/v3/weather/now.json?key=SmazqPcltzTft-X3v&location=chengdu&language=zh-Hans&unit=c");
    }
    //http client配置
    esp_http_client_config_t config = {
        .method = HTTP_METHOD_GET, //get请求
        .url = count_url,
        .event_handler = _http_weather_event_handle,//注册时间回调
        .skip_cert_common_name_check = true,
    };

	esp_http_client_handle_t weather_client = esp_http_client_init(&config);//初始化配置
	esp_err_t err = esp_http_client_perform(weather_client);//执行请求

	if(err == ESP_OK)
	{
		ESP_LOGI(TAG, "Status = %d, content_length = %d",
				esp_http_client_get_status_code(weather_client),//状态码
				esp_http_client_get_content_length(weather_client));//数据长度
	}
	esp_http_client_cleanup(weather_client);//断开并释放资源
}

void http_fans_get(){
    char url[100];
    if(fans_type == 0){
        //The_Itach1
        strcpy(url,"http://api.bilibili.com/x/relation/stat?vmid=540069568&jsonp=jsonp");
    }else{
        //阿奇
        strcpy(url,"http://api.bilibili.com/x/relation/stat?vmid=257459324&jsonp=jsonp");
    }

    //http client配置 
    esp_http_client_config_t config = {
        .method = HTTP_METHOD_GET, //get请求
        .url = url,
        .event_handler = _http_fans_event_handle,//注册时间回调
        .skip_cert_common_name_check = true,
    };

	esp_http_client_handle_t fans_client = esp_http_client_init(&config);//初始化配置
	esp_err_t err = esp_http_client_perform(fans_client);//执行请求

	if(err == ESP_OK)
	{
		ESP_LOGI(TAG, "Status = %d, content_length = %d",
				esp_http_client_get_status_code(fans_client),//状态码
				esp_http_client_get_content_length(fans_client));//数据长度
	}
	esp_http_client_cleanup(fans_client);//断开并释放资源
}

void http_city_get(){
    //http client配置
    esp_http_client_config_t config = {
        .method = HTTP_METHOD_GET, //get请求
        .url = "http://pv.sohu.com/cityjson?ie=utf-8", //请求url
        .event_handler = _http_city_event_handle,//注册时间回调
        .skip_cert_common_name_check = true,
    };

	esp_http_client_handle_t city_client = esp_http_client_init(&config);//初始化配置
	esp_err_t err = esp_http_client_perform(city_client);//执行请求

	if(err == ESP_OK)
	{
		ESP_LOGI(TAG, "Status = %d, content_length = %d",
				esp_http_client_get_status_code(city_client),//状态码
				esp_http_client_get_content_length(city_client));//数据长度
	}
	esp_http_client_cleanup(city_client);//断开并释放资源
}

void ds_http_post(void)
{
    // //http client配置
    // esp_http_client_config_t config = {
    //     .method = HTTP_METHOD_GET, //get请求
    //     .url = "http://quan.suning.com/getSysTime.do",
    //     .event_handler = _http_event_handle,//注册时间回调
    //     .skip_cert_common_name_check = true,
    // };

    // // // POST
    // // const char *post_data = "field1=value1&field2=value2";
    // // esp_http_client_set_url(client, "http://httpbin.org/post");
    // // esp_http_client_set_method(client, HTTP_METHOD_POST);
    // // esp_http_client_set_post_field(client, post_data, strlen(post_data));
    // // int err = esp_http_client_perform(client);
    // // if (err == ESP_OK) {
    // //     ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %d",
    // //             esp_http_client_get_status_code(client),
    // //             esp_http_client_get_content_length(client));

    // //             int len =  esp_http_client_get_content_length(client);
    // //             int read_len = 0;
    // //             char buf[1024] = {0};
    // //             read_len = esp_http_client_read(client, buf, len);
    // //             printf("\nrecv data len:%d %d %s\n",read_len,len,buf);
    // // } else {
    // //     ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
    // // }

    // esp_http_client_cleanup(client);
}

void ds_http_request_type(HTTP_REQUEST_TYPE_E type){
    HTTP_REQUEST_TYPE_E evt;
	evt = type;
	xQueueSend(http_request_event_queue, &evt, 0);
}

void ds_http_request_all(){
    ds_http_request_type(HTTP_GET_FANS);
    ds_http_request_type(HTTP_GET_WEATHER);
    ds_http_request_type(HTTP_GET_TIME);
    ds_http_request_type(HTTP_GET_FANS);
}

static void http_request_task(void *pvParameters)
{
    while(1) {
        HTTP_REQUEST_TYPE_E evt;
        xQueueReceive(http_request_event_queue, &evt, portMAX_DELAY);
        ESP_LOGI(TAG, "http_get_task %d",evt);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        switch (evt)
        {
        case HTTP_GET_TIME:
            http_time_get();
            break;
        case HTTP_GET_WEATHER:
            http_weather_get();
            break;
        case HTTP_GET_FANS:
            http_fans_get();
            break;
        case HTTP_GET_CITY:
            http_city_get();
            break;
        default:
            break;
        }
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

void ds_http_request_init(void)
{
	http_request_event_queue = xQueueCreate(10, sizeof(HTTP_REQUEST_TYPE_E));
    xTaskCreate(&http_request_task, "http_request_task", 4096, NULL, 5, NULL);
}
