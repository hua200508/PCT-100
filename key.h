#ifndef __KEY_H
#define __KEY_H

#include <Arduino.h>

// 使用引脚 8
#define KEY_PIN 8

// 读取按键状态的宏
#define KEY digitalRead(KEY_PIN)

void key_init(void);

#endif