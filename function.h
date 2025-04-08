#ifndef FUNCTION_H
#define FUNCTION_H

#include <Arduino.h>
#include "config.h"

// Configuração dos pinos
#define TOUCH_PIN 4
#define MOTOR_RACAO_PIN 15
#define MOTOR_PORTA_PIN1 2 // Ponte H: lado 1 do motor da porta
#define MOTOR_PORTA_PIN2 5 // Ponte H: lado 2 do motor da porta
#define SENSOR_ABRIR_PIN 25 // Fim de curso para abertura 
#define SENSOR_FECHAR_PIN 26 // Fim de curso para fechamento 
#define LED 32//LED AWS


int delayTime = 2000; // Tempo padrão para liberar ração
int retryDelay = 1500; // Tempo máximo para tentar abrir/fechar a porta
int maxRetries = 3;    // Número máximo de tentativas recursivas

void liberarDose();
bool abrirPorta(int tentativa);
bool fecharPorta(int tentativa);
void pararMotor();


// Estado atual
String status = "OK";
int nivelRacao = 78; // valor simulado

// ===== Função de Callback do MQTT =====
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  String msg = "";
  for (int i = 0; i < length; i++) {
    msg += (char)payload[i];
  }

  Serial.print("Mensagem recebida em ");
  Serial.print(topic);
  Serial.print(": ");
  Serial.println(msg);

  if (msg == "liberarDose") {
    liberarDose();
  }
}


// ===== Publicar dados =====
void publishData() {
  String payload = "{";
  payload += "\"status\":\"" + status + "\",";
  payload += "\"nivel_racao\":" + String(nivelRacao);
  payload += "}";

  client.publish(MQTT_PUBLISH_TOPIC, payload.c_str());
  Serial.println("Publicado: " + payload);
}

void liberarDose() {
  digitalWrite(2,HIGH);
  if (abrirPorta(0)) {
    delay(1000);  // Aguarda antes de liberar a ração

    Serial.println("Liberando dose de ração...");
    digitalWrite(MOTOR_RACAO_PIN, HIGH);  // Ativa motor da ração
    delay(delayTime);                     // Tempo para liberar ração
    digitalWrite(MOTOR_RACAO_PIN, LOW);   // Desativa motor da ração
    Serial.println("Dose liberada.");

    delay(1000);  // Aguarda antes de fechar a porta

    if (!fecharPorta(0)) {
      Serial.println("Erro ao fechar a porta!");
    }
  } else {
    Serial.println("Erro ao abrir a porta!");
  }
    digitalWrite(2,LOW);
}


bool abrirPorta(int tentativa) {
  if (tentativa >= maxRetries) {
    Serial.println("Limite de tentativas para abrir a porta atingido.");
    return false; // Retorna falha após atingir o limite de tentativas
  }

  // Se a porta já estiver aberta, não faz nada
  if (digitalRead(SENSOR_ABRIR_PIN) == LOW) {
    Serial.println("Porta já está aberta.");
    return true;
  }

  unsigned long startTime = millis();
  Serial.println("Abrindo porta...");

  while (digitalRead(SENSOR_ABRIR_PIN) == HIGH) {
    digitalWrite(MOTOR_PORTA_PIN1, HIGH);
    digitalWrite(MOTOR_PORTA_PIN2, LOW);

    // Verifica timeout
    if (millis() - startTime > retryDelay) {
      Serial.println("Timeout ao tentar abrir a porta.");
      pararMotor();
      delay(500);
      
      // Pequeno recuo antes de tentar novamente
      digitalWrite(MOTOR_PORTA_PIN1, LOW);
      digitalWrite(MOTOR_PORTA_PIN2, HIGH);
      delay(200);
      pararMotor();

      return abrirPorta(tentativa + 1); // Chama recursivamente
    }
  }

  pararMotor();
  Serial.println("Porta aberta com sucesso!");
  return true;
}

bool fecharPorta(int tentativa) {
  if (tentativa >= maxRetries) {
    Serial.println("Limite de tentativas para fechar a porta atingido.");
    return false;
  }

  // Se a porta já estiver fechada, não faz nada
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

      // Pequeno recuo antes de tentar novamente
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

// Função para parar o motor
void pararMotor() {
  digitalWrite(MOTOR_PORTA_PIN1, LOW);
  digitalWrite(MOTOR_PORTA_PIN2, LOW);
}


#endif
