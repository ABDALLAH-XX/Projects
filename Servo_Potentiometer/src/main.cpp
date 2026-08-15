// Servo Sweep example for the ESP32
// https://wokwi.com/arduino/projects/323706614646309460

#include <ESP32Servo.h>

Servo servo;

//const int servoPin = 16;

int potpin = 14;  // analog pin used to connect the potentiometer
int val;    // variable to read the value from the analog pin

void setup() {
  Serial.begin(115200);
  servo.attach(16);
  pinMode(potpin, INPUT);
}


void loop() {
  val = analogRead(potpin);            // reads the value of the potentiometer (value between 0 and 1023)
  val = map(val, 0, 4095, 0, 180);     // scale it to use it with the servo (value between 0 and 180)
  servo.write(val);                  // sets the servo position according to the scaled value
  delay(15);
}
