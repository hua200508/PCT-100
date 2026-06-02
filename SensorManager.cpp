/*
 * SensorManager.cpp
 * 传感器管理器实现 (OneWire + DallasTemperature 库)
 * 注: 输出控制已移除，由 relay 模块统一管理
 */

#include "SensorManager.h"

// ==================== 构造函数 ====================
SensorManager::SensorManager()
    : _adcValue(0)
    , _voltage(0.0f)
    , _rPhoto(0.0f)
    , _lightPercent(0.0f)
    , _lightLux(0.0f)
    , _tempC(DEVICE_DISCONNECTED_C)
    , _oneWire(nullptr)
    , _sensors(nullptr)
{
}

// ==================== 初始化 ====================
void SensorManager::begin() {
    // 设置ADC衰减（测量0-3.1V范围）
    analogSetAttenuation(ADC_11db);

    // 初始化 DS18B20（OneWire + DallasTemperature）
    _oneWire = new OneWire(ONE_WIRE_BUS);
    _sensors = new DallasTemperature(_oneWire);
    _sensors->begin();

    int deviceCount = _sensors->getDeviceCount();
    if (deviceCount > 0) {
        _sensors->setResolution(12);
    }
}

// ==================== 读取光敏电阻 ====================
void SensorManager::readLightSensor() {
    _adcValue = analogRead(ADC_PIN);
    _voltage = _adcValue * (3.3f / 4095.0f);
    _rPhoto = R_FIXED * _voltage / (3.3f - _voltage);
    _lightPercent = map(constrain((int)_rPhoto, R_BRIGHT, R_DARK),
                        R_DARK, R_BRIGHT, 0, 100);

    // 计算 lux: lux = (反相ADC)^2 / 30000
    int invertedAdc = 4095 - _adcValue;
    _lightLux = (float)(invertedAdc * invertedAdc) / 30000.0f;
}

// ==================== 读取DS18B20温度 ====================
void SensorManager::readTemperature() {
    if (_sensors->getDeviceCount() == 0) {
        _tempC = DEVICE_DISCONNECTED_C;
        return;
    }

    _sensors->requestTemperatures();
    float temp = _sensors->getTempCByIndex(0);

    if (temp == DEVICE_DISCONNECTED_C) {
        _tempC = DEVICE_DISCONNECTED_C;
    } else {
        _tempC = temp;
    }
}
