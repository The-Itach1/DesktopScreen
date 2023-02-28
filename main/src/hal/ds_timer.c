/* Timer group-hardware timer example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "esp_types.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/periph_ctrl.h"
#include "driver/timer.h"
#include "esp_log.h"

#include "ds_timer.h"

static const char *TAG = "TIMER APP";

#define TIMER_DIVIDER         16  //  定时器时钟分频器 
#define TIMER_SCALE           (TIMER_BASE_CLK / TIMER_DIVIDER/1000)   //将计数器值转换为秒
#define TIMER_INTERVAL0_SEC   (10) // //第一个定时器的采样测试间隔 10毫秒
#define TEST_WITH_RELOAD      1        // 测试将在没有自动重新加载的情况下完成

/*
* 传递事件的示例结构
* 从定时器中断处理程序到主程序。
 */
typedef struct {
    uint64_t timer_minute_count;
    uint64_t timer_second_count;
} timer_event_t;

timer_event_t g_timer_event;

xQueueHandle timer_queue;

/*
* 定时器组 0 ISR 处理程序
*
* 笔记：
* 我们不在这里调用计时器 API，因为它们没有用 IRAM_ATTR 声明。
* 如果我们同意在禁用 SPI 闪存缓存时不为定时器 irq 提供服务，
* 我们可以在没有 ESP_INTR_FLAG_IRAM 标志的情况下分配此中断并使用普通 API。
 */
void IRAM_ATTR timer_group0_isr(void *para)
{
    timer_spinlock_take(TIMER_GROUP_0);
    int timer_idx = (int) para;

    /*准备基本的事件数据
       然后将被发送回主程序任务*/
    timer_event_t evt;

    /*清除中断
       并在不重新加载的情况下更新定时器的闹钟时间*/
    timer_group_clr_intr_status_in_isr(TIMER_GROUP_0, TIMER_0);

    /*警报被触发后
    我们需要再次启用它，以便下次触发*/
    timer_group_enable_alarm_in_isr(TIMER_GROUP_0, timer_idx);

    /*现在只需将事件数据发送回主程序任务*/
    xQueueSendFromISR(timer_queue, &evt, NULL);
    timer_spinlock_give(TIMER_GROUP_0);
}

/*
* 初始化定时器组 0 的选定定时器
*
* timer_idx - 要初始化的定时器编号
* auto_reload - 计时器是否应该在报警时自动重新加载？
* timer_interval_sec - 要设置的警报间隔
 */
static void example_tg0_timer_init(int timer_idx,
                                   bool auto_reload, double timer_interval_sec)
{
    /* Select and initialize basic parameters of the timer */
    timer_config_t config = {
        .divider = TIMER_DIVIDER,
        .counter_dir = TIMER_COUNT_UP,
        .counter_en = TIMER_PAUSE,
        .alarm_en = TIMER_ALARM_EN,
        .auto_reload = auto_reload,
    }; // default clock source is APB
    timer_init(TIMER_GROUP_0, timer_idx, &config);

    /* Timer's counter will initially start from value below.
       Also, if auto_reload is set, this value will be automatically reload on alarm */
    timer_set_counter_value(TIMER_GROUP_0, timer_idx, 0x00000000ULL);

    /*配置报警值和报警中断。*/
    timer_set_alarm_value(TIMER_GROUP_0, timer_idx, timer_interval_sec * TIMER_SCALE);
    timer_enable_intr(TIMER_GROUP_0, timer_idx);
    timer_isr_register(TIMER_GROUP_0, timer_idx, timer_group0_isr,
                       (void *) timer_idx, ESP_INTR_FLAG_IRAM, NULL);

    timer_start(TIMER_GROUP_0, timer_idx);
}

/*
* 本示例程序的主要任务
 */
static void timer_example_evt_task(void *arg)
{
    while (1) {
        timer_event_t evt;
        xQueueReceive(timer_queue, &evt, portMAX_DELAY);
        g_timer_event.timer_minute_count ++;
        //60s 计算一次 刷新时间
        if(g_timer_event.timer_minute_count >= 6000){
            g_timer_event.timer_minute_count = 0;
            ESP_LOGI(TAG, "1 minute run ");
        }
        g_timer_event.timer_second_count ++;
        //1s计算一次 
        if(g_timer_event.timer_second_count >= 100){
            g_timer_event.timer_second_count = 0;
            ESP_LOGI(TAG, "1s run ");
        }
    }
}

/*
 * In this example, we will test hardware timer0 and timer1 of timer group0.
 */
void ds_timer_init(void)
{
    //初始化
    g_timer_event.timer_minute_count = 0;
    g_timer_event.timer_second_count = 0;
    //创建一个新的队列实例，实际上就是分配了空间。
    timer_queue = xQueueCreate(10, sizeof(timer_event_t));

    //初始化定时器
    example_tg0_timer_init(TIMER_0, TEST_WITH_RELOAD, TIMER_INTERVAL0_SEC);
    //创建一个任务，用来接收时间数据
    xTaskCreate(timer_example_evt_task, "timer_evt_task", 2048, NULL, 5, NULL);
}

