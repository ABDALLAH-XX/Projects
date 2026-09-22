#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_MOSI   23
#define OLED_CLK    18
#define OLED_DC      2
#define OLED_RESET   4
#define OLED_CS      5

// Initialisation SH1106 SPI matériel
Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

void setup() {
  display.begin(0, true); // (Adresse I2C inutilisée en SPI, Reset automatique)
  display.clearDisplay();
  
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);
  display.setCursor(0, 20);
  display.println("Hello!");
  display.println("SH1106 OK");
  
  display.display();
}

void loop() {
}