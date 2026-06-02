/*
 * wifi_manager.cpp — ESP32 WiFi 管理模块实现
 */

#include "wifi_manager.h"

WiFiManager::WiFiManager()
    : _state(WIFI_STATE_AUTO_CONNECT)
    , _selectedNetwork(-1)
    , _password("")
    , _networkCount(0)
    , _savedSSID("")
    , _savedPassword("")
    , _connectedSSID("")
    , _connectedRSSI(0)
{
}

void WiFiManager::begin() {
    Serial.println("\n========================================");
    Serial.println("[WiFi] ESP32 WiFi 管理模块启动");
    Serial.println("========================================");

    // 读取保存的WiFi信息
    _prefs.begin(PREFS_NAMESPACE, false);
    _savedSSID = _prefs.getString(KEY_SSID, "");
    _savedPassword = _prefs.getString(KEY_PASSWORD, "");

    if (_savedSSID.length() > 0) {
        Serial.print("[WiFi] 发现已保存的WiFi: ");
        Serial.println(_savedSSID);
        Serial.println("[WiFi] 正在尝试自动连接...");
        _state = WIFI_STATE_AUTO_CONNECT;
    } else {
        Serial.println("[WiFi] 未保存WiFi信息，进入配网模式");
        _state = WIFI_STATE_SCAN;
        showNetworkList();
        _state = WIFI_STATE_SELECT;
    }
}

void WiFiManager::loop() {
    // 处理串口输入
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        input.trim();
        if (input.length() > 0) {
            handleInput(input);
        }
    }

    // 状态机处理
    switch (_state) {
        case WIFI_STATE_AUTO_CONNECT:
            doAutoConnect();
            break;

        case WIFI_STATE_CONNECT:
            doConnect();
            break;

        case WIFI_STATE_CONNECTED:
            // 保持连接，定期检查状态
            if (WiFi.status() != WL_CONNECTED) {
                Serial.println("\n[WiFi] 警告: 连接已断开!");
                _state = WIFI_STATE_FAILED;
            }
            break;

        case WIFI_STATE_FAILED:
            // 连接失败后进入交互式配网
            delay(1500);
            showNetworkList();
            _state = WIFI_STATE_SELECT;
            break;

        default:
            break;
    }
}

// ==================== 自动连接 ====================
void WiFiManager::doAutoConnect() {
    WiFi.mode(WIFI_STA);
    if (_savedPassword.length() > 0) {
        WiFi.begin(_savedSSID.c_str(), _savedPassword.c_str());
    } else {
        WiFi.begin(_savedSSID.c_str());
    }

    int timeout = 20;  // 10秒超时
    while (WiFi.status() != WL_CONNECTED && timeout > 0) {
        delay(500);
        Serial.print(".");
        timeout--;
    }

    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
        _connectedSSID = _savedSSID;
        _connectedRSSI = WiFi.RSSI();
        Serial.println("[WiFi] 自动连接成功!");
        showConnectionInfo();
        _state = WIFI_STATE_CONNECTED;
    } else {
        Serial.println("[WiFi] 自动连接失败，进入配网模式");
        showNetworkList();
        _state = WIFI_STATE_SELECT;
    }
}

// ==================== 扫描网络列表 ====================
void WiFiManager::showNetworkList() {
    _networkCount = WiFi.scanNetworks();
    if (_networkCount == 0) {
        Serial.println("\n[WiFi] 未发现任何WiFi信号! 重新扫描中...");
        delay(2000);
        showNetworkList();
        return;
    }

    Serial.printf("\n[WiFi] 发现 %d 个WiFi网络:\n", _networkCount);
    Serial.println("----------------------------------------");
    for (int i = 0; i < _networkCount; i++) {
        Serial.printf(" %2d | %-25s | %d dBm | %s\n",
                      i + 1,
                      WiFi.SSID(i).substring(0, 25).c_str(),
                      WiFi.RSSI(i),
                      (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "开放" : "加密");
    }
    Serial.println("----------------------------------------");
    Serial.print("[WiFi] 请输入要连接的WiFi序号(1-");
    Serial.print(_networkCount);
    Serial.println("): ");
}

// ==================== 处理用户输入 ====================
void WiFiManager::handleInput(const String& input) {
    // ---- 全局命令: disconnect ----
    // 在任意状态下输入 "disconnect" 都会断开当前WiFi并进入配网
    if (input.equalsIgnoreCase("disconnect")) {
        if (_state == WIFI_STATE_CONNECTED) {
            Serial.print("\n[WiFi] 正在断开: ");
            Serial.println(WiFi.SSID());
            WiFi.disconnect(true, false);  // 断开并关闭WiFi模式
            _connectedSSID = "";
            _connectedRSSI = 0;
            Serial.println("[WiFi] 已断开连接\n");
        } else {
            Serial.println("[WiFi] 当前未连接WiFi\n");
        }
        // 扫描并显示网络列表，准备下一次连接
        showNetworkList();
        _state = WIFI_STATE_SELECT;
        return;
    }

    switch (_state) {
        case WIFI_STATE_SELECT:
        {
            int choice = input.toInt();
            if (choice >= 1 && choice <= _networkCount) {
                _selectedNetwork = choice - 1;
                String ssid = WiFi.SSID(_selectedNetwork);

                Serial.print("[WiFi] 您选择了: ");
                Serial.println(ssid);

                if (WiFi.encryptionType(_selectedNetwork) == WIFI_AUTH_OPEN) {
                    Serial.println("[WiFi] 该网络为开放网络，无需密码");
                    _password = "";
                    _state = WIFI_STATE_CONNECT;
                } else {
                    Serial.print("[WiFi] 请输入密码: ");
                    _state = WIFI_STATE_INPUT_PASS;
                }
            } else {
                Serial.print("[WiFi] 输入无效! 请输入1-");
                Serial.print(_networkCount);
                Serial.println("之间的数字:");
            }
        }
        break;

        case WIFI_STATE_INPUT_PASS:
            _password = input;
            _state = WIFI_STATE_CONNECT;
            break;

        default:
            break;
    }
}

// ==================== 执行连接 ====================
void WiFiManager::doConnect() {
    String ssid = WiFi.SSID(_selectedNetwork);
    Serial.println("\n[WiFi] 正在连接...");
    Serial.print("[WiFi] SSID: ");
    Serial.println(ssid);

    WiFi.mode(WIFI_STA);

    if (_password.length() > 0) {
        WiFi.begin(ssid.c_str(), _password.c_str());
    } else {
        WiFi.begin(ssid.c_str());
    }

    int timeout = 30;  // 15秒超时
    while (WiFi.status() != WL_CONNECTED && timeout > 0) {
        delay(500);
        Serial.print(".");
        timeout--;
    }

    Serial.println();

    if (WiFi.status() == WL_CONNECTED) {
        // 连接成功
        _connectedSSID = ssid;
        _connectedRSSI = WiFi.RSSI();
        _state = WIFI_STATE_CONNECTED;

        // 保存到Flash
        _prefs.putString(KEY_SSID, ssid);
        _prefs.putString(KEY_PASSWORD, _password);
        Serial.println("[WiFi] 保存: WiFi信息已保存到Flash");

        Serial.println("\n****************************************");
        Serial.println("           WiFi 连接成功!");
        Serial.println("****************************************");
        showConnectionInfo();
    } else {
        // 连接失败
        _state = WIFI_STATE_FAILED;
        Serial.println("\n****************************************");
        Serial.println("           WiFi 连接失败!");
        Serial.println("****************************************");
        Serial.println("[WiFi] 可能原因:");
        Serial.println("  - 密码错误");
        Serial.println("  - 信号太弱");
        Serial.println("  - 路由器拒绝连接");
        Serial.println("****************************************");
    }
}

// ==================== 显示连接信息 ====================
void WiFiManager::showConnectionInfo() {
    Serial.print("[WiFi] IP地址:    ");
    Serial.println(WiFi.localIP());
    Serial.print("[WiFi] 子网掩码:  ");
    Serial.println(WiFi.subnetMask());
    Serial.print("[WiFi] 网关:      ");
    Serial.println(WiFi.gatewayIP());
    Serial.print("[WiFi] 信号强度:  ");
    Serial.print(WiFi.RSSI());
    Serial.println(" dBm");
    Serial.print("[WiFi] MAC地址:   ");
    Serial.println(WiFi.macAddress());
    Serial.println("****************************************");
}

// ==================== 获取本地IP ====================
String WiFiManager::getLocalIP() const {
    if (_state == WIFI_STATE_CONNECTED) {
        return WiFi.localIP().toString();
    }
    return "未连接";
}
