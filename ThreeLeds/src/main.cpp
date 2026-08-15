#include <Arduino.h>

#define LED1 12
#define LED2 13
#define LED3 14

void setup() {
  // put your setup code here, to run once:
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED1, HIGH); // turn the LED on (HIGH is the voltage level)
  digitalWrite(LED2, LOW); // turn the LED on (HIGH is the voltage level)
  digitalWrite(LED3, LOW); // turn the LED on (HIGH is the voltage level)
  Serial.println("LED1 HIGH LED2 LOW LED3 LOW");
  delay(1000); //wait for a second
  digitalWrite(LED1, LOW); //turn the LED off by making the voltage low
  digitalWrite(LED2, HIGH); // turn the LED on (HIGH is the voltage level)
  digitalWrite(LED3, LOW); // turn the LED on (HIGH is the voltage level)
  Serial.println("LED1 LOW LED2 HIGH LED3 LOW");
  delay(1000); //wait for a second
  digitalWrite(LED1, LOW); //turn the LED off by making the voltage low
  digitalWrite(LED2, LOW); // turn the LED on (HIGH is the voltage level)
  digitalWrite(LED3, HIGH); // turn the LED on (HIGH is the voltage level)
  Serial.println("LED1 LOW LED2 LOW LED3 HIGH");
  delay(1000);
}

