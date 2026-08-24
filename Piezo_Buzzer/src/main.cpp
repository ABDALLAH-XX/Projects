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
  tone(buzzerPin, 1000);  // Play 1000 Hz tone
  delay(1000);            // Wait for 1 second
  noTone(buzzerPin);      // Stop the tone
  delay(1000);            // Wait for 1 second before repeating                   // Wait for 1 second
}