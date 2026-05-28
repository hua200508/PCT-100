#ifndef __KEY_H
#define __KEY_H

#include <Arduino.h>

#define KEY1_PIN 3    // GPIO3：总开关
#define KEY2_PIN 10   // GPIO10：继电器控制

#define KEY1 digitalRead(KEY1_PIN)
#define KEY2 digitalRead(KEY2_PIN)

void key1_init(void);
void key2_init(void);

#endif
