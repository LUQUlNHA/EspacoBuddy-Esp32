#ifndef CONFIG_H
#define CONFIG_H

#include <WiFi.h>           // Biblioteca para conexão Wi-Fi no ESP32
#include <PubSubClient.h>   // Biblioteca para comunicação MQTT

// ======= Credenciais da Rede Wi-Fi =======
const char* ssid = "Lucas";         // Nome da rede Wi-Fi
const char* password = "12345678";  // Senha da rede Wi-Fi

// ======= Configurações do Broker MQTT =======
const char* MQTT_BROKER = "test.mosquitto.org";  // Endereço do broker público (pode ser substituído por um privado)
const int MQTT_PORT = 1883;                      // Porta padrão do MQTT sem TLS

// ======= Tópicos MQTT utilizados =======
const char* MQTT_PUBLISH_TOPIC = "menu/params/post/espacobuddy";   // Tópico para publicação de dados (envio)
const char* MQTT_SUBSCRIBE_TOPIC = "menu/params/get/espacobuddy";  // Tópico para recebimento de comandos

// ======= Objetos Globais =======
// Objeto responsável pela conexão Wi-Fi
WiFiClient espClient;

// Objeto MQTT, configurado para usar o cliente Wi-Fi
PubSubClient client(espClient);

// ======= Função de Conexão Wi-Fi =======
void setupWiFi() {
  Serial.print("Conectando ao Wi-Fi: ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);  // Inicia conexão com rede
  
  // Aguarda conexão
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  // Exibe IP ao conectar
  Serial.println("\nWi-Fi conectado!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

// ======= Função para Reconectar ao Broker MQTT =======
void reconnectMQTT(void (*callback)(char*, byte*, unsigned int)) {
  while (!client.connected()) {
    Serial.print("Conectando ao broker MQTT...");
    
    // Tenta conectar com o ID "ESP32Client"
    if (client.connect("ESP32Client")) {
      Serial.println("Conectado!");

      // Define função callback para tratar mensagens recebidas
      client.setCallback(callback);

      // Inscreve-se no tópico de recebimento
      client.subscribe(MQTT_SUBSCRIBE_TOPIC);
    } else {
      // Em caso de falha, exibe o código e tenta novamente após 2s
      Serial.print("Falhou. Código: ");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

#endif
