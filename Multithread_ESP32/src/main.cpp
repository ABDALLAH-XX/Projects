#include <Arduino.h>

#define LED1 19
#define LED2 18

void taskLED1(void *pvParameters) {
  pinMode(LED1, OUTPUT);
  while (true) {
    digitalWrite(LED1, !digitalRead(LED1));
    vTaskDelay(pdMS_TO_TICKS(300)); // clignote toutes les 300ms
  }
}

void taskLED2(void *pvParameters) {
  pinMode(LED2, OUTPUT);
  while (true) {
    digitalWrite(LED2, !digitalRead(LED2));
    vTaskDelay(pdMS_TO_TICKS(700)); // clignote toutes les 700ms
  }
}

void setup() {
  xTaskCreatePinnedToCore(taskLED1, "LED1", 1000, NULL, 1, NULL, 0); // core 0
  xTaskCreatePinnedToCore(taskLED2, "LED2", 1000, NULL, 1, NULL, 1); // core 1
}

void loop() {
  // vide, tout se passe dans les tâches
}