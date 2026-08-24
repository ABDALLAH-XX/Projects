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
  tone(buzzerPin, 523);  // Play C5 note (523 Hz)
  delay(500);            // Wait for 0.5 second
  tone(buzzerPin, 659);  // Play E5 note (659 Hz)
  delay(500);
  noTone(buzzerPin);
  delay(1000);
}