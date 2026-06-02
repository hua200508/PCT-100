/*
 * display.h — SH1106 OLED 显示屏模块
 * ==================================
 *   驱动: U8G2 + SH1106, 128x64, I2C
 *   引脚: SDA=GPIO4, SCL=GPIO5
 */

#ifndef __DISPLAY_H
#define __DISPLAY_H

#include <Arduino.h>
#include <U8g2lib.h>
#include <Wire.h>

#define OLED_SDA  4
#define OLED_SCL  5

class DisplayManager {
public:
    DisplayManager();
    void begin();
    void update(float lightLux, float temperature,
                bool masterOn, bool autoMode,
                bool lightOn, bool fanOn,
                bool lightAlarm = false, bool tempAlarm = false,
                bool wifiConnected = false, const char* wifiIP = "");

private:
    U8G2_SH1106_128X64_VCOMH0_F_HW_I2C _u8g2;
    char _buf[32];                              // 格式化缓冲区
};

#endif
