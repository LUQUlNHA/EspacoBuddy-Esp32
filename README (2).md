
# EspacoBuddy - Sistema de Alimentação Automatizada com ESP32

Este projeto implementa um sistema de alimentação automatizada para animais de estimação utilizando um microcontrolador ESP32, motores, sensores de fim de curso, e comunicação MQTT via Wi-Fi. A estrutura modular segue boas práticas de design de software e referencia os princípios SOLID para garantir manutenibilidade, reutilização e escalabilidade.

## 📦 Estrutura do Projeto

- `main.ino`: Função `setup()` e `loop()` central do sistema.
- `config.h`: Contém definições de pinos, credenciais de Wi-Fi, tópicos MQTT e funções de configuração de rede.
- `function.h`: Implementa a lógica de abertura/fechamento da porta, liberação de ração, publicações MQTT e leitura de sensores.

## 🔧 Funcionalidades

- Liberação de porção de ração por comando MQTT.
- Controle da porta com sensores de fim de curso e motor bidirecional.
- Publicação periódica do status do sistema.
- Sistema tolerante a falhas com tentativas automáticas de reabertura e fechamento da porta.
- Callback MQTT modular para fácil expansão de comandos.

## ✅ Princípios de Design (SOLID)

- **S - Single Responsibility Principle (Princípio da Responsabilidade Única):**
  - Cada arquivo/função tem uma única responsabilidade. Ex: `config.h` cuida apenas da configuração e rede, enquanto `function.h` contém a lógica de alimentação.

- **O - Open/Closed Principle (Aberto para Extensão, Fechado para Modificação):**
  - A função `mqttCallback` pode ser facilmente estendida com novos comandos sem modificar seu comportamento base.

- **L - Liskov Substitution Principle:**
  - Embora o projeto não utilize herança, a composição de funções permite substituições controladas e previsíveis (ex: publicar dados pode ser adaptado para diferentes payloads).

- **I - Interface Segregation Principle:**
  - O código evita criar funções genéricas demais e mantém as responsabilidades específicas, separando lógica de motor, sensores e rede.

- **D - Dependency Inversion Principle:**
  - As dependências estão invertidas ao usar interfaces genéricas do PubSubClient e WiFiClient, promovendo testabilidade.

## 🧪 Requisitos de Hardware

- ESP32
- Motor DC (x2) com Ponte H
- Sensores de fim de curso (abertura e fechamento)
- Sensor touch
- LED indicador
- Acesso à rede Wi-Fi

## 🔗 Dependências

- Biblioteca `WiFi.h`
- Biblioteca `PubSubClient.h`

## 📡 Comunicação MQTT

- **Broker:** `test.mosquitto.org`
- **Tópico para publicar:** `menu/params/post/espacobuddy`
- **Tópico para assinar:** `menu/params/get/espacobuddy`

## 🚀 Como Usar

1. Configure a rede Wi-Fi e tópicos MQTT no `config.h`.
2. Faça upload dos arquivos para o ESP32.
3. Use um cliente MQTT para enviar o comando `liberarDose`.
4. Acompanhe as mensagens via Monitor Serial (115200 baud).

---

Desenvolvido utilizando boas práticas de engenharia embarcada e arquitetura de software.
