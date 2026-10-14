#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>

// --- Configuration Réseau Wi-Fi ---
const char* ssid = "nom_du_wifi";
const char* password = "mot_de_passe";

// --- Configuration Broker MQTT Public (HiveMQ) ---
const char* mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;

// Topic unique pour vos tests (personnalisez le nom pour qu'il soit unique)
const char* mqtt_topic = "DHT11/esp32/abdallah/mesures";

// --- Broches et Capteur DHT11 ---
#define DHTPIN 21     // GPIO connecté au signal du DHT11
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

WiFiClient espClient;
PubSubClient client(espClient);

// Gestion du timing sans delay()
unsigned long lastMsg = 0;
const long interval = 5000; // Envoi toutes les 5 secondes

void setup_wifi() {
    delay(10);
    Serial.println();
    Serial.print("Connexion à ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connecté !");
    Serial.print("Adresse IP : ");
    Serial.println(WiFi.localIP());
}

void reconnect() {
    // Boucle de reconnexion au broker MQTT public
    while (!client.connected()) {
        Serial.print("Tentative de connexion au broker MQTT public...");
        
        // Création d'un ID client unique basé sur des chiffres aléatoires
        String clientId = "ESP32Client-Abdallah-";
        clientId += String(random(0xffff), HEX);
        
        if (client.connect(clientId.c_str())) {
            Serial.println(" connectés !");
        } else {
            Serial.print(" échec, rc=");
            Serial.print(client.state());
            Serial.println(" nouvelle tentative dans 5 secondes");
            delay(5000);
        }
    }
}

void setup() {
    Serial.begin(115200);
    dht.begin();
    setup_wifi();
    client.setServer(mqtt_server, mqtt_port);
}

void loop() {
    // Vérification de la connexion MQTT
    if (!client.connected()) {
        reconnect();
    }
    client.loop();

    unsigned long now = millis();
    if (now - lastMsg > interval) {
        lastMsg = now;

        // Lecture de la température et de l'humidité
        float humidity = dht.readHumidity();
        float temperature = dht.readTemperature();

        // Vérification de la validité des données du DHT11
        if (isnan(humidity) || isnan(temperature)) {
            Serial.println("Erreur de lecture du capteur DHT11 !");
            return;
        }

        // Formatage du message en JSON (sécurisé avec snprintf)
        char payload[128];
        snprintf(payload, sizeof(payload), "{\"temperature\": %.1f, \"humidity\": %.1f}", temperature, humidity);

        Serial.print("Publication sur [");
        Serial.print(mqtt_topic);
        Serial.print("] : ");
        Serial.println(payload);

        // Publication effective sur le cloud
        client.publish(mqtt_topic, payload);
    }
}