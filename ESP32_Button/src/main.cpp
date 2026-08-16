#include <Arduino.h>

#define BUTTON_PIN 26

int lastState = HIGH;
int currentState;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50; // 50ms debounce


void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.println("=== Button with Debouncing ===");
  Serial.println("Press the button - no false triggers!");
}

void loop() {
  int reading = digitalRead(BUTTON_PIN);
  
  // Check if state changed
  if (reading != lastState) {
    lastDebounceTime = millis();
  }
  
  // Wait for debounce time
  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != currentState) {
      currentState = reading;

      if (currentState == LOW) {
        Serial.println("Button PRESSED (debounced)!");
      }
    }
  }
  
  lastState = reading;
}