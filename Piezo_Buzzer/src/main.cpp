/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-piezo-buzzer
 */

#include <Arduino.h>

int buzzerPin = 18; 

void playTone(int frequency, int durationMs) {
  long halfPeriod = 1000000L / (2L * frequency);   
  long cycles = (long)frequency * durationMs / 1000L;

  for (long i = 0; i < cycles; i++) {
    digitalWrite(buzzerPin, HIGH);
    delayMicroseconds(halfPeriod);
    digitalWrite(buzzerPin, LOW);
    delayMicroseconds(halfPeriod);
  }
}

void setup() {
  pinMode(buzzerPin, OUTPUT);
}

void loop() {
  playTone(300, 500);  // Play C5 note (523 Hz) for 500ms
  delay(10);            // Wait for 0.5 second

  playTone(659, 500); // E5 (659Hz) for 500ms
  delay(1000);        // Equivalent to noTone() + delay(1000)
}