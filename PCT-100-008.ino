#include "key.h"
#include "relay.h"
#include "exti.h"

extern uint8_t relay_mode;
extern bool auto_mode;

void setup()
{
  Serial.begin(115200);
  unsigned long t = millis();
  while (!Serial && (millis() - t < 5000)) delay(10);

  key_init();
  relay_init();
  exti_init();

  Serial.println("System Ready. KEY1=总开关  KEY2=多功能键");
  Serial.println("短按顺序: 全关→风扇转→灯亮→全开→循环");
  Serial.println("长按2秒切换手动/自动模式");
}

void loop()
{
  key_poll();
  exti_check();

  relay_update(master_switch, relay_mode);

  // 每 200ms 输出状态（仅在变化时）
  static unsigned long last_print = 0;
  static bool last_master = false;
  static uint8_t last_mode = 0;
  static bool last_auto = true;

  if (millis() - last_print > 200) {
    if (last_master != master_switch || last_mode != relay_mode || last_auto != auto_mode) {
      Serial.print("[状态] 总开关:");
      Serial.print(master_switch ? "ON " : "OFF");
      Serial.print(" | 模式:");
      Serial.print(auto_mode ? "自动" : "手动");
      Serial.print(" | 输出:");
      switch (relay_mode) {
        case 0: Serial.println("00(灯灭,风扇停)"); break;
        case 1: Serial.println("01(灯灭,风扇转)"); break;
        case 2: Serial.println("10(灯亮,风扇停)"); break;
        case 3: Serial.println("11(灯亮,风扇转)"); break;
      }
      last_master = master_switch;
      last_mode   = relay_mode;
      last_auto   = auto_mode;
    }
    last_print = millis();
  }

  delay(5);  // 快速响应 KEY2 短按
}
