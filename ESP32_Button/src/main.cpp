#include <Arduino.h>

#define BUTTON_PIN 26

void setup() {
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.println("=== BUTTON DIAGNOSTIC ===");
  Serial.println("Press button to see state change");
  Serial.println("1 = Not Pressed, 0 = Pressed");
  Serial.println("==========================");
}

void loop() {
  int buttonState = digitalRead(BUTTON_PIN);
  
  Serial.print("Button State: ");
  Serial.println(buttonState);
  
  if (buttonState == LOW) {
    Serial.println(">>> BUTTON IS PRESSED! <<<");
  }
  
  delay(200);  // Read every 200ms
}