#include <Arduino.h>
#include <Wire.h>
#include <SparkFunLSM6DS3.h>
#include <Adafruit_SSD1306.h>
#include <math.h>

// ──────────────────────────────────────────
//  OLED 設定（Wemos Mini OLED Shield 64×48）
//  stblassitude 庫已內建 offset 修正，
//  初始化用 display.begin() 不需傳地址
// ──────────────────────────────────────────
#define OLED_RESET  -1

// ──────────────────────────────────────────
//  I2C 腳位（D1 Mini 固定：D2=SDA, D1=SCL）
//  LSM6DS3 I2C 地址：0x6A（SA0 接 GND）
// ──────────────────────────────────────────
#define LSM6DS3_ADDR  0x6B

// ──────────────────────────────────────────
//  校準樣本數
// ──────────────────────────────────────────
#define CALIB_SAMPLES  200

LSM6DS3 imu(I2C_MODE, LSM6DS3_ADDR);
Adafruit_SSD1306 display(OLED_RESET);   // Wemos Mini OLED 庫不需傳尺寸

// 校準偏移量
float offsetAx = 0, offsetAy = 0, offsetAz = 0;

// ──────────────────────────────────────────
//  低通濾波器
// ──────────────────────────────────────────
float filteredRoll  = 0;
float filteredPitch = 0;
const float ALPHA   = 0.15f;   // 0.0=極平滑  1.0=無濾波

// 前向宣告
void calibrateIMU();
void showError(const char* line1, const char* line2);

// ══════════════════════════════════════════
//  SETUP
// ══════════════════════════════════════════
void setup() {
    Serial.begin(115200);
    delay(300);

    Wire.begin();   // D1 Mini 固定 D2=SDA, D1=SCL

    // ── 初始化 OLED ──
    // Wemos Mini OLED 庫已內建 64×48 offset，直接 begin() 即可
    display.begin();
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println("Tilt Meter");
    display.println("  v2.0");
    display.println();
    display.println("Init...");
    display.display();

    // ── 初始化 LSM6DS3 ──
    if (imu.begin() != 0) {
        Serial.println("[ERROR] LSM6DS3 init failed");
        showError("LSM6DS3", "ERROR!");
        while (true) delay(1000);
    }

    delay(200);

    // ── 校準畫面 ──
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("Calibrating");
    display.println("Keep flat!");
    display.display();

    calibrateIMU();

    Serial.println("[OK] Ready");
    Serial.print("Offset Ax="); Serial.print(offsetAx, 4);
    Serial.print(" Ay="); Serial.print(offsetAy, 4);
    Serial.print(" Az="); Serial.println(offsetAz, 4);

    delay(400);
}

// ══════════════════════════════════════════
//  LOOP
// ══════════════════════════════════════════
void loop() {
    // 讀取加速度（單位已是 g，平放時 Az ≈ 1.0）
    float ax = imu.readFloatAccelX() - offsetAx;
    float ay = imu.readFloatAccelY() - offsetAy;
    float az = imu.readFloatAccelZ() - offsetAz;

    // 讀取溫度（°C）
    float temp = imu.readTempC();

    // 計算角度
    float roll  = atan2f(ay, az) * 180.0f / PI;
    float pitch = atan2f(-ax, sqrtf(ay * ay + az * az)) * 180.0f / PI;

    // 低通濾波
    filteredRoll  = ALPHA * roll  + (1.0f - ALPHA) * filteredRoll;
    filteredPitch = ALPHA * pitch + (1.0f - ALPHA) * filteredPitch;

    bool isLevel = (fabsf(filteredRoll) < 2.0f && fabsf(filteredPitch) < 2.0f);

    // ── 序列埠輸出 ──
    Serial.print("Roll:"); Serial.print(filteredRoll, 1);
    Serial.print(" Pitch:"); Serial.print(filteredPitch, 1);
    Serial.print(" Temp:"); Serial.print(temp, 1);
    Serial.println("C");

    // ── OLED 顯示（64×48，每行 8px 高，共 6 行）──
    //
    //  行0: ROLL
    //  行1:  xx.x °
    //  行2: PITCH
    //  行3:  xx.x °
    //  行4: TEMP
    //  行5:  xx.x C  / ** LEVEL **
    //
    display.clearDisplay();
    display.setTextSize(1);   // 每字元 6×8 px，64px 寬可放 10 個字

    // ROLL
    display.setCursor(0, 0);
    display.print("ROLL");
    display.setCursor(0, 9);
    display.print(" ");
    display.print(filteredRoll, 1);
    display.print((char)247);   // ° 符號

    // PITCH
    display.setCursor(0, 20);
    display.print("PITCH");
    display.setCursor(0, 29);
    display.print(" ");
    display.print(filteredPitch, 1);
    display.print((char)247);

    // TEMP / LEVEL
    display.setCursor(0, 40);
    if (isLevel) {
        display.print("**LEVEL**");
    } else {
        display.print("T:");
        display.print(temp, 1);
        display.print("C");
    }

    display.display();

    delay(50);   // ~20 Hz
}

// ══════════════════════════════════════════
//  校準（靜止平放，取加速度平均偏移）
// ══════════════════════════════════════════
void calibrateIMU() {
    double sumAx = 0, sumAy = 0, sumAz = 0;
    for (int i = 0; i < CALIB_SAMPLES; i++) {
        sumAx += imu.readFloatAccelX();
        sumAy += imu.readFloatAccelY();
        sumAz += imu.readFloatAccelZ();
        delay(5);
    }
    offsetAx = sumAx / CALIB_SAMPLES;
    offsetAy = sumAy / CALIB_SAMPLES;
    // Z 軸扣除重力 1g
    offsetAz = sumAz / CALIB_SAMPLES - 1.0f;
}

// ══════════════════════════════════════════
//  OLED 錯誤訊息
// ══════════════════════════════════════════
void showError(const char* line1, const char* line2) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 12);
    display.println(line1);
    display.setCursor(0, 24);
    display.println(line2);
    display.display();
}