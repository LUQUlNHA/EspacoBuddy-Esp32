#include "config.h"
#include "function.h"


void setup() {
  Serial.begin(115200);

  //definir modos de opeção de cada pino
  pinMode(LED, OUTPUT);
  pinMode(2, OUTPUT);
  pinMode(TOUCH_PIN, INPUT);
  pinMode(MOTOR_RACAO_PIN, OUTPUT);
  pinMode(MOTOR_PORTA_PIN1, OUTPUT);
  pinMode(MOTOR_PORTA_PIN2, OUTPUT);
  pinMode(SENSOR_ABRIR_PIN, INPUT_PULLUP);
  pinMode(SENSOR_FECHAR_PIN, INPUT_PULLUP);

  setupWiFi();
  client.setServer(MQTT_BROKER, MQTT_PORT);
  reconnectMQTT(mqttCallback);

    //Direcionar a porta para a posição inicial (fechada)
  Serial.println("Fechando a porta para posição inicial...");
  if (fecharPorta(0)) {
    Serial.println("Porta fechada. Sistema pronto!");
  } else {
    Serial.println("Falha ao fechar a porta na inicialização!");
  }
}

void loop() {
  if (!client.connected()) {
    reconnectMQTT(mqttCallback);
  }

  client.loop();

  static unsigned long lastPublish = 0;
  if (millis() - lastPublish > 10000) {
    publishData();
    lastPublish = millis();
  }
}
