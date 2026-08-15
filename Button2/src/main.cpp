#include <Arduino.h>

#define BUTTON_PIN 21

int lastState = LOW; //previous state from the input
int currentState; //current reading from the input


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  // initialize the pushbutton as an pull-up input
  pinMode(BUTTON_PIN, INPUT_PULLDOWN);
}

void loop() {
  // put your main code here, to run repeatedly:
  currentState = digitalRead(BUTTON_PIN);

  if(lastState == HIGH && currentState == LOW)
    Serial.println("the state changed from HIGH to LOW");
    
  // save the last state
  lastState = currentState;
}
