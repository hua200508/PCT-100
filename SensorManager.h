/*
 * SensorManager.h
 * ESP32-C3 传感器管理器 (使用 OneWire + DallasTemperature 库)
 * - 光敏电阻 GL5516（ADC, IO1）
 * - DS18B20 温度传感器（1-Wire, GPIO10）
 *
 * 注: 输出控制由 relay.h/relay.cpp 统一管理，本模块仅负责数据采集
 */

#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Arduino.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// DS18B20 错误码定义
#define DEVICE_DISCONNECTED_C  (-127.0f)

// ==================== 引脚定义 ====================
#define ADC_PIN         1       // 光敏电阻 ADC 输入
#define ONE_WIRE_BUS    10      // DS18B20 数据引脚 → GPIO10

// ==================== 传感器 / 控制参数 ====================
#define R_FIXED         10000.0 // 分压固定电阻 10KΩ
#define R_BRIGHT        500.0   // 强光下光敏电阻阻值(Ω)
#define R_DARK          50000.0 // 暗光下光敏电阻阻值(Ω)
#define LIGHT_THRESHOLD 50.0    // 光照阈值(%)，低于此值开灯

class SensorManager {
public:
    SensorManager();
    void begin();

    // --- 光照传感器 ---
    void   readLightSensor();
    int    getAdcValue()        const { return _adcValue; }
    float  getLightVoltage()    const { return _voltage; }
    float  getPhotoResistance() const { return _rPhoto; }
    float  getLightIntensity()  const { return _lightPercent; }
    float  getLightLux()        const { return _lightLux; }

    // --- 温度传感器 ---
    void   readTemperature();
    float  getTemperatureC()    const { return _tempC; }

private:
    OneWire*            _oneWire;     // 1-Wire 总线对象
    DallasTemperature*  _sensors;     // DS18B20 传感器对象

    // --- 传感器数据 ---
    int    _adcValue;
    float  _voltage;
    float  _rPhoto;
    float  _lightPercent;
    float  _lightLux;
    float  _tempC;
};

#endif // SENSOR_MANAGER_H
