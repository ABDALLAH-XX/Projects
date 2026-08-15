#include <Arduino.h>

#define BUTTON_PIN 26

int pressCount = 0;
int lastState = HIGH;

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.println("Button Counter Started");
}

void loop() {
  int currentState = digitalRead(BUTTON_PIN);
  
  if (lastState == HIGH && currentState == LOW) {
    pressCount++;
    Serial.print("Press Button: ");
    Serial.print(pressCount);
    Serial.println(" - Button PRESSED!");
  } 
  
  lastState = currentState;
}