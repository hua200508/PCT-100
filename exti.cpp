/*
 * exti.cpp — KEY2 消抖 + 长按/短按识别（轮询版）
 */

#include "exti.h"
#include "key.h"

extern bool master_switch;

volatile bool key2_pressed = false;
volatile unsigned long key2_press_time = 0;

static volatile unsigned long last_release = 0;
static const unsigned long K2_DEBOUNCE = 50;
static const unsigned long RELEASE_COOLDOWN = 200;
static const unsigned long LONG_PRESS_MS = 2000;

static bool press_confirmed = false;
static bool long_handled = false;
static bool debounced_level = LOW;
static bool raw_last_level = LOW;
static unsigned long last_level_change = 0;

uint8_t relay_mode = 0;   // 0=全关 2=灯亮 1=风扇转 3=全开
bool    auto_mode   = true;  // 默认自动模式

// 短按切换顺序: 00 → 01(风) → 10(灯) → 11(灯+风)
static const uint8_t seq[] = {0, 1, 2, 3};
static const uint8_t seq_len = 4;

void IRAM_ATTR key2_isr(void);  // 前置声明

void exti_init(void)
{
  pinMode(KEY2_PIN, INPUT_PULLDOWN);
}

static void do_short_press(void)
{
  if (!master_switch) {
    Serial.println("[KEY2] 短按无效：总开关未合上");
    return;
  }
  if (auto_mode) {
    Serial.println("[KEY2] 短按无效：当前为自动模式，请长按2秒切换");
    return;
  }
  // 手动模式：切换状态
  uint8_t pos = 0;
  for (uint8_t i = 0; i < seq_len; i++) {
    if (seq[i] == relay_mode) { pos = i; break; }
  }
  relay_mode = seq[(pos + 1) % seq_len];
  Serial.print("[KEY2] 短按 → ");
  switch (relay_mode) {
    case 0: Serial.println("00 灯灭，风扇停"); break;
    case 1: Serial.println("01 灯灭，风扇转"); break;
    case 2: Serial.println("10 灯亮，风扇停"); break;
    case 3: Serial.println("11 灯亮，风扇转"); break;
  }
}

static void do_long_press(void)
{
  long_handled = true;
  auto_mode = !auto_mode;
  if (auto_mode) relay_mode = 0;
  Serial.print("[KEY2] 长按 → ");
  Serial.println(auto_mode ? "自动模式（已复位到 00）" : "手动模式");
}

void exti_check(void)
{
  bool raw_level = digitalRead(KEY2_PIN);
  unsigned long now = millis();

  if (raw_level != raw_last_level) {
    raw_last_level = raw_level;
    last_level_change = now;
    return;
  }

  if (now - last_level_change < K2_DEBOUNCE) {
    return;
  }

  if (raw_level != debounced_level) {
    debounced_level = raw_level;
    if (debounced_level == HIGH) {
      if (now - last_release < RELEASE_COOLDOWN) {
        return;
      }
      key2_pressed = true;
      key2_press_time = now;
      press_confirmed = true;
      long_handled = false;
    } else {
      if (key2_pressed && press_confirmed && !long_handled) {
        unsigned long dur = now - key2_press_time;
        if (dur < LONG_PRESS_MS) {
          do_short_press();
        } else {
          do_long_press();
        }
      }
      key2_pressed = false;
      press_confirmed = false;
      long_handled = false;
      last_release = now;
    }
  }

  if (key2_pressed && press_confirmed && !long_handled) {
    unsigned long dur = now - key2_press_time;
    if (dur >= LONG_PRESS_MS) {
      do_long_press();
    }
  }
}
