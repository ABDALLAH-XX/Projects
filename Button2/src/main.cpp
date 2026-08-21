#include <Arduino.h>
#include <ezButton.h>


#define RED_BUTTON 27
#define BLUE_BUTTON 25

ezButton button1(RED_BUTTON);  // create ez Button object
ezButton button2(BLUE_BUTTON);  // create ez Button object


void setup() {
  // put your setup code here, to run once:
  // initialize the pushbutton as an pull-up input
  button1.setDebounceTime(100); // 100ms
  button2.setDebounceTime(100); // 100ms
  Serial.begin(9600);
}

void loop() {
  button1.loop(); // must call loop() function first
  button2.loop();

  // get button state after debounce
  int button1State = button1.getState();
  int button2State = button2.getState();

  if (button1.isPressed())
    Serial.println("Button 1 is pressed");

  if (button1.isReleased())
    Serial.println("Button 1 is released");

  if (button2.isPressed())
    Serial.println("Button 2 is pressed");

  if (button2.isReleased())
    Serial.println("Button 2 is released");
}
