#ifndef __RELAY_H
#define __RELAY_H

#include <Arduino.h>

#define RELAY_PIN 8   // GPIO8：继电器输出

#define RELAY_ON()  digitalWrite(RELAY_PIN, LOW)
#define RELAY_OFF() digitalWrite(RELAY_PIN, HIGH)

void relay_init(void);

#endif
