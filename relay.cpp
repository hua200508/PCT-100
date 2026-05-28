#include "relay.h"

void relay_init(void)
{
  pinMode(RELAY_PIN, OUTPUT);
  RELAY_OFF();
}
