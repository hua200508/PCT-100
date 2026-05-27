#include "led.h"

void led_init(void)
{
  // 设置 IO8 为输出模式
  pinMode(LED_PIN, OUTPUT);
  // 初始输出高电平（灌入电流接法下，高电平为熄灭）
  digitalWrite(LED_PIN, HIGH);
}