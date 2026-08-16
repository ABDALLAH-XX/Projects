#include <Arduino.h>

#define BUTTON_PIN 26        // GPIO26 pin connected to button
#define SHORT_PRESS_TIME 500 // 500 milliseconds

// Variables will change:
int lastState = LOW;  // the previous state from the input pin
int currentState;     // the current reading from the input pin
unsigned long pressedTime = 0;
unsigned long releasedTime = 0;

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  // read the state of the switch/button
  currentState = digitalRead(BUTTON_PIN);

  if (lastState == HIGH && currentState == LOW)
    pressedTime = millis();
  else if (lastState == LOW && currentState == HIGH) {
    releasedTime = millis();

    long pressDuration = releasedTime - pressedTime;

    if (pressDuration < SHORT_PRESS_TIME)
      Serial.println("A short press is detected");
  }

  // save the last state
  lastState = currentState;
}