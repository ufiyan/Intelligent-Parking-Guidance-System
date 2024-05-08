// ABSTRACT :-
// This project develops an Intelligent Parking Guidance System designed to streamline the parking process in small lots using Arduino microcontrollers and ultrasonic sensors. By detecting vehicle presence 
//and guiding drivers with real-time availability via LEDs and an LCD display, the system aims to reduce search time and enhance parking efficiency. Each parking spot features an ultrasonic sensor 
//for occupancy detection, with LEDs indicating space status. A central Arduino aggregates data, guiding drivers to vacant spots and assisting with precise parking alignment, thus improving safety and optimizing
// parking space utilization. This innovative approach leverages basic technology to address parking challenges effectively.​
#include <LiquidCrystal.h>
#include <Servo.h>
#include<SoftwareSerial.h>

SoftwareSerial DELTASerial(13, 7); // RX pin = 13, TX pin = 7
LiquidCrystal lcd(12, 11, 10, 9, 8, 7);
Servo entryBarrier;
const int motorPin = 13;
const int trigPin = 5;
const int echoPin = 6;
int freeSpots = 3;
unsigned long previousMillis = 0;
const long interval = 1000;
String message = "Intelligent Parking Guidance System    ";
int position = 0;
bool carDetected = false;
unsigned long gateOpenMillis = 0;
const long gateOpenDuration = 3000; // Duration for which the gate remains open

char serialBuffer[32]; // Buffer for serial data
int bufferPosition = 0; // Position in buffer

int measureDiff=10000;

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  entryBarrier.attach(motorPin);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  updateLCD();
}

void updateLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(message.substring(0, 16));
  lcd.setCursor(0, 1);
  lcd.print("Free Spots: ");
  lcd.print(freeSpots);
}


void loop() {
  unsigned long currentMillis = millis();

  // Read serial data in a non-blocking manner
  if (Serial.available()) {
        String received = Serial.readStringUntil('\n');
        received.trim();
        Serial.print("Received command: ");
        Serial.println(received);
        if (received == "Spot occupied"){
          if (freeSpots==3){
          freeSpots--;}
          updateLCD();
        }
        if (received == "Spot empty"){
          if (freeSpots==2){
          freeSpots++;}
          updateLCD();
        }

        // if (received == "TESTING 3") {
        //     lcd.setCursor(0, 1);
        //     lcd.print("TEST 3 Received ");
    // }
  }

  // Scrolling text logic
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    if (!carDetected) {
      scrollText();
    }
  }

  // Car detection logic
  checkForCar(currentMillis);
}


void scrollText() {
  if (carDetected) return;

  lcd.setCursor(0, 0);
  lcd.print(message.substring(position, min(position + 16, message.length())) + "                ");
  position++;
  if (position > message.length() - 16) {
    position = 0;
  }
}

void checkForCar(unsigned long currentMillis) {
  long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  if (distance > 0 && distance < 5 && !carDetected) {
    carDetected = true;
    lcd.setCursor(0, 0);
    lcd.print("Gate Opening    ");
    lcd.setCursor(0, 1);
    lcd.print("Please Wait     ");
    entryBarrier.write(90); // Open the gate
    gateOpenMillis = currentMillis;
    Serial.println("Hi hello");  // This line sends the message "Hi hello"
    
  }

  if (carDetected && currentMillis - gateOpenMillis > gateOpenDuration) {
    entryBarrier.write(0); // Close the gate
    carDetected = false;
    updateLCD();
    position = 0;
    scrollText();
 }
}
