#ifndef __KEY_EXTI_H
#define __KEY_EXTI_H

#include <Arduino.h>

#define KEY1_PIN  21  // 总开关
#define KEY2_PIN  20  // 继电器控制

void key_exti_init(void);
void key1_isr(void);
void key2_isr(void);

#endif