#include <Arduino.h>

#define BUTTON_PIN 26        // GPIO26 pin connected to button
#define LONG_PRESS_TIME 1000 // 1000 milliseconds

// Variables will change:
int lastState = LOW;  // the previous state from the input pin
int currentState;     // the current reading from the input pin
unsigned long pressedTime = 0;
bool isPressing = false;
bool isLongDetected = false;

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
}

void loop() {
  // read the state of the switch/button
  currentState = digitalRead(BUTTON_PIN);

  if (lastState == HIGH && currentState == LOW) {
    pressedTime = millis();
    isPressing = true;
    isLongDetected = false;
  } else if (lastState == LOW && currentState == HIGH) {
    isPressing = false;
  }
  
  if (isPressing == true && isLongDetected == false){
    long pressDuration = millis() - pressedTime;

    if (pressDuration > LONG_PRESS_TIME){
      Serial.println("A long press is detected");
      isLongDetected = true;
    }
  }

  // save the last state
  lastState = currentState;
}