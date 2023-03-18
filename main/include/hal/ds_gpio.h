#ifndef _DS_GPIO_H_
#define _DS_GPIO_H_

void ds_touch_gpio_init(void);
void ds_touch_gpio_isr_remove();
void ds_touch_gpio_isr_add();
void ds_screen_gpio_init(void);
void ds_gpio_set_screen_cs(uint32_t level);
void ds_gpio_set_screen_dc(uint32_t level);
void ds_gpio_set_screen_rst(uint32_t level);
int ds_gpio_get_screen_busy(void);
void ds_gpio_set_touch_rst(uint32_t level);

#endif

