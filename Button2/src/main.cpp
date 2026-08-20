#include <Arduino.h>

#define RED_BUTTON 27
#define BLUE_BUTTON 25

int lastState1 = HIGH;
int lastState2 = HIGH;

void setup() {
  // put your setup code here, to run once:
  // initialize the pushbutton as an pull-up input
  pinMode(RED_BUTTON, INPUT_PULLUP);
  pinMode(BLUE_BUTTON, INPUT_PULLUP);
  Serial.begin(115200);
}

void loop() {
  int currentState1 = digitalRead(RED_BUTTON);
  int currentState2 = digitalRead(BLUE_BUTTON);


  if (currentState1 != lastState1) {
    if (currentState1 == LOW) {
      Serial.println("Bouton rouge presse !");
    } else {
      Serial.println("Bouton rouge relache !");
    }
    lastState1 = currentState1;
  }

  if (currentState2 != lastState2) {
    if (currentState2 == LOW) {
      Serial.println("Bouton bleu presse !");
    } else {
      Serial.println("Bouton bleu relache !");
    }
    lastState2 = currentState2;
  }
  
  delay(10);
}
