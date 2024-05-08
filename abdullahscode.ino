#include <LiquidCrystal.h>
#include <Servo.h>

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
Servo entryBarrier;
const int motorPin = 8;
const int trigPin = 9;
const int echoPin = 10;
volatile bool pulseSent = false;
unsigned long pulseStartTime = 0;
const long pulseDuration = 10;  // Pulse duration in microseconds as a variable for tracking
bool gateOpen = false;
unsigned long gateOpenMillis = 0;
const long gateOpenDuration = 3000; // Duration for which the gate remains open

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  entryBarrier.attach(motorPin);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  updateLCD("THANK U");
}

void updateLCD(String message) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(message);
}

void sendPulse() {
  digitalWrite(trigPin, HIGH);
  pulseStartTime = micros();
  pulseSent = true;
}

void loop() {
  unsigned long currentMillis = millis();
  unsigned long currentMicros = micros();

  if (Serial.available()) {
    String received = Serial.readStringUntil('\n');
    received.trim();
    if (received == "CRASH") {
      updateLCD("HIGH TRAFFIC");
      return;
    }
  }

  if (!pulseSent) {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    sendPulse();
  } else if (pulseSent && (currentMicros - pulseStartTime >= pulseDuration)) {
    digitalWrite(trigPin, LOW);
    pulseSent = false;

    // Measure the distance immediately after sending pulse
    unsigned long duration = pulseIn(echoPin, HIGH);
    int distance = duration * 0.034 / 2;

    if (distance <= 5 && !gateOpen) {
      entryBarrier.write(90);
      gateOpenMillis = currentMillis;
      gateOpen = true;
    }
    if (gateOpen && currentMillis - gateOpenMillis >= gateOpenDuration) {
      entryBarrier.write(0);
      gateOpen = false;
      updateLCD("THANK U");
    }
  }
}
