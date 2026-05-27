#include "key.h"

void key_init(void)
{
  // 启用内部下拉电阻。未按下时为低电平(0)，按下时为高电平(1)
  pinMode(KEY_PIN, INPUT_PULLDOWN);
}