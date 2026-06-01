/*
 * 硬件诊断 v2
 * 打开串口监视器 115200，等连接建立后再输出
 */
#include <Arduino.h>

#define KEY1_PIN 20
#define KEY2_PIN 21
#define RELAY_PIN 6

void setup()
{
  pinMode(KEY1_PIN, INPUT);
  pinMode(KEY2_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);

  // 先闪灯 3 次确认程序在跑
  for (int i = 0; i < 3; i++) {
    digitalWrite(RELAY_PIN, LOW);
    delay(200);
    digitalWrite(RELAY_PIN, HIGH);
    delay(200);
  }

  Serial.begin(115200);
  // 等 USB CDC 建立（最多等 5 秒）
  unsigned long t = millis();
  while (!Serial && (millis() - t < 5000)) {
    delay(10);
  }

  Serial.println("\n=== Diag v2 ===");
  Serial.println("KEY1=GPIO20  KEY2=GPIO21  RELAY=GPIO6");
}

void loop()
{
  int k1 = digitalRead(KEY1_PIN);
  int k2 = digitalRead(KEY2_PIN);

  Serial.print("KEY1=");
  Serial.print(k1);
  Serial.print(" KEY2=");
  Serial.print(k2);

  if (k1 == LOW) {
    digitalWrite(RELAY_PIN, LOW);
    Serial.println(" -> ON");
  } else {
    digitalWrite(RELAY_PIN, HIGH);
    Serial.println(" -> OFF");
  }

  delay(500);
}
