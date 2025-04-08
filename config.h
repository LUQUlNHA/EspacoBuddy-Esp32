#ifndef CONFIG_H
#define CONFIG_H

#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>


// ======= Credenciais Wi-Fi =======
const char* ssid = "Lucas";
const char* password = "12345678";

// ======= MQTT Broker (HiveMQ) =======
const char* MQTT_BROKER = "6226f55bd628466c9146337dca41dca0.s1.eu.hivemq.cloud";
const int MQTT_PORT = 8883;
const char* MQTT_USER = "EspacoBuddyEsp32";
const char* MQTT_PASSWORD = "EngenhariaDeSoftware1";

// ======= Tópicos MQTT =======
const char* MQTT_PUBLISH_TOPIC = "menu/params/post/espacobuddy";
const char* MQTT_SUBSCRIBE_TOPIC = "menu/params/get/espacobuddy";

// ======= Objetos Globais =======
WiFiClientSecure espClient;
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
  espClient.setInsecure();  // Ignora a verificação do certificado SSL/TLS

}

// ======= Reconexão MQTT =======
void reconnectMQTT(void (*callback)(char*, byte*, unsigned int)) {
  while (!client.connected()) {
    Serial.print("Conectando ao broker MQTT...");
    if (client.connect("ESP32Client", MQTT_USER, MQTT_PASSWORD)) {
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
