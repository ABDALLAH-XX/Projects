/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-piezo-buzzer
 */

#include <Arduino.h>

int buzzerPin = 18; 

void setup() {
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  digitalWrite(buzzerPin, HIGH);  // Turn buzzer on
  delay(1000);                    // Wait for 1 second
  digitalWrite(buzzerPin, LOW);   // Turn buzzer off
  delay(1000);                    // Wait for 1 second
}