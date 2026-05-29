#ifndef __RELAY_H
#define __RELAY_H

#include <Arduino.h>

#define RELAY_PIN  18  // 继电器控制引脚

void relay_init(void);
void relay_on(void);
void relay_off(void);
void relay_loop(void);

#endif