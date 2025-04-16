# IoT Weather Monitoring System with ESP32

This project is an *IoT-based Weather Monitoring System* using an *ESP32 microcontroller. It monitors environmental conditions such as **temperature, humidity, smoke, rain, and **soil moisture, and provides real-time data through an **on-device LCD* and a *web-based dashboard*.

## Features

- Displays sensor data on a 16x2 I2C LCD
- Protects data access with a 4-digit keypad password
- Shows temperature, humidity, smoke, rain, and soil moisture levels
- Web dashboard (hosted locally on ESP32) auto-refreshes every second
- ESP32 runs in Access Point mode (no external WiFi required)
- Password can be reset by pressing '5' twice quickly on keypad

## Hardware Used

- ESP32 Dev Board
- DHT11 Temperature and Humidity Sensor
- MQ2 Smoke Sensor
- Rain Sensor
- Soil Moisture Sensor
- 16x2 I2C LCD Display
- 4x4 Keypad
- Buzzer (for alerts)
- Jumper wires, breadboard, etc.

## Circuit Overview

| Component           | Pin Connection       |
|---------------------|----------------------|
| DHT11               | GPIO 5               |
| MQ2 (Digital)       | GPIO 4               |
| Rain Sensor         | GPIO 18              |
| Soil Moisture       | GPIO 19 (Analog)     |
| Buzzer              | GPIO 23              |
| Keypad Rows         | GPIOs 12, 14, 27, 26 |
| Keypad Columns      | GPIOs 25, 33, 32, 35 |
| LCD I2C Address     | 0x27 (16x2 screen) |

## How It Works

1. *Startup*: ESP32 creates a WiFi access point named ESP32-Network with password 12345678.
2. *LCD* shows IP address and prompts for a *4-digit password*.
3. *Keypad* input checks password (default is 2233). Access is granted upon correct entry.
4. *Dashboard*: After logging in, the LCD rotates through:
   - Temperature & Humidity
   - Smoke Status
   - Rain Status
   - Soil Moisture
5. *Web Access*: Connect to ESP32’s WiFi and visit 192.168.4.1 in a browser to see the live sensor dashboard.

## Setup Instructions

1. *Clone this repo*:
    bash
    git clone https://github.com/Omar-Sa6ry/Weather_Monitoring_System-IOT-/blob/main/main.c%2B%2B
   
3. **Open the .ino file** in Arduino IDE.
4. *Install required libraries*:
   - WiFi.h
   - WebServer.h
   - DHT.h
   - LiquidCrystal_I2C.h
   - Keypad.h
5. *Select Board*: ESP32 Dev Module
6. *Upload the sketch* to your ESP32 board
7. *Connect to ESP32 WiFi*, open browser to 192.168.4.1

## Security

- Default password is 2233
- You can change it in the code: const String correctPassword = "2233";
- Press 5 twice quickly to *log out and re-enter password*

