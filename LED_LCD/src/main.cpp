#include <Arduino.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>


#define BUTTON_PIN 18   // Button --> pin GPIO18
#define LED_PIN 21      // LED    --> pin GPIO21

#define SDA_PIN 23      // SDA    --> pin GPIO 23
#define SCL_PIN 22      // SDL    --> pin GPIO 22


LiquidCrystal_I2C lcd(0x27, 16, 2); 

// Global variables
int led_state = LOW;
int button_state;
int last_button_state = HIGH; 

void setup() {
  Serial.begin(115200);

  Wire.begin(SDA_PIN, SCL_PIN); 

  lcd.init();          
  lcd.backlight();      
  lcd.print("Initialisation...");

  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Initialize LED state
  digitalWrite(LED_PIN, led_state);
}

void loop() {
  
  last_button_state = button_state;
  button_state = digitalRead(BUTTON_PIN);
  
  if (last_button_state == HIGH && button_state == LOW) {
    
    // Button pressed, the led state has changed
    led_state = !led_state; 

    digitalWrite(LED_PIN, led_state);

    // Update the display
    lcd.clear(); 
    lcd.setCursor(0, 0);

    if (led_state == HIGH) {
      lcd.print("LED: ON");
      lcd.setCursor(0, 1);
    } else {
      lcd.print("LED: OFF");
      lcd.setCursor(0, 1);
    }
  }
  
  delay(20); 
}