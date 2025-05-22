#include "config.h"      // Arquivo de configuração com pinos, credenciais de rede e constantes
#include "function.h"    // Arquivo com as funções auxiliares (WiFi, MQTT, motores, etc.)

/**
 * Função de configuração do sistema (executada uma vez na inicialização).
 */
void setup() {
  Serial.begin(115200);  // Inicializa comunicação serial para debug

  // Define o modo de operação (entrada ou saída) de cada pino utilizado no sistema
  pinMode(LED, OUTPUT);                     // LED indicador
  pinMode(2, OUTPUT);                       // Pino genérico (talvez um LED adicional)
  pinMode(TOUCH_PIN, INPUT);               // Sensor touch (entrada)
  pinMode(MOTOR_RACAO_PIN, OUTPUT);        // Motor que libera a ração
  pinMode(MOTOR_PORTA_PIN1, OUTPUT);       // Controle do motor da porta (direção 1)
  pinMode(MOTOR_PORTA_PIN2, OUTPUT);       // Controle do motor da porta (direção 2)
  pinMode(SENSOR_ABRIR_PIN, INPUT_PULLUP); // Sensor de fim de curso (porta aberta)
  pinMode(SENSOR_FECHAR_PIN, INPUT_PULLUP);// Sensor de fim de curso (porta fechada)

  // Conecta à rede Wi-Fi
  setupWiFi();

  // Define o servidor MQTT a ser utilizado
  client.setServer(MQTT_BROKER, MQTT_PORT);

  // Conecta ao broker MQTT e define a função de callback para mensagens recebidas
  reconnectMQTT(mqttCallback);

  // Garante que a porta esteja inicialmente fechada por segurança
  Serial.println("Fechando a porta para posição inicial...");
  if (fecharPorta(0)) {
    Serial.println("Porta fechada. Sistema pronto!");
  } else {
    Serial.println("Falha ao fechar a porta na inicialização!");
  }
}

/**
 * Função principal do loop do programa (executada continuamente).
 */
void loop() {
  // Garante que o cliente MQTT esteja conectado
  if (!client.connected()) {
    reconnectMQTT(mqttCallback);  // Reestabelece conexão MQTT se necessário
  }

  client.loop();  // Mantém a conexão MQTT ativa e trata mensagens recebidas

  // Publica dados a cada 10 segundos
  static unsigned long lastPublish = 0;
  if (millis() - lastPublish > 10000) {  // 10.000 ms = 10 s
    publishData();                      // Envia os dados do dispositivo via MQTT
    lastPublish = millis();            // Atualiza o timestamp da última publicação
  }
}
