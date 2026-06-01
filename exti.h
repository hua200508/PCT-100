#ifndef __EXTI_H
#define __EXTI_H

#include <Arduino.h>

extern volatile bool    key2_pressed;
extern volatile unsigned long key2_press_time;
extern uint8_t relay_mode;
extern bool    auto_mode;

void exti_init(void);
void exti_check(void);  // 主循环调用，检测松手并处理长按/短按

#endif
