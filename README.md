# RFID Access Control System (Servo + LCD + LEDs + Buzzer)

## 📌 Description
This Arduino project is an **RFID-based access control system** that uses an RC522 RFID reader to authenticate users. Authorized cards unlock a servo-controlled door, display messages on an I2C LCD, and play buzzer tones. Unauthorized access attempts trigger alerts.

## 🧰 Components Used
- Arduino Uno / Nano
- MFRC522 RFID Reader
- RFID Card / Tag
- Servo Motor
- 16x2 I2C LCD (0x27)
- Red, Yellow, Green LEDs
- Buzzer
- Jumper wires
- Breadboard

## 🔌 Pin Connections
| Component | Arduino Pin |
|--------|-------------|
| RFID SDA | D10 |
| RFID RST | D9 |
| Servo | D6 |
| Buzzer | D7 |
| Red LED | D3 |
| Yellow LED | D4 |
| Green LED | D5 |
| LCD SDA | A4 |
| LCD SCL | A5 |

## ⚙️ System Features
- RFID card authentication
- Servo motor door lock/unlock
- LCD feedback messages
- LED status indicators
- Buzzer sound feedback
- Alert mode after 3 failed attempts
- Non-blocking servo auto-close

## 📟 How It Works
1. System initializes and waits for RFID card
2. Authorized card:
   - Green LED ON
   - Servo unlocks
   - Welcome message displayed
   - Buzzer success tone
3. Unauthorized card:
   - Red LED blinks
   - Buzzer alert
4. After 3 failed attempts:
   - Alert/Danger mode activated

## 📚 Required Libraries
- SPI
- MFRC522
- LiquidCrystal_I2C
- Servo
- Wire

## 🚀 How to Use
1. Install required libraries in Arduino IDE
2. Upload `servobuzzer_RFIDLEDLCD.ino`
3. Scan registered RFID card
4. Observe LCD, LEDs, servo, and buzzer responses
