#include "exti.h"
#include "key.h"

extern bool master_switch;

volatile bool key2_pressed = false;
volatile unsigned long key2_press_time = 0;

static volatile unsigned long k2_last_isr = 0;
static volatile unsigned long last_release = 0;
static const unsigned long K2_DEBOUNCE = 15;
static const unsigned long RELEASE_COOLDOWN = 200;

static bool long_handled = false;

uint8_t relay_mode = 0;   // 0=全关 2=灯亮 1=风扇转 3=全开
bool    auto_mode   = true;  // 默认自动模式

// 短按切换顺序: 00 → 01(风) → 10(灯) → 11(灯+风)
static const uint8_t seq[] = {0, 1, 2, 3};
static const uint8_t seq_len = 4;

void IRAM_ATTR key2_isr(void);  // 前置声明

void exti_init(void)
{
  pinMode(KEY2_PIN, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(KEY2_PIN), key2_isr, RISING);
}

void IRAM_ATTR key2_isr(void)
{
  unsigned long now = millis();
  if (now - k2_last_isr < K2_DEBOUNCE) return;
  if (now - last_release < RELEASE_COOLDOWN) return;
  k2_last_isr = now;
  key2_press_time = now;
  key2_pressed = true;
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
  static bool last_level = LOW;
  static uint8_t low_cnt = 0;
  static uint8_t high_cnt = 0;
  bool cur = digitalRead(KEY2_PIN);

  if (!key2_pressed) {
    last_level = cur;
    low_cnt = 0;
    high_cnt = 0;
    return;
  }

  unsigned long dur = millis() - key2_press_time;

  if (cur == HIGH) {
    last_level = HIGH;
    low_cnt = 0;
    high_cnt++;
    // 连续3次 HIGH（约15ms）才确认是真实按下，而非松手抖动毛刺
    if (high_cnt == 3) {
      long_handled = false;
    }
    if (!long_handled && dur >= 2000) {
      do_long_press();
    }
    return;
  }

  // cur == LOW：可能是松手，也可能是噪声，需要连续多次 LOW 确认
  high_cnt = 0;
  if (last_level == HIGH) {
    low_cnt = 1;
  } else if (dur >= K2_DEBOUNCE) {
    low_cnt++;
  }

  // 连续 3 次（约15ms）读到 LOW 才确认松手
  if (low_cnt >= 3) {
    if (!long_handled && dur >= 15) {
      do_short_press();
    }
    key2_pressed = false;
    low_cnt = 0;
    high_cnt = 0;
    last_release = millis();
  }

  last_level = cur;
}
