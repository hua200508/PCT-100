#include "key.h"
#include "relay.h"
#include "exti.h"

void setup()
{
  relay_init();
  exti_init();
  RELAY_OFF();

  Serial.begin(115200);
  Serial.println("Ready. Master=GPIO3(INT), Key=GPIO10(POLL)");
}

void loop()
{
  exti_process();  // 中断 + 轮询消抖

  if (master_state == 0)
  {
    RELAY_OFF();   // 总开关关 → 灯灭
  }
  else if (triggered == 1)
  {
    RELAY_ON();    // 已触发 → 常亮
  }
  else
  {
    /* 待命闪烁：500ms 周期 */
    static unsigned long last_toggle = 0;
    if (millis() - last_toggle >= 250)
    {
      last_toggle = millis();
      digitalWrite(RELAY_PIN, !digitalRead(RELAY_PIN));  // 翻转
    }
  }
}
