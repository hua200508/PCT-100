/*
 * wifi_manager.h — ESP32 WiFi 管理模块
 * ===================================
 *   功能:
 *     1. 上电检查Flash中是否有保存的WiFi信息
 *     2. 有则自动连接，成功直接使用
 *     3. 无或连接失败，进入交互式配网
 *     4. 新连接成功后保存到Flash
 *   模式: WIFI_STA (客户端)
 */

#ifndef __WIFI_MANAGER_H
#define __WIFI_MANAGER_H

#include <Arduino.h>
#include <WiFi.h>
#include <Preferences.h>

// Flash存储命名空间
#define PREFS_NAMESPACE "wifi_config"
#define KEY_SSID        "ssid"
#define KEY_PASSWORD    "password"

// WiFi状态机
enum WiFiState {
    WIFI_STATE_AUTO_CONNECT,   // 尝试自动连接
    WIFI_STATE_SCAN,           // 扫描网络
    WIFI_STATE_SELECT,         // 等待用户选择
    WIFI_STATE_INPUT_PASS,     // 等待输入密码
    WIFI_STATE_CONNECT,        // 正在连接
    WIFI_STATE_CONNECTED,      // 已连接
    WIFI_STATE_FAILED          // 连接失败
};

class WiFiManager {
public:
    WiFiManager();

    // 初始化（读取Flash中保存的凭据，尝试自动连接）
    void begin();

    // 主循环调用，处理状态机和串口输入
    void loop();

    // 获取当前WiFi状态
    WiFiState getState() const { return _state; }
    bool     isConnected() const { return (_state == WIFI_STATE_CONNECTED); }

    // 获取连接信息
    String   getLocalIP() const;
    String   getSSID() const   { return _connectedSSID; }
    int      getRSSI() const   { return _connectedRSSI; }

private:
    WiFiState _state;
    int       _selectedNetwork;
    String    _password;
    int       _networkCount;
    String    _savedSSID;
    String    _savedPassword;
    String    _connectedSSID;
    int       _connectedRSSI;

    Preferences _prefs;

    // 内部方法
    void doAutoConnect();
    void showNetworkList();
    void handleInput(const String& input);
    void doConnect();
    void showConnectionInfo();
};

#endif // __WIFI_MANAGER_H
