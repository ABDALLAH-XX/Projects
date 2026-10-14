#include <Arduino.h>
#include <DHT.h>

#define DHT_PIN 21
#define DHT_TYPE DHT11

DHT dht(DHT_PIN, DHT_TYPE);

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("Test du capteur DHT11");
    Serial.print("GPIO utilise : ");
    Serial.println(DHT_PIN);

    dht.begin();
    delay(2000);
}

void loop() {
    float temperature = dht.readTemperature();

    if (isnan(temperature)) {
        Serial.println("ECHEC : aucune temperature valide");
    } else {
        Serial.print("Temperature : ");
        Serial.print(temperature);
        Serial.println(" C");
    }

    delay(2500);
}