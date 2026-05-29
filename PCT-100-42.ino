#include "key.h"
#include "relay.h"
#include "exti.h"

void setup()
{
  relay_init();
  exti_init();
  RELAY_OFF();

  Serial.begin(115200);
  unsigned long t = millis();
  while (!Serial && (millis() - t < 5000)) delay(10);

  Serial.println("Ready. KEY1=Poll(GPIO20)  KEY2=INT(GPIO21)  Relay=GPIO6  Fan=GPIO7");
}

void loop()
{
  exti_process();  // KEY1 轮询 + KEY2 中断消抖
  key2_charge();   // 维持 KEY2 引脚充电，保证中断检测

  if (master_state == 0)
  {
    RELAY_OFF();
    FAN_OFF();
  }
  else if (triggered == 1)
  {
    RELAY_ON();
    FAN_ON();
  }
  else
  {
    RELAY_OFF();
    FAN_OFF();
  }
}
