#include "exti.h"
#include "key.h"

volatile uint8_t master_state = 0;  // 总开关初始关闭
volatile uint8_t triggered  = 0;   // 初始：未触发

void exti_init(void)
{
  key1_init();
  key2_init();
}

/* ---- KEY1 轮询 + KEY2 中断消抖处理 ---- */
void exti_process(void)
{
  /* --- KEY1 总开关（自锁：按下=LOW=关，弹起=HIGH=开） --- */
  /* 限频 50Hz，减少自锁开关短路占空比 */
  {
    static uint8_t  stable = HIGH, last = HIGH;
    static unsigned long change = 0, last_read = 0;

    if (millis() - last_read >= 20)
    {
      last_read = millis();
      uint8_t r = key1_read();
      if (r != last) { change = millis(); last = r; }
      if (millis() - change >= DEBOUNCE_MS && r != stable)
      {
        stable = r;
        master_state = (stable == LOW) ? 0 : 1;
        if (master_state)
          triggered = 0;
        Serial.print("Master:");
        Serial.println(master_state ? "ON" : "OFF");
      }
    }
  }

  /* --- KEY2 控制键（中断 FALLING + 消抖） --- */
  {
    static uint8_t lock = 0;
    static unsigned long lock_time = 0;

    if (key2_pressed())
    {
      if (!lock && master_state)
      {
        triggered = !triggered;
        Serial.print("Relay:");
        Serial.println(triggered ? "ON" : "STANDBY");
      }
      lock = 1;
      lock_time = millis();
    }
    if (lock && (millis() - lock_time >= DEBOUNCE_MS))
      lock = 0;
  }
}
