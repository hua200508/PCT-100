#include "relay.h"

void relay_init(void)
{
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  RELAY_OFF();
  FAN_OFF();
}
