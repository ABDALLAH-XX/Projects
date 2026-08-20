#include <Arduino.h>

#define RED_BUTTON 27
#define BLUE_BUTTON 26


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  // initialize the pushbutton as an pull-up input
  pinMode(RED_BUTTON, INPUT_PULLUP);
  pinMode(BLUE_BUTTON, INPUT_PULLUP);
}

void loop() {
  if (digitalRead(RED_BUTTON) == LOW) {
    Serial.println("Bouton rouge pressé !");
    delay(200);
  }

  if (digitalRead(BLUE_BUTTON) == LOW) {
    Serial.println("Bouton bleu pressé !");
    delay(200);
 }

}
