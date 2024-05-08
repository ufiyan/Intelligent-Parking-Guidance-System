// ABSTRACT :-
// This project develops an Intelligent Parking Guidance System designed to streamline the parking process in small lots using Arduino microcontrollers and ultrasonic sensors. By detecting vehicle presence 
//and guiding drivers with real-time availability via LEDs and an LCD display, the system aims to reduce search time and enhance parking efficiency. Each parking spot features an ultrasonic sensor 
//for occupancy detection, with LEDs indicating space status. A central Arduino aggregates data, guiding drivers to vacant spots and assisting with precise parking alignment, thus improving safety and optimizing
// parking space utilization. This innovative approach leverages basic technology to address parking challenges effectively.​
#include <LiquidCrystal.h>

// Initialize the LCD library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
#define trigPin 9
#define echoPin 10
#define redLED 7
#define yellowLED 8
#define greenLED 6
#define buzzer 13

unsigned long previousMillis = 0;
unsigned long previousSensorMillis = 0;
unsigned long interval = 500; // Interval at which to blink (milliseconds)
unsigned long sensorInterval = 50; // Interval at which to read the sensor

int spotsOccupied = 0;
int spotsEmpty = 0;

String currentStatus = "";
String lastStatus = "";

void setup() {
  lcd.begin(16, 2);
  pinMode(redLED, OUTPUT);
  pinMode(yellowLED, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Serial.begin(9600);
}

void loop() {
    unsigned long currentMillis = millis();

    if (Serial.available()) {
        String received = Serial.readStringUntil('\n');
        received.trim();
        Serial.print("Received command: ");
        Serial.println(received);

        // if (received == "TESTING 3") {
        //     lcd.setCursor(0, 1);
        //     lcd.print("TEST 3 Received ");
        // }
    }

    if (currentMillis - previousSensorMillis >= sensorInterval) {
        previousSensorMillis = currentMillis;
        measureDistance();
    }

    // Flashing yellow LED logic for "OBJECT MOVING"
    if (currentStatus == "OBJECT MOVING" && (currentMillis - previousMillis >= interval)) {
        previousMillis = currentMillis; // Save the last time we toggled the LED
        digitalWrite(yellowLED, !digitalRead(yellowLED)); // Toggle LED state
    } else if (currentStatus != "OBJECT MOVING") {
        digitalWrite(yellowLED, LOW); // Make sure it's off if not in the correct state
    }
}

void measureDistance() {
    // Trigger the ultrasonic sensor
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);//modify the message being sent to the main use millis to calculate 10 seconds for parking occupancy and if spot is occupied send message to the control unit 
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    long duration = pulseIn(echoPin, HIGH);
    long distance = (duration / 2) / 30; // Calculate the distance in cm

    updateStatus(distance);
}

void updateStatus(long distance) {
    // Determine the current status based on the distance
    if (distance < 4) { // Less than 4 cm
        currentStatus = "TOO CLOSE";
        spotsOccupied = 1;
        spotsEmpty = 0;
        digitalWrite(buzzer, HIGH);
    } else {
        digitalWrite(buzzer, LOW); // Buzzer off when not too close
        if (distance >= 4 && distance < 8) {
            currentStatus = "PERFECT POSITION";
            spotsOccupied = 1;
            spotsEmpty = 0;
            Serial.println("Spot occupied");
        } else if (distance >= 8 && distance < 12) {
            currentStatus = "OBJECT MOVING";
            spotsOccupied = 0;
            spotsEmpty = 0;
        } else if (distance >= 12) {
            currentStatus = "EMPTY SPOT";
            spotsOccupied = 0;
            spotsEmpty = 1;
            Serial.println("Spot empty");
        }
    }

    // Update the LCD only if the status has changed
    if (currentStatus != lastStatus) {
        lastStatus = currentStatus;
        lcd.clear();
        lcd.setCursor(0, 1);
        lcd.print(currentStatus);
    }

    // Always display the distance on the first line
    lcd.setCursor(0, 0);
    lcd.print("Dist: ");
    lcd.print(distance);
    lcd.print(" cm   "); // Clear the previous text with extra spaces

    // LED logic
    digitalWrite(redLED, currentStatus == "PERFECT POSITION");
    digitalWrite(greenLED, currentStatus == "EMPTY SPOT");
}
