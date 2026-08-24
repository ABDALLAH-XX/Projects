#include <Arduino.h>

#define BUTTON_PIN 21 // ESP32 pin GPIO18, which connected to button
#define LED_PIN 4 // ESP32 pin GPIO1, which connected to led

// variables will change:
int button_state = 0; // variable for reading the button status

void setup() {
  // initialize the LED pin as an output:
  pinMode(LED_PIN, OUTPUT);
  // initialize the button pin as an pull-up output:
  //the pull-up input pin will be HIGH when the button is open and LOW when the button is pressed.
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.begin(115200);
}

void loop() {
  // read the state of the button value : 
  button_state = digitalRead(BUTTON_PIN);

  // control LED according to the state of button
  if (button_state == LOW) {       // if button is pressed
    digitalWrite(LED_PIN, HIGH);  // turn on LED
    Serial.println("LED ON");
  }
  else {                            // otherwise, button is not pressed
    digitalWrite(LED_PIN, LOW);   // turn off LED
    Serial.println("LED OFF");
  }
  
  delay(100);
}
