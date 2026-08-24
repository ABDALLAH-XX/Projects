#include <Arduino.h>

#define BUTTON_PIN 21
#define BUZZER_PIN 18




void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  
}

void loop() {
  // put your main code here, to run repeatedly:
  int buttonState = digitalRead(BUTTON_PIN); 

  if (buttonState == LOW) {
    Serial.println("The button is being pressed");
    //digitalWrite(BUZZER_PIN, HIGH); // turn on
    tone(BUZZER_PIN, 500);
  }
  else {
    if (buttonState == HIGH) {
      Serial.println("The button is released");
      //digitalWrite(BUZZER_PIN, LOW); // turn off
      noTone(BUZZER_PIN);
    }
  }
}
