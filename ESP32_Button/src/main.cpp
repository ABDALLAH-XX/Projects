#include <Arduino.h>

#define BUTTON_PIN 26

int lastState = HIGH;
const char* messages[] = {"Hello!", "Awesome!", "Keep going!", "ESP32 rocks!",
"Great job!"};


void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  randomSeed(analogRead(0));
}

void loop() {
  int currentState = digitalRead(BUTTON_PIN);
  
  if (lastState == HIGH && currentState == LOW) {
    int index = random(5);
    Serial.println(messages[index]);
  } 
  
  lastState = currentState;
}