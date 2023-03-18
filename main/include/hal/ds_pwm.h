#ifndef _DS_PWM_H_
#define _DS_PWM_H_

typedef enum{
    BEEP_SHORT_100MS = 0,
    BEEP_SHORT_500MS,
    BEEP_LONG,
}BEEP_TYPE_E;

void ds_pwm_init(void);
void send_beep_event(BEEP_TYPE_E type);
void send_beep_event_from_isr(BEEP_TYPE_E type);

#endif