#include "key.h"

static volatile uint8_t k2_flag = 0;

static void IRAM_ATTR key2_isr(void) { k2_flag = 1; }

void key1_init(void) { pinMode(KEY1_PIN, INPUT); }

void key2_init(void)
{
  pinMode(KEY2_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(KEY2_PIN), key2_isr, FALLING);
}

/*
 * KEY1 轮询读取：虚拟上拉 → 读回。
 * 仅每 ~20ms 调用一次（由 exti_process 限频），避免自锁开关长时间短路。
 */
uint8_t key1_read(void)
{
  pinMode(KEY1_PIN, OUTPUT);
  digitalWrite(KEY1_PIN, HIGH);
  delayMicroseconds(1);
  pinMode(KEY1_PIN, INPUT);
  delayMicroseconds(1);
  return digitalRead(KEY1_PIN);
}

/*
 * KEY2 充电：虚拟上拉维持引脚 HIGH，供 FALLING 中断检测。
 * 每轮循环调用，间隔 ~30µs，远小于漏电时间（~260µs），引脚始终高于 VIH。
 * OUTPUT HIGH 期间的 RISING 不会触发 FALLING 中断。
 */
void key2_charge(void)
{
  pinMode(KEY2_PIN, OUTPUT);
  digitalWrite(KEY2_PIN, HIGH);
  delayMicroseconds(1);
  pinMode(KEY2_PIN, INPUT);
  delayMicroseconds(1);
}

uint8_t key2_pressed(void)
{
  uint8_t v = k2_flag;
  k2_flag = 0;
  return v;
}
