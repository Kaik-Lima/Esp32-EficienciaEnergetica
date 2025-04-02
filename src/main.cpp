#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <PNGdec.h>

// Definições de pino ADC
#define ADC_PIN_corrente 34
#define ADC_PIN_tensao   35

TFT_eSPI tft = TFT_eSPI();

#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS   15
#define TFT_DC   5
#define TFT_RST  4
#define TFT_BL   32

// Credenciais da rede WiFi
const char* ssid     = "LIMA";
const char* password = "Mikasefa";

// URL da API no computador onde está rodando o Python
const char* url = "http://192.168.0.39:5000/consumoequipamento";

void calculaGrandezasEletricas();
void enviaDadosParaAPI(float consumo, String data, int id_equipamento);

float correnteIN = 0.0;
float tensaoRms  = 0.0;
float potencia   = 0.0;

// Inicializa WiFi e Serial
void setup() {
  
    tft.begin();
    tft.setRotation(0);
    tft.fillScreen(TFT_BLACK);
    Serial.begin(115200);

    WiFi.begin(ssid, password);
    Serial.print("Conectando ao WiFi");
    
    int tentativas = 0;
    while (WiFi.status() != WL_CONNECTED && tentativas < 30) {  // Tenta por 30 segundos
        delay(1000);
        Serial.print(".");
        tentativas++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nConectado ao WiFi!");
        Serial.print("Endereço IP: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("\nFalha ao conectar no WiFi. Reiniciando ESP...");
        ESP.restart();
    }
}

// Faz leituras e envia para a API
void loop() {
    calculaGrandezasEletricas();

    // Obtém a data e hora do ESP32
    String dataAtual = String(millis());  // Usando millis() como timestamp simples

    enviaDadosParaAPI(potencia, dataAtual, 1); // ID do equipamento = 1
    delay(60000);  // Aguarda 1 minuto antes de enviar novamente
}

// Calcula corrente, tensão e potência
void calculaGrandezasEletricas() {
    uint32_t adcValueCorrente = analogRead(ADC_PIN_corrente);
    correnteIN = (adcValueCorrente / 4095.0) * 20.0;  // Corrente simulada

    uint32_t adcValueTensao = analogRead(ADC_PIN_tensao);
    tensaoRms = (adcValueTensao / 4095.0) * 250.0;  // Tensão simulada

    potencia = tensaoRms * correnteIN;  // Calcula potência

    Serial.println("Leitura dos Sensores:");
    Serial.print("Corrente: ");
    Serial.print(correnteIN);
    Serial.println(" A");
    Serial.print("Tensão: ");
    Serial.print(tensaoRms);
    Serial.println(" V");
    Serial.print("Potência: ");
    Serial.print(potencia);
    Serial.println(" W");
}

// Envia os dados para a API no formato correto
void enviaDadosParaAPI(float consumo, String data, int id_equipamento) {
    if(WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(url);
        http.addHeader("Content-Type", "application/json");

        String payload = "{\"consumo\":" + String(consumo) + 
                         ", \"data_consumo\":\"" + data + 
                         "\", \"id_equipamento\":" + String(id_equipamento) + "}";

        Serial.println("Enviando JSON: " + payload);

        int httpResponseCode = http.POST(payload);

        if (httpResponseCode > 0) {
            Serial.print("Resposta da API: ");
            Serial.println(httpResponseCode);
        } else {
            Serial.print("Erro ao enviar dados: ");
            Serial.println(httpResponseCode);
        }

        http.end();
    } else {
        Serial.println("Erro: WiFi desconectado.");
    }
}
