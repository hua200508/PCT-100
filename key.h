#ifndef __KEY_H
#define __KEY_H

#include <Arduino.h>

#define KEY1_PIN    20
#define KEY2_PIN    21
#define DEBOUNCE_MS 10

extern bool master_switch;

void key_init(void);
void key_poll(void);

#endif
