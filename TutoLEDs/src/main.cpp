#include <Arduino.h>

#define LED1 2

void setup() {
  // put your setup code here, to run once:
  pinMode(LED1, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED1, HIGH); // turn the LED on (HIGH is the voltage level)
  Serial.println("LED HIGH");
  delay(1000); //wait for a second
  digitalWrite(LED1, LOW); //turn the LED off by making the voltage low
  Serial.println("LED LOW");
  delay(1000); //wait for a second
}