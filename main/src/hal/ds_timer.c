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

#include "ds_timer.h"
#include "ds_system_data.h"
#include "ds_ft6336.h"
#include "ds_ui_timepage.h"
#include "ds_ui_tomatopage.h"

#define TIMER_DIVIDER         16  //  Hardware timer clock divider
#define TIMER_SCALE           (TIMER_BASE_CLK / TIMER_DIVIDER/1000)  // convert counter value to ms seconds
#define TIMER_INTERVAL0_SEC   (10) // sample test interval for the first timer
#define TEST_WITH_RELOAD      1        // testing will be done with auto reload

/*
 * A sample structure to pass events
 * from the timer interrupt handler to the main program.
 */
typedef struct {
    uint64_t timer_minute_count;
    uint64_t timer_second_count;
    uint64_t timer_second_count_isr;
} timer_event_t;

timer_event_t g_timer_event;

xQueueHandle timer_queue;
xQueueHandle ui_update_timer_queue;

/*
 * Timer group0 ISR handler
 *
 * Note:
 * We don't call the timer API here because they are not declared with IRAM_ATTR.
 * If we're okay with the timer irq not being serviced while SPI flash cache is disabled,
 * we can allocate this interrupt without the ESP_INTR_FLAG_IRAM flag and use the normal API.
 */
void IRAM_ATTR timer_group0_isr(void *para)
{
    // timer_spinlock_take(TIMER_GROUP_0);
    int timer_idx = (int) para;

    // /* Prepare basic event data
    //    that will be then sent back to the main program task */
    timer_event_t evt;

    timer_group_clr_intr_status_in_isr(TIMER_GROUP_0, TIMER_0);

    /* After the alarm has been triggered
      we need enable it again, so it is triggered the next time */
    timer_group_enable_alarm_in_isr(TIMER_GROUP_0, timer_idx);

    /* Now just send the event data back to the main program task */
    g_timer_event.timer_second_count_isr ++;
    //1s计算一次 
    if(g_timer_event.timer_second_count_isr >= 100){
        g_timer_event.timer_second_count_isr = 0;
        update_system_time_second();
    }
    xQueueSendFromISR(timer_queue, &evt, NULL);
    xQueueSendFromISR(ui_update_timer_queue, &evt, NULL);
    // timer_spinlock_give(TIMER_GROUP_0);
}

/*
 * Initialize selected timer of the timer group 0
 *
 * timer_idx - the timer number to initialize
 * auto_reload - should the timer auto reload on alarm?
 * timer_interval_sec - the interval of alarm to set
 */
static void tg0_timer_init(int timer_idx,
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

    /* Configure the alarm value and the interrupt on alarm. */
    timer_set_alarm_value(TIMER_GROUP_0, timer_idx, timer_interval_sec * TIMER_SCALE);
    timer_enable_intr(TIMER_GROUP_0, timer_idx);
    timer_isr_register(TIMER_GROUP_0, timer_idx, timer_group0_isr,
                       (void *) timer_idx, ESP_INTR_FLAG_IRAM, NULL);

    timer_start(TIMER_GROUP_0, timer_idx);
}

/*
 * The main task of this example program 10ms/1次
 */
static void timer_evt_task(void *arg)
{
    while (1) {
        timer_event_t evt;
        xQueueReceive(timer_queue, &evt, portMAX_DELAY);
        count_tp_action_manage_time();
        TP_POSITION_T position;
        if(get_tp_action_status() > 0 && get_tp_action_status() <= 2){
            get_ft6336_touch_sta(&position);
            if(position.status != 0)
                set_tp_action_manage_start_point(position.x,position.y);
        }else if(get_tp_action_status() > 2){
            get_ft6336_touch_sta(&position);
            if(position.status != 0)
                set_tp_action_manage_stop_point(position.x,position.y);
        }
    }
}

/*
 * The main task of this example program 10ms/1次
 */
static void ui_timer_update_task(void *arg)
{
    while (1) {
        timer_event_t evt;
        xQueueReceive(ui_update_timer_queue, &evt, portMAX_DELAY);
        g_timer_event.timer_minute_count ++;
        //60s 计算一次 刷新时间
        if(g_timer_event.timer_minute_count >= 6000){
            g_timer_event.timer_minute_count = 0;
            ds_ui_timepage_updatetime();
        }
        g_timer_event.timer_second_count ++;
        //1s计算一次 
        if(g_timer_event.timer_second_count >= 100){
            g_timer_event.timer_second_count = 0;
            ds_ui_tomatopage_updatetime();
        }
    }
}

/*
 * In this example, we will test hardware timer0 and timer1 of timer group0.
 */
void ds_timer_init(void)
{
    g_timer_event.timer_minute_count = 0;
    g_timer_event.timer_second_count = 0;
    g_timer_event.timer_second_count_isr = 0;
    timer_queue = xQueueCreate(10, sizeof(timer_event_t));
    ui_update_timer_queue = xQueueCreate(10, sizeof(timer_event_t));
    tg0_timer_init(TIMER_0, TEST_WITH_RELOAD, TIMER_INTERVAL0_SEC);
    xTaskCreate(timer_evt_task, "timer_evt_task", 2048, NULL, 5, NULL);
    xTaskCreate(ui_timer_update_task, "ui_timer_update_task", 2048, NULL, 5, NULL);
}

