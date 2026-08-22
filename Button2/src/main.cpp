#include <Arduino.h>
#include <ezButton.h>

#define BUTTON_NUM 3 // the number of buttons


ezButton buttonArray[] = {
  ezButton(27),
  ezButton(26),
  ezButton(25)
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  
  for (byte i = 0; i < BUTTON_NUM; i++) {
    buttonArray[i].setDebounceTime(100); // 100ms
  }
}

void loop() {
  for (byte i = 0; i < BUTTON_NUM; i++)
    buttonArray[i].loop(); // MUST call the loop() function

  for (byte i = 0; i< BUTTON_NUM; i++) {
    // get button state after debounce
    int buttonState = buttonArray[i].getState();

    if (buttonArray[i].isPressed()) {
      Serial.println("The button ");
      Serial.print(i + 1);
      Serial.println(" is pressed");
    }

    if (buttonArray[i].isReleased()) {
      Serial.println("The button ");
      Serial.print(i + 1);
      Serial.println(" is released");
    }
  }
}
