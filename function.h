#ifndef FUNCTION_H
#define FUNCTION_H

#include <Arduino.h>
#include "config.h"

// ==================== Configuração dos pinos ====================
#define TOUCH_PIN 4              // Pino do sensor de toque
#define MOTOR_RACAO_PIN 15       // Pino do motor de liberação da ração
#define MOTOR_PORTA_PIN1 2       // Pino 1 do motor da porta (ponte H)
#define MOTOR_PORTA_PIN2 5       // Pino 2 do motor da porta (ponte H)
#define SENSOR_ABRIR_PIN 25      // Sensor de fim de curso indicando porta aberta
#define SENSOR_FECHAR_PIN 26     // Sensor de fim de curso indicando porta fechada
#define LED 32                   // LED indicador (pode ser usado para status)

// ==================== Parâmetros de controle ====================
int delayTime = 2000;     // Tempo que o motor da ração ficará ligado (em ms)
int retryDelay = 1500;    // Tempo máximo para abrir/fechar a porta antes de tentar novamente
int maxRetries = 3;       // Máximo de tentativas ao abrir/fechar a porta

// ==================== Prototipação das funções ====================
void liberarDose();                   // Libera uma porção de ração
bool abrirPorta(int tentativa);       // Abre a porta com controle de tentativas
bool fecharPorta(int tentativa);      // Fecha a porta com controle de tentativas
void pararMotor();                    // Para os motores da porta

// ==================== Variáveis de estado ====================
String status = "OK";     // Status do sistema (pode ser atualizado conforme falhas ou eventos)
int nivelRacao = 78;      // Nível simulado da ração (%)

// ==================== Função Callback do MQTT ====================
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];  // Converte payload em string
  }

  Serial.print("Mensagem recebida em ");
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(msg);

  // Verifica se o comando é para liberar uma dose de ração
  if (msg == "liberarDose") {
    liberarDose();
  }
}

// ==================== Publicar dados via MQTT ====================
void publishData() {
  String payload = "{";
  payload += "\"status\":\"" + status + "\",";
  payload += "\"nivel_racao\":" + String(nivelRacao);
  payload += "}";

  client.publish(MQTT_PUBLISH_TOPIC, payload.c_str());
  Serial.println("Publicado: " + payload);
}

// ==================== Liberação de ração ====================
void liberarDose() {
  digitalWrite(2, HIGH); // Ativa LED indicador (ou outro sinal)

  if (abrirPorta(0)) {
    delay(1000); // Espera um pouco antes de liberar

    Serial.println("Liberando dose de ração...");
    digitalWrite(MOTOR_RACAO_PIN, HIGH);  // Liga motor
    delay(delayTime);                     // Tempo de operação
    digitalWrite(MOTOR_RACAO_PIN, LOW);   // Desliga motor
    Serial.println("Dose liberada.");

    delay(1000); // Espera antes de fechar a porta

    if (!fecharPorta(0)) {
      Serial.println("Erro ao fechar a porta!");
    }
  } else {
    Serial.println("Erro ao abrir a porta!");
  }

  digitalWrite(2, LOW); // Desliga LED indicador
}

// ==================== Abertura da porta ====================
bool abrirPorta(int tentativa) {
  if (tentativa >= maxRetries) {
    Serial.println("Limite de tentativas para abrir a porta atingido.");
    return false;
  }

  // Se já estiver aberta, retorna sucesso
  if (digitalRead(SENSOR_ABRIR_PIN) == LOW) {
    Serial.println("Porta já está aberta.");
    return true;
  }

  unsigned long startTime = millis();
  Serial.println("Abrindo porta...");

  while (digitalRead(SENSOR_ABRIR_PIN) == HIGH) {
    digitalWrite(MOTOR_PORTA_PIN1, HIGH);
    digitalWrite(MOTOR_PORTA_PIN2, LOW);

    if (millis() - startTime > retryDelay) {
      Serial.println("Timeout ao tentar abrir a porta.");
      pararMotor();
      delay(500);

      // Recuo para evitar travamento
      digitalWrite(MOTOR_PORTA_PIN1, LOW);
      digitalWrite(MOTOR_PORTA_PIN2, HIGH);
      delay(200);
      pararMotor();

      return abrirPorta(tentativa + 1); // Tenta novamente
    }
  }

  pararMotor();
  Serial.println("Porta aberta com sucesso!");
  return true;
}

// ==================== Fechamento da porta ====================
bool fecharPorta(int tentativa) {
  if (tentativa >= maxRetries) {
    Serial.println("Limite de tentativas para fechar a porta atingido.");
    return false;
  }

  // Se já estiver fechada, retorna sucesso
  if (digitalRead(SENSOR_FECHAR_PIN) == LOW) {
    Serial.println("Porta já está fechada.");
    return true;
  }

  unsigned long startTime = millis();
  Serial.println("Fechando porta...");

  while (digitalRead(SENSOR_FECHAR_PIN) == HIGH) {
    digitalWrite(MOTOR_PORTA_PIN1, LOW);
    digitalWrite(MOTOR_PORTA_PIN2, HIGH);

    if (millis() - startTime > retryDelay) {
      Serial.println("Timeout ao tentar fechar a porta.");
      pararMotor();
      delay(500);

      // Recuo antes de nova tentativa
      digitalWrite(MOTOR_PORTA_PIN1, HIGH);
      digitalWrite(MOTOR_PORTA_PIN2, LOW);
      delay(100);
      pararMotor();

      return fecharPorta(tentativa + 1);
    }
  }

  pararMotor();
  Serial.println("Porta fechada com sucesso!");
  return true;
}

// ==================== Parar motor da porta ====================
void pararMotor() {
  digitalWrite(MOTOR_PORTA_PIN1, LOW);
  digitalWrite(MOTOR_PORTA_PIN2, LOW);
}

#endif
