#ifndef __EXTI_H
#define __EXTI_H

#include <Arduino.h>

#define DEBOUNCE_MS 15  // 消抖时间（毫秒）

/* KEY1=轮询+虚拟上拉,  KEY2=中断(FALLING)+虚拟上拉 */
/* master_state: 0=总开关关 1=总开关开                   */
/* triggered:    0=待命      1=常亮                      */
extern volatile uint8_t master_state;
extern volatile uint8_t triggered;

void exti_init(void);
void exti_process(void);

#endif
