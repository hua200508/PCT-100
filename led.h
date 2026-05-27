#ifndef __LED_H
#define __LED_H

#include <Arduino.h>

// 使用引脚 8
#define LED_PIN 8

// 宏定义：控制LED引脚电平
#define LED(x) digitalWrite(LED_PIN, x)

void led_init(void);

#endif