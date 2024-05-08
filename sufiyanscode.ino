// ABSTRACT :-
// This project develops an Intelligent Parking Guidance System designed to streamline the parking process in small lots using Arduino microcontrollers and ultrasonic sensors. By detecting vehicle presence 
//and guiding drivers with real-time availability via LEDs and an LCD display, the system aims to reduce search time and enhance parking efficiency. Each parking spot features an ultrasonic sensor 
//for occupancy detection, with LEDs indicating space status. A central Arduino aggregates data, guiding drivers to vacant spots and assisting with precise parking alignment, thus improving safety and optimizing
// parking space utilization. This innovative approach leverages basic technology to address parking challenges effectively.​

#include <Servo.h>

#define TRIGGER_PIN 9
#define ECHO_PIN 10
#define BUZZER_PIN 7
#define RED_LIGHT_PIN 12
#define YELLOW_LIGHT_PIN 13
#define SERVO_PIN 6

unsigned long previousMillis = 0; 
unsigned long servoMillis = 0;
unsigned long yellowLightMillis = 0;
const long interval = 100; // Interval for checking distance
const long yellowInterval = 100; // Interval for yellow light flashing
const long servoInterval = 500; // Interval for servo movement
Servo myservo;  
int angle = 0;  
bool yellowState = false; // State of yellow light

void setup() {
  myservo.attach(SERVO_PIN); 
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(RED_LIGHT_PIN, OUTPUT);
  pinMode(YELLOW_LIGHT_PIN, OUTPUT);
  digitalWrite(RED_LIGHT_PIN, HIGH);
  myservo.write(0);
  Serial.begin(9600); // Initialize serial communication at 9600 baud rate
}

void loop() {
  unsigned long currentMillis = millis();
  float duration, distanceCm, distanceInch;

  // Trigger the ultrasonic sensor
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);

  duration = pulseIn(ECHO_PIN, HIGH);
  distanceCm = (duration / 2) / 29.1;
  distanceInch = distanceCm * 0.393701;

  if (distanceInch <= 6) {
    digitalWrite(RED_LIGHT_PIN, LOW);
    digitalWrite(BUZZER_PIN, HIGH); 
    Serial.println("CRASH"); 
    // Send signal to master Arduino

    // Handle yellow light blinking
    if (currentMillis - yellowLightMillis >= yellowInterval) {
      yellowLightMillis = currentMillis;
      yellowState = !yellowState; // Toggle yellow light state
      digitalWrite(YELLOW_LIGHT_PIN, yellowState ? HIGH : LOW);

    }
  } else {
    digitalWrite(RED_LIGHT_PIN, HIGH);
    digitalWrite(YELLOW_LIGHT_PIN, LOW);
    digitalWrite(BUZZER_PIN, LOW);
    yellowState = false; // Reset yellow light state
  }

  // Servo rotation logic for faster movement
  if (currentMillis - servoMillis >= servoInterval) {
    servoMillis = currentMillis;
    if (angle == 0) {
      myservo.write(90);
      angle = 90;
    } else {
      myservo.write(0);
      angle = 0;
    }
  }
}
