#include "relay.h"

void relay_init(void) {
  pinMode(RELAY_PIN, OUTPUT);
  relay_off(); // 默认关闭
}

void relay_on(void) {
  digitalWrite(RELAY_PIN, HIGH); // 继电器模块通常是高电平吸合，若不工作可改为LOW
}

void relay_off(void) {
  digitalWrite(RELAY_PIN, LOW);
}

void relay_loop(void) {
  // 可在这里加状态指示，比如串口打印
}