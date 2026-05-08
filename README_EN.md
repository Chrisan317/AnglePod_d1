# AnglePod_d1 - Tilt Meter

English | [中文](README.md)

A digital tilt meter based on **Wemos D1 Mini**, using **LSM6DS3** accelerometer and **OLED display** to measure and display tilt angles in real-time.

## Features

- 📐 **Real-time Tilt Measurement** - Display Roll and Pitch angles
- 🌡️ **Temperature Monitoring** - Display IMU sensor temperature
- 💧 **Level Detection** - Show "LEVEL" indicator when tilt angle is below 2°
- 🔧 **Auto Calibration** - Automatic accelerometer offset calibration on power-up
- 📊 **Low-Pass Filtering** - Smooth data output to reduce noise
- 📱 **Serial Output** - Real-time data output for debugging

## Hardware Requirements

- **Microcontroller**: Wemos D1 Mini (ESP8266)
- **IMU Sensor**: SparkFun LSM6DS3 (I2C)
  - I2C Address: `0x6B` (SA0 pulled to 3.3V)
- **Display**: Wemos Mini OLED Shield (64×48 pixels)
- **I2C Connection**:
  - SDA: D2 (GPIO4)
  - SCL: D1 (GPIO5)

## Software Dependencies

Libraries referenced in `platformio.ini`:

```
SparkFunLSM6DS3
Adafruit_SSD1306
```

Installation:
```bash
platformio lib install "SparkFunLSM6DS3"
platformio lib install "Adafruit_SSD1306"
```

## Wiring Configuration

| Component | Connection |
|-----------|------------|
| LSM6DS3 SDA | Wemos D2 (GPIO4) |
| LSM6DS3 SCL | Wemos D1 (GPIO5) |
| LSM6DS3 GND | GND |
| LSM6DS3 VCC | 3.3V |
| OLED SDA | Wemos D2 (GPIO4) |
| OLED SCL | Wemos D1 (GPIO5) |
| OLED GND | GND |
| OLED VCC | 3.3V |

## Usage Instructions

### Build and Upload

```bash
platformio run --target upload
```

### Calibration Procedure

1. After power-up, "Calibrating - Keep flat!" message appears on screen
2. Place device on a **completely level** horizontal surface (do not move)
3. Wait for calibration to complete, then automatic measurement mode starts
4. Serial port will output calibration offset values

### OLED Display Information

Screen divided into three data areas:

```
ROLL
  xx.x °

PITCH
  xx.x °

T: xx.x C  or  **LEVEL**
```

- **ROLL**: Left-right tilt angle (-180° to +180°)
- **PITCH**: Front-back tilt angle (-90° to +90°)
- **TEMP**: Sensor temperature, or **LEVEL** indicator when level

### Serial Output

Baud Rate: **115200 bps**

Output Format:
```
Roll:xx.x Pitch:xx.x Temp:xx.xC
```

## Parameter Adjustment

The following parameters in `src/main.cpp` can be adjusted:

| Parameter | Default | Description |
|-----------|---------|-------------|
| `CALIB_SAMPLES` | 200 | Number of calibration samples |
| `ALPHA` | 0.15 | Low-pass filter coefficient (0.0=very smooth, 1.0=no filter) |
| `isLevel` threshold | 2.0° | Level detection angle threshold |
| Update rate | 50ms (~20Hz) | Delay time in milliseconds |

## Troubleshooting

### "LSM6DS3 ERROR!" Message
- Check I2C wiring
- Confirm LSM6DS3 address is `0x6B` (SA0 should be pulled to 3.3V)
- Check power supply

### Inaccurate Angle Readings
- Re-run calibration procedure
- Ensure device is completely level during initialization
- Adjust `ALPHA` parameter to change filtering level

### OLED Not Displaying
- Check I2C connections
- Confirm Wemos Mini OLED library is properly installed

## License

MIT License

## Author

AnglePod Project
