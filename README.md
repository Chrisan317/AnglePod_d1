# AnglePod_d1 - Tilt Meter

[English](README_EN.md) | 中文

一個基於 **Wemos D1 Mini** 的數字傾斜計量表，使用 **LSM6DS3** 加速度計和 **OLED 顯示屏** 實時顯示傾斜角度。

## 功能特性

- 📐 **實時傾斜角度測量** - 顯示 Roll（滾轉）和 Pitch（俯仰）角度
- 🌡️ **溫度監測** - 顯示 IMU 感測器溫度
- 💧 **水平檢測** - 當傾斜角度小於 2° 時顯示 "LEVEL" 提示
- 🔧 **自動校準** - 上電自動校準加速度計偏移
- 📊 **低通濾波** - 平滑數據輸出，減少噪聲
- 📱 **串列埠輸出** - 實時數據串列輸出便於除錯

## 硬件要求

- **微控制器**: Wemos D1 Mini (ESP8266)
- **IMU 感測器**: SparkFun LSM6DS3 (I2C)
  - I2C 地址: `0x6B` (SA0 接 3.3V pull high)
- **顯示屏**: Wemos Mini OLED Shield (64×48 pixels)
- **I2C 連接**:
  - SDA: D2 (GPIO4)
  - SCL: D1 (GPIO5)

## 軟件依賴

在 `platformio.ini` 中引用的庫：

```
SparkFunLSM6DS3
Adafruit_SSD1306
```

安裝方式：
```bash
platformio lib install "SparkFunLSM6DS3"
platformio lib install "Adafruit_SSD1306"
```

## 接線配置

| 元件 | 連接 |
|------|------|
| LSM6DS3 SDA | Wemos D2 (GPIO4) |
| LSM6DS3 SCL | Wemos D1 (GPIO5) |
| LSM6DS3 GND | GND |
| LSM6DS3 VCC | 3.3V |
| OLED SDA | Wemos D2 (GPIO4) |
| OLED SCL | Wemos D1 (GPIO5) |
| OLED GND | GND |
| OLED VCC | 3.3V |

## 使用說明

### 編譯和上傳

```bash
platformio run --target upload
```

### 校準程序

1. 上電後會顯示 "Calibrating - Keep flat!" 訊息
2. 將設備放在**完全平坦**的水平面上（不要移動）
3. 待校準完成後自動進入測量模式
4. 串列埠會輸出校準偏移值

### OLED 顯示說明

屏幕分為三個數據區域：

```
ROLL
  xx.x °

PITCH
  xx.x °

T: xx.x C  或  **LEVEL**
```

- **ROLL**: 左右傾斜角度（-180° 至 +180°）
- **PITCH**: 前後傾斜角度（-90° 至 +90°）
- **TEMP**: 傳感器溫度，或當水平時顯示 **LEVEL** 提示

### 串列埠輸出

波特率: **115200 bps**

輸出格式：
```
Roll:xx.x Pitch:xx.x Temp:xx.xC
```

## 參數調整

在 `src/main.cpp` 中可調整以下參數：

| 參數 | 默認值 | 說明 |
|------|--------|------|
| `CALIB_SAMPLES` | 200 | 校準樣本數 |
| `ALPHA` | 0.15 | 低通濾波係數 (0.0=極平滑, 1.0=無濾波) |
| `isLevel` 閾值 | 2.0° | 水平檢測判定角度 |
| 更新速率 | 50ms (~20Hz) | 延遲時間（毫秒） |

## 故障排除

### "LSM6DS3 ERROR!" 訊息
- 檢查 I2C 接線
- 確認 LSM6DS3 地址為 `0x6B`（SA0 應接 GND）
- 檢查電源供應

### 角度讀數不準確
- 重新執行校準程序
- 確保設備初始化時完全平放
- 調整 `ALPHA` 參數改變濾波程度

### OLED 不顯示
- 檢查 I2C 連線
- 確認 Wemos Mini OLED 庫已正確安裝

## 許可證

MIT License

## 作者

AnglePod Project
