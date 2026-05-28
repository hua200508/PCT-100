#include "exti.h"
#include "key.h"

volatile uint8_t master_state = 0;  // 总开关初始关闭
volatile uint8_t triggered  = 0;   // 初始：未触发

/* ---- 中断 ISR（仅 KEY1） ---- */
static volatile uint8_t k1_irq = 0;
static volatile unsigned long k1_time = 0;

void key1_isr(void) { k1_time = millis(); k1_irq = 1; }

void exti_init(void)
{
  key1_init();
  key2_init();
  attachInterrupt(digitalPinToInterrupt(KEY1_PIN), key1_isr, FALLING);
  /* KEY2 使用纯轮询，不注册中断 */
}

/* ---- 轮询消抖 + 逻辑处理 ---- */
void exti_process(void)
{
  /* --- KEY1 总开关 --- */
  {
    static uint8_t  stable = HIGH, last = HIGH, lock = 0;
    static unsigned long change = 0;

    uint8_t r = KEY1;
    if (r != last) { change = millis(); last = r; }
    if (millis() - change >= DEBOUNCE_MS && r != stable)
    {
      stable = r;
      if (stable == LOW && !lock)
      {
        master_state = !master_state;
        if (master_state)
          triggered = 0;          // 总开关开 → 进入闪烁待命
        Serial.print("Master:");
        Serial.println(master_state ? "ON" : "OFF");
        lock = 1;
      }
      else if (stable == HIGH && lock)
        lock = 0;
    }
    if (k1_irq) k1_irq = 0;
  }

  /* KEY2 控制键（纯轮询，仅在总开关开时有效） */
  {
    static uint8_t  stable = HIGH, last = HIGH, lock = 0;
    static unsigned long change = 0;

    uint8_t r = KEY2;
    if (r != last) { change = millis(); last = r; }
    if (millis() - change >= DEBOUNCE_MS && r != stable)
    {
      stable = r;
      if (stable == LOW && !lock && master_state)
      {
        triggered = !triggered;   // 闪烁 ↔ 常亮
        Serial.print("Relay:");
        Serial.println(triggered ? "ON" : "STANDBY");
        lock = 1;
      }
      else if (stable == HIGH && lock)
        lock = 0;
    }
  }
}
