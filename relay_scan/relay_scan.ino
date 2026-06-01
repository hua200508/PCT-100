/*
 * 继电器 GPIO 扫描工具
 * 每隔 2 秒切换一个 GPIO，灯亮说明找到了正确的引脚
 * 打开串口监视器（115200）观察当前测试的 GPIO
 */

// GPIO0-10, 18-21 逐个测试（跳过 20,21 已被按键占用）
const uint8_t pins[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 18, 19};
const int count = sizeof(pins) / sizeof(pins[0]);
int idx = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println("=== Relay GPIO Scanner ===");

  // 全部初始化为输出、低电平
  for (int i = 0; i < count; i++) {
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], LOW);
  }
}

void loop()
{
  uint8_t pin = pins[idx];

  // 关掉上一个
  if (idx > 0) digitalWrite(pins[idx - 1], LOW);
  else digitalWrite(pins[count - 1], LOW);

  Serial.print("Testing GPIO");
  Serial.print(pin);
  Serial.print(" ... ");

  // 先高电平 1.5 秒
  digitalWrite(pin, HIGH);
  Serial.println("HIGH (灯应该亮)");
  delay(1500);

  // 低电平 0.5 秒（灭一下区分）
  digitalWrite(pin, LOW);
  delay(500);

  idx = (idx + 1) % count;
}
