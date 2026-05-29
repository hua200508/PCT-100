#include "relay.h"
#include "key_exti.h"

void setup() {
  Serial.begin(115200);
  relay_init();    // 继电器初始化
  key_exti_init(); // 按键中断初始化
}

void loop() {
  // 主循环空跑，全部由中断控制
  relay_loop();
}