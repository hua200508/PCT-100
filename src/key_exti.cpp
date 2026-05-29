#include "key_exti.h"
#include "relay.h"

// 全局状态（必须加volatile，防止编译器优化）
volatile uint8_t key1_enable = 0; // KEY1总开关：0关闭 1开启
volatile uint8_t relay_state = 0; // 继电器状态

void key_exti_init(void) {
  // KEY1 初始化：上拉输入
  pinMode(KEY1_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(KEY1_PIN), key1_isr, FALLING);

  // KEY2 初始化：上拉输入
  pinMode(KEY2_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(KEY2_PIN), key2_isr, FALLING);
}

// KEY1中断：总开关
void key1_isr(void) {
  // 简易消抖（中断里delay不推荐，只是为了简单）
  delay(20);
  if (digitalRead(KEY1_PIN) == LOW) {
    key1_enable = !key1_enable;

    if (key1_enable == 0) {
      relay_off(); // 总关 → 继电器强制关闭
      relay_state = 0;
    }
  }
}

// KEY2中断：控制继电器
void key2_isr(void) {
  delay(20);
  if (digitalRead(KEY2_PIN) == LOW && key1_enable == 1) {
    relay_state = !relay_state;
    if (relay_state == 1) {
      relay_on();
    } else {
      relay_off();
    }
  }
}