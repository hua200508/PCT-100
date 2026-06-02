/*
 * display.cpp — SH1106 OLED 中文显示实现
 * =====================================
 *   字体: u8g2_font_wqy12_t_gb2312 (文泉驿12px)
 *   编码: UTF-8（源文件需保存为 UTF-8）
 */

#include "display.h"
#include "SensorManager.h"      // DEVICE_DISCONNECTED_C

DisplayManager::DisplayManager()
    : _u8g2(U8G2_R0, U8X8_PIN_NONE)             // 无复位引脚
{
}

void DisplayManager::begin() {
    Wire.begin(OLED_SDA, OLED_SCL);
    _u8g2.begin();
    _u8g2.enableUTF8Print();

    // 启动画面
    _u8g2.clearBuffer();
    _u8g2.setFont(u8g2_font_wqy12_t_gb2312);
    _u8g2.drawUTF8(10, 20, "PCT-100");
    _u8g2.drawUTF8(10, 38, "系统启动中...");
    _u8g2.sendBuffer();
}

// 报警阈值
#define LIGHT_ALARM_THRESHOLD  150
#define TEMP_ALARM_THRESHOLD   33.0f

void DisplayManager::update(float lightLux, float temperature,
                             bool masterOn, bool autoMode,
                             bool lightOn, bool fanOn,
                             bool lightAlarm, bool tempAlarm,
                             bool wifiConnected, const char* wifiIP) {
    _u8g2.clearBuffer();
    _u8g2.setFont(u8g2_font_wqy12_t_gb2312);

    // ====== 第1行: 模式 + 总闸 ======
    snprintf(_buf, sizeof(_buf), "模式：%s",
             autoMode ? "自动" : "手动");
    _u8g2.drawUTF8(0, 12, _buf);

    snprintf(_buf, sizeof(_buf), "总闸：%s",
             masterOn ? "ON" : "OFF");
    _u8g2.drawUTF8(64, 12, _buf);

    // ====== 第2行: 光照 lux / 150 ======
    if (lightAlarm) {
        snprintf(_buf, sizeof(_buf), "光照：%d / %d !",
                 (int)lightLux, LIGHT_ALARM_THRESHOLD);
    } else {
        snprintf(_buf, sizeof(_buf), "光照：%d / %d",
                 (int)lightLux, LIGHT_ALARM_THRESHOLD);
    }
    _u8g2.drawUTF8(0, 24, _buf);

    // ====== 第3行: 温度 / 33.0 ======
    if (temperature == DEVICE_DISCONNECTED_C) {
        snprintf(_buf, sizeof(_buf), "温度：--.- / %.1f",
                 TEMP_ALARM_THRESHOLD);
    } else if (tempAlarm) {
        snprintf(_buf, sizeof(_buf), "温度：%.1f / %.1f !",
                 temperature, TEMP_ALARM_THRESHOLD);
    } else {
        snprintf(_buf, sizeof(_buf), "温度：%.1f / %.1f",
                 temperature, TEMP_ALARM_THRESHOLD);
    }
    _u8g2.drawUTF8(0, 36, _buf);

    // ====== 第4行: 灯光 + 风扇 ======
    snprintf(_buf, sizeof(_buf), "灯：%s",
             lightOn ? "ON" : "OFF");
    _u8g2.drawUTF8(0, 48, _buf);

    snprintf(_buf, sizeof(_buf), "扇：%s",
             fanOn ? "ON" : "OFF");
    _u8g2.drawUTF8(48, 48, _buf);

    // ====== 第5行: WiFi状态 ======
    if (wifiConnected && strlen(wifiIP) > 0) {
        snprintf(_buf, sizeof(_buf), "IP:%s", wifiIP);
    } else {
        snprintf(_buf, sizeof(_buf), "WiFi:未连接");
    }
    _u8g2.drawUTF8(0, 62, _buf);

    _u8g2.sendBuffer();
}
