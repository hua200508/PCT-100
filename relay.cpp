#include "relay.h"

void relay_init(void)
{
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(FAN_PIN, OUTPUT);
  RELAY_OFF();
  FAN_OFF();
}

void relay_update(bool master_on, uint8_t combo)
{
  if (!master_on) {
    RELAY_OFF();
    FAN_OFF();
    return;
  }

  // bit1 (0x02) = 灯, bit0 (0x01) = 风扇
  (combo & 0x02) ? RELAY_ON() : RELAY_OFF();
  (combo & 0x01) ? FAN_ON()   : FAN_OFF();
}
