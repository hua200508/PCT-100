#ifndef __KEY_H
#define __KEY_H

#include <Arduino.h>

#define KEY1_PIN 20   // U6 总开关（自锁）→ GPIO20（轮询）
#define KEY2_PIN 21   // U5 多功能开关（轻触）→ GPIO21（中断）

void key1_init(void);
void key2_init(void);
uint8_t key1_read(void);     // 虚拟上拉读取（轮询用）
void key2_charge(void);      // 虚拟上拉充电（每轮调用，维持中断检测）
uint8_t key2_pressed(void);  // 读取并清除中断标志

#endif
