#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

#define SS_PIN 10
#define RST_PIN 9
MFRC522 rfid(SS_PIN, RST_PIN);
Servo myServo;
LiquidCrystal_I2C lcd(0x27, 16, 2);

// https://github.com/AjayaPhysics/Arduino-RFID-Access-Control-with-Servo-LCD-LEDs-and-Buzzer-using-Arduino---Full-Tutorial

const int redLED = 3;
const int yellowLED = 4; // Always ON as system status
const int greenLED = 5;
const int buzzer = 7;
const int servoPin = 6;

const int openAngle = 90;
const int closeAngle = 0;

const byte authorizedCards[][4] = {
  //{0x23, 0xE2, 0x1C, 0xE5}
  {0x24, 0x11, 0xC0, 0xA3}
};
const int numAuthorized = sizeof(authorizedCards) / 4;

String welcomeMessages[] = {
  "What's up Mah Nigga!",
  "Access Granted",
  "Good Day!",
  "Entry Approved",
  "Welcome Back!"
};

int failCount = 0;
bool doorOpen = false;
unsigned long eventTimer = 0;
unsigned long buzzerTimer = 0;
bool buzzerOn = false;
int buzzerStage = 0;
void showMessage(String line1, String line2 = "");

void setup() {
  Serial.begin(9600);
  SPI.begin();
  rfid.PCD_Init();
  lcd.init();
  lcd.begin(16, 2);
  lcd.backlight();

  pinMode(redLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(servoPin, OUTPUT);

  myServo.attach(servoPin);
  myServo.write(closeAngle);

  digitalWrite(yellowLED, HIGH); // System ready light
  allLEDsOff();
  bootSequence();
  showMessage("System Ready", "Scan Your Card");
}

void loop() {
  handleDoorAutoClose();
  handleBuzzerSequence();

  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) return;

  byte scannedUID[4];
  for (byte i = 0; i < 4; i++) scannedUID[i] = rfid.uid.uidByte[i];

  Serial.print("Scanned UID: ");
  for (byte i = 0; i < 4; i++) {
    Serial.print(scannedUID[i], HEX); Serial.print(" ");
  }
  Serial.println();

  if (isAuthorized(scannedUID)) {
    grantAccess();
    failCount = 0;
  } else {
    failCount++;
    denyAccess();
    if (failCount >= 3) {
      dangerMode();
      failCount = 0;
    }
  }

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
}

// --------------- Access Control Functions ----------------

bool isAuthorized(byte *uid) {
  for (int i = 0; i < numAuthorized; i++) {
    bool match = true;
    for (int j = 0; j < 4; j++) {
      if (uid[j] != authorizedCards[i][j]) {
        match = false; break;
      }
    }
    if (match) return true;
  }
  return false;
}

void grantAccess() {
  allLEDsOff();
  digitalWrite(greenLED, HIGH);
  myServo.write(openAngle);
  doorOpen = true;
  eventTimer = millis();

  String message = welcomeMessages[random(0, sizeof(welcomeMessages) / sizeof(welcomeMessages[0]))];
  showMessage(message, "");

  buzzerStage = 1; // Start buzzer sequence
  buzzerTimer = millis();
}

void denyAccess() {
  allLEDsOff();
  showMessage("Access Denied!", "");
  for (int i = 0; i < 3; i++) {
    digitalWrite(redLED, HIGH);
    tone(buzzer, 400);
    delay(100);
    digitalWrite(redLED, LOW);
    noTone(buzzer);
    delay(100);
  }
  showMessage("Try Again...", "");
}

void dangerMode() {
  showMessage("!! ALERT MODE !!", "");
  for (int i = 0; i < 5; i++) {
    digitalWrite(redLED, HIGH);
    digitalWrite(yellowLED, LOW);
    tone(buzzer, 300);
    delay(150);
    digitalWrite(redLED, LOW);
    digitalWrite(yellowLED, HIGH);
    noTone(buzzer);
    delay(150);
  }
  showMessage("System Ready", "Scan Card");
}

// --------------- Utility Functions ----------------

void allLEDsOff() {
  digitalWrite(redLED, LOW);
  digitalWrite(greenLED, LOW);
}

void showMessage(String line1, String line2 = "") {
  lcd.clear();
  lcd.setCursor(0, 0); lcd.print(line1);
  lcd.setCursor(0, 1); lcd.print(line2);
}

void bootSequence() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");
  for (int i = 0; i < 2; i++) {
    digitalWrite(greenLED, HIGH); delay(100);
    digitalWrite(redLED, HIGH); delay(100);
    allLEDsOff(); delay(100);
  }
}

// --------------- Non-blocking Event Handlers ----------------

void handleDoorAutoClose() {
  if (doorOpen && millis() - eventTimer >= 3000) {
    myServo.write(closeAngle);
    digitalWrite(greenLED, LOW);
    doorOpen = false;
    showMessage("Scan Again", "");
  }
}

void handleBuzzerSequence() {
  unsigned long now = millis();
  if (buzzerStage == 0) return;

  if (buzzerStage == 1 && now - buzzerTimer >= 0) {
    tone(buzzer, 1000);
    buzzerStage++;
    buzzerTimer = now;
  } else if (buzzerStage == 2 && now - buzzerTimer >= 100) {
    tone(buzzer, 1400);
    buzzerStage++;
    buzzerTimer = now;
  } else if (buzzerStage == 3 && now - buzzerTimer >= 100) {
    tone(buzzer, 1800);
    buzzerStage++;
    buzzerTimer = now;
  } else if (buzzerStage == 4 && now - buzzerTimer >= 100) {
    noTone(buzzer);
    buzzerStage = 0;
  }
}
