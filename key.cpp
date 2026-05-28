#include "key.h"

void key1_init(void)
{
  pinMode(KEY1_PIN, INPUT_PULLUP);  // 上拉：平常 HIGH，碰 GND→LOW
}

void key2_init(void)
{
  pinMode(KEY2_PIN, INPUT_PULLUP);  // 上拉：平常 HIGH，碰 GND→LOW
}
