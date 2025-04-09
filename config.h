#ifndef CONFIG_H
#define CONFIG_H

#include <WiFi.h>
#include <PubSubClient.h>

// ======= Credenciais Wi-Fi =======
const char* ssid = "Lucas";
const char* password = "12345678";

// ======= MQTT Broker (test.mosquitto.org) =======
const char* MQTT_BROKER = "test.mosquitto.org";
const int MQTT_PORT = 1883;

// ======= Tópicos MQTT =======
const char* MQTT_PUBLISH_TOPIC = "menu/params/post/espacobuddy";
const char* MQTT_SUBSCRIBE_TOPIC = "menu/params/get/espacobuddy";

// ======= Objetos Globais =======
WiFiClient espClient;           // Usa conexão sem SSL/TLS
PubSubClient client(espClient);

// ======= Conexão Wi-Fi =======
void setupWiFi() {
  Serial.print("Conectando ao Wi-Fi: ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

// ======= Reconexão MQTT =======
void reconnectMQTT(void (*callback)(char*, byte*, unsigned int)) {
  while (!client.connected()) {
    Serial.print("Conectando ao broker MQTT...");
    if (client.connect("ESP32Client")) {
      Serial.println("Conectado!");
      client.setCallback(callback);
      client.subscribe(MQTT_SUBSCRIBE_TOPIC);
    } else {
      Serial.print("Falhou. Código: ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

#endif
