#include <Arduino.h>
#include <ezButton.h>


//#define BUTTON_PIN 26        // GPIO26 pin connected to button
#define SHORT_PRESS_TIME 1000 // 1000 milliseconds
#define LONG_PRESS_TIME 1000 // 1000 milliseconds

ezButton button(26); // create ezButton object that allows that attach to pin GPIO26

// Variables will change:
unsigned long pressedTime = 0;
unsigned long releasedTime = 0;
bool isPressing = false;
bool isLongDetected = false;

void setup() {
  Serial.begin(9600);
  button.setDebounceTime(50); // set debounce time to 50s
}

void loop() {
  // read the state of the switch/button
  button.loop(); // MUST call the loop() function first

  if (button.isPressed()) {
    pressedTime = millis();
    isPressing = true;
    isLongDetected = false;
  }

  if (button.isReleased()) {
    isPressing = false;
    releasedTime = millis();


    long pressDuration = releasedTime - pressedTime;

    if (pressDuration < SHORT_PRESS_TIME)
      Serial.println("A short press is detected");
  }

  if (isPressing == true && isLongDetected == false) {
    long pressDuration = millis() - pressedTime;

    if (pressDuration > LONG_PRESS_TIME) {
      Serial.println("A long press is detected");
      isLongDetected = true;
    }
  }
}