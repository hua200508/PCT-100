/*
 * PCT-100-008.ino — 光敏+温控自动控制系统 + WiFi管理 (OLED中文显示)
 * ===============================================================
 *   KEY1 (GPIO20): 总开关（自锁按键）
 *     合上 → 系统使能
 *     断开 → 全部关闭
 *
 *   KEY2 (GPIO21): 多功能按键（轻触按键）
 *     短按 → 手动模式: 循环切换 00→01→10→11
 *     长按2秒 → 切换 自动/手动模式
 *
 *   自动模式:
 *     光照 < 90% → 亮灯;  > 95% → 灭灯  (滞回5%)
 *     温度 > 32°C → 风扇开; < 31.5°C → 风扇停 (滞回0.5°C)
 *   手动模式: KEY2 短按自由控制灯和风扇
 *
 *   传感器:
 *     光敏电阻 GL5516 → GPIO1 (ADC)
 *     DS18B20 温度传感器 → GPIO10 (1-Wire)
 *
 *   显示屏:
 *     SH1106 OLED 128×64 → SDA=GPIO4, SCL=GPIO5
 *
 *   输出:
 *     GPIO6 → 灯
 *     GPIO7 → 风扇
 *
 *   WiFi:
 *     自动记忆连接 + 串口交互配网
 */

#include "key.h"
#include "relay.h"
#include "exti.h"
#include "SensorManager.h"
#include "display.h"
#include "wifi_manager.h"

SensorManager   sm;
DisplayManager  display;
WiFiManager     wifi;

// 光照滞回阈值 (lux)
#define LIGHT_ON_THRESHOLD   150   // 光照>150 lux → 开灯
#define LIGHT_OFF_THRESHOLD  150   // 光照<150 lux → 关灯
#define LIGHT_DEBOUNCE_MS    500  // 光照状态稳定500ms后才切换

// 温度滞回阈值
#define FAN_ON_TEMP          33.0f  // 温度>33°C → 开风扇
#define FAN_OFF_TEMP         32.5f  // 温度<32.5°C → 关风扇

// ==================== setup ====================
void setup()
{
  Serial.begin(115200);
  unsigned long t = millis();
  while (!Serial && (millis() - t < 5000)) delay(10);

  sm.begin();
  display.begin();
  key_init();
  relay_init();
  exti_init();

  Serial.println("========================================");
  Serial.println("PCT-100-008 光敏+温控自动系统 + WiFi");
  Serial.println("----------------------------------------");
  Serial.println("KEY1=总开关  KEY2=多功能键");
  Serial.println("自动: 光照<150 lux亮灯  >200 lux灭灯 | 温度>32°C开风扇 <31.5°C关风扇");
  Serial.println("长按2秒: 切换自动/手动模式");
  Serial.print  ("DS18B20: GPIO");
  Serial.println(ONE_WIRE_BUS);
  Serial.println("========================================");

  // 启动WiFi管理（自动连接或配网）
  wifi.begin();
}

// ==================== loop ====================
void loop()
{
  // WiFi管理（处理状态机和串口输入）
  wifi.loop();

  key_poll();
  exti_check();

  // 读取光照传感器（每帧读取，ADC速度快）
  sm.readLightSensor();

  // 确定当前输出模式
  uint8_t effective_mode = relay_mode;

  // 自动模式: 根据光照 + 温度自动控制灯和风扇
  if (auto_mode && master_switch) {
    effective_mode = 0x00;  // 从全关开始，逐位叠加

    // ---- 光照控灯（滞回 + 消抖） ----
    {
      float light = sm.getLightLux();
      static bool  light_auto_on = false;
      static unsigned long light_change_time = 0;
      static bool debounce_pending = false;

      if (!debounce_pending) {
        bool need_change = false;
        if (light_auto_on) {
          need_change = (light < LIGHT_OFF_THRESHOLD);   // 已开灯 → 光照低于阈值关灯
        } else {
          need_change = (light > LIGHT_ON_THRESHOLD);    // 已关灯 → 光照高于阈值开灯
        }
        if (need_change) {
          debounce_pending  = true;
          light_change_time = millis();
        }
      } else {
        bool condition_holds = false;
        if (light_auto_on) {
          condition_holds = (light < LIGHT_OFF_THRESHOLD);
        } else {
          condition_holds = (light > LIGHT_ON_THRESHOLD);
        }
        if (!condition_holds) {
          debounce_pending = false;
        } else if (millis() - light_change_time >= LIGHT_DEBOUNCE_MS) {
          light_auto_on   = !light_auto_on;
          debounce_pending = false;
          Serial.print("[自动-光] 光照:");
          Serial.print(light, 0);
          Serial.print(" lux → ");
          Serial.println(light_auto_on ? "开灯" : "关灯");
        }
      }

      if (light_auto_on) effective_mode |= 0x02;  // 灯亮
    }

    // ---- 温度控风扇（滞回，无消抖——温度变化缓慢） ----
    {
      float temp = sm.getTemperatureC();
      static bool fan_auto_on = false;

      if (temp != DEVICE_DISCONNECTED_C) {
        if (fan_auto_on) {
          if (temp < FAN_OFF_TEMP) {
            fan_auto_on = false;
            Serial.print("[自动-温] 温度:");
            Serial.print(temp, 1);
            Serial.println("°C → 关风扇");
          }
        } else {
          if (temp > FAN_ON_TEMP) {
            fan_auto_on = true;
            Serial.print("[自动-温] 温度:");
            Serial.print(temp, 1);
            Serial.println("°C → 开风扇");
          }
        }
      }

      if (fan_auto_on) effective_mode |= 0x01;  // 风扇转
    }
  }

  // 执行继电器输出
  relay_update(master_switch, effective_mode);

  // 温度采样（每5秒，DS18B20转换需约750ms）
  static unsigned long last_temp = 0;
  if (master_switch && (millis() - last_temp > 5000)) {
    last_temp = millis();
    sm.readTemperature();
  }

  // 状态输出 + 屏幕刷新（每2秒）
  static unsigned long last_print = 0;
  if (millis() - last_print > 2000) {
    last_print = millis();

    float lightLux = sm.getLightLux();
    float temp  = sm.getTemperatureC();
    bool  lightOn = (effective_mode & 0x02);
    bool  fanOn   = (effective_mode & 0x01);

    // 报警判断
    bool lightAlarm = (lightLux > 150);
    bool tempAlarm  = (temp != DEVICE_DISCONNECTED_C && temp > 33.0f);

    // OLED 中文显示
    display.update(lightLux, temp, master_switch, auto_mode, lightOn, fanOn,
                   lightAlarm, tempAlarm, wifi.isConnected(), wifi.getLocalIP().c_str());

    // 串口输出
    Serial.print("[状态] 光照:");
    Serial.print(lightLux, 0);
    if (lightAlarm) Serial.print("[报警]");
    Serial.print(" lux | 开关:");
    Serial.print(master_switch ? "ON " : "OFF");
    Serial.print(" | 模式:");
    Serial.print(auto_mode ? "自动" : "手动");
    Serial.print(" | 灯:");
    Serial.print(lightOn ? "亮" : "灭");
    Serial.print(" 风扇:");
    Serial.print(fanOn ? "转" : "停");
    Serial.print(" | 温度:");
    if (temp == DEVICE_DISCONNECTED_C) {
      Serial.print("未连接");
    } else {
      Serial.print(temp, 1);
      if (tempAlarm) Serial.print("[报警]");
      Serial.print("C");
    }
    // WiFi状态
    Serial.print(" | WiFi:");
    if (wifi.isConnected()) {
      Serial.print(wifi.getLocalIP());
    } else {
      Serial.print("未连接");
    }
    Serial.println();
  }

  delay(5);  // 快速响应 KEY2 短按
}
