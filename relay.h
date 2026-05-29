#ifndef __RELAY_H
#define __RELAY_H

#include <Arduino.h>

#define RELAY_PIN 6   // GPIO6：继电器输出（OUT3 → RLY3 → USB2）
#define FAN_PIN   7   // GPIO7：风扇

#define RELAY_ON()  digitalWrite(RELAY_PIN, HIGH)
#define RELAY_OFF() digitalWrite(RELAY_PIN, LOW)
#define FAN_ON()    digitalWrite(FAN_PIN, HIGH)
#define FAN_OFF()   digitalWrite(FAN_PIN, LOW)

void relay_init(void);

#endif
