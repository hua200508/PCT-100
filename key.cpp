#include "key.h"
#include "exti.h"

bool master_switch = false;

void key_init(void)
{
  pinMode(KEY1_PIN, INPUT_PULLDOWN);
}

void key_poll(void)
{
  static uint8_t last_state = LOW;
  uint8_t cur = digitalRead(KEY1_PIN);

  if (cur != last_state) {
    delay(DEBOUNCE_MS);
    cur = digitalRead(KEY1_PIN);
    if (cur != last_state) {
      last_state = cur;
      master_switch = (cur == HIGH);
      if (master_switch) {
        relay_mode = 0;
        auto_mode  = true;
      }
      Serial.print("[KEY1] 总开关 ");
      Serial.println(master_switch ? "合上（自动+00）" : "断开");
    }
  }
}
