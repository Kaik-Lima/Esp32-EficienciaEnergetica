

#include "xbm48X48.h"             // Sketch tab header for xbm images
#include "xbm128X128.h"          // Sketch tab header for xbm images


#include <TFT_eSPI.h> // Biblioteca para o display TFT
#include <WiFi.h>     // Biblioteca para WiFi no ESP32
//#include <SPI.h>
#include <esp_adc_cal.h>


// Defina o pino ADC que você está usando
//#define ADC_PIN_corrente 34
//#define ADC_PIN_tensao 35
#define ADC_PIN_corrente 32
#define ADC_PIN_tensao 33

// Credenciais da rede WiFi
const char* ssid = "ATMEL_2G";
const char* password = "bemtevi32";


float maxAdcValueCorrente = 4095.0;
float maxScaleValueCorrente = 20.0;

float maxAdcValueVotls = 4095.0;
float maxScaleValueVotls = 250.0;

float correnteIN = 0.0;
float tensaoRms = 0.0;
float potencia = 0.0;


TFT_eSPI tft = TFT_eSPI(); // Criação de uma instância do objeto TFT

void setup() 
  {
  tft.init(); // Inicialização do display
  tft.setRotation(0); // Rotação do display (ajuste conforme necessário)
  tft.fillScreen(TFT_BLACK); // Black screen fill

  // Inicialização da comunicação serial
  Serial.begin(9600);
  imprimeLogo();
  delay(2000);
  statusSemWifi();
  initWifi(); 
  }


void loop()
  {
  calculaGrandezasEletricas();
  delay(4000);
  }


void initWifi()
  {  
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) 
      {
      Serial.println("Tentando se conectar à rede WiFi...");
      delay(2000);
      }
      Serial.println("Conectado à rede WiFi com sucesso!");
      tft.fillScreen(TFT_BLACK);
  }


void imprimeLogo()
  {  
  // mostra o icone do sem wifi 
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("GERENCIADOR", 30, 10, 4); //string, eixo x, eixo y, font
  tft.drawString("ENERGIA", 65, 40, 4); //string, eixo x, eixo y, font
  tft.drawXBitmap(56, 110, iconeLogo, logoWidth, logoHeight , TFT_WHITE, TFT_RED);
  tft.drawString("SASC", 90, 262, 4);
  //tft.setTextColor(TFT_WHITE, TFT_BLACK);
  //delay(500);
  }


// #########################################################################
//  Draw the analogue meter on the screen
// #########################################################################
void statusSemWifi()
{
// mostra o icone do sem wifi 
tft.fillScreen(TFT_BLACK);
tft.setTextColor(TFT_WHITE, TFT_BLACK);
tft.drawString("GERENCIADOR", 30, 10, 4); //string, eixo x, eixo y, font
tft.drawString("ENERGIA", 65, 40, 4); //string, eixo x, eixo y, font
tft.drawXBitmap(56, 110, iconSemWifi, logoWidth, logoHeight , TFT_WHITE, TFT_RED);
tft.drawString("Sem Wifi", 68, 262, 4);
delay(500);
}

void statusComWifi()
{
// mostra o icone do com wifi 
tft.fillScreen(TFT_BLACK);
tft.setTextColor(TFT_WHITE, TFT_BLACK);
tft.drawString("GERENCIADOR", 30, 10, 4); //string, eixo x, eixo y, font
tft.drawString("ENERGIA", 65, 40, 4); //string, eixo x, eixo y, font
tft.drawXBitmap(56, 110, iconComWifi, logoWidth, logoHeight , TFT_WHITE, TFT_GREEN);
tft.drawString("Com Wifi", 68, 262, 4); //string, eixo x, eixo y, font
delay(500);
}

/*
void statusWifi48x48()
{
}

void statusTomada48x48()
{
}

void statusTomada()
{
// mostra o icone de energia desconectada
tft.fillScreen(TFT_BLACK);
tft.setTextColor(TFT_WHITE, TFT_BLACK);
tft.drawString("GERENCIADOR", 30, 10, 4); //string, eixo x, eixo y, font
tft.drawString("ENERGIA", 65, 40, 4); //string, eixo x, eixo y, font
tft.drawXBitmap(56, 110, tomada, logoWidth, logoHeight , TFT_WHITE, TFT_RED);
tft.drawString("Sem Carga", 55, 262, 4); //string, eixo x, eixo y, font
delay(500);


// mostra o icone de energia conectada
tft.fillScreen(TFT_BLACK);
tft.setTextColor(TFT_WHITE, TFT_BLACK);
tft.drawString("GERENCIADOR", 30, 10, 4); //string, eixo x, eixo y, font
tft.drawString("ENERGIA", 65, 40, 4); //string, eixo x, eixo y, font
tft.drawXBitmap(56, 110, tomada, logoWidth, logoHeight , TFT_WHITE, TFT_GREEN);
tft.drawString("Com Carga", 55, 262, 4); //string, eixo x, eixo y, font
delay(500);
}*/


void calculaGrandezasEletricas()
{

  //############## CALCULA DE CORRENTE (AMPERES)
  // Realiza a leitura do valor ADC
  uint32_t adcValueCorrente = analogRead(ADC_PIN_corrente);   
  // Converte o valor ADC para a escala de tensão 0 - 5A 
  // valor adc, 0, 4095, 0A, 5A
  //float correnteIN = map(adcValueCorrente, 0, maxAdcValueCorrente, minScaleValueCorrente, maxScaleValueCorrente);
  correnteIN  = (adcValueCorrente / maxAdcValueCorrente) * maxScaleValueCorrente;

  //############## CALCULA DE TENSÃO (VOLTS)
  // Realiza a leitura do valor ADC
  uint32_t adcValueTensao = analogRead(ADC_PIN_tensao); 
  // Converte o valor ADC para a escala de 0 a 250
  // valor adc, 0, 4095, 0V, 250V
  //float tensaoRms = map(adcValueTensao, 0, maxAdcValueVotls, minScaleValueVotls, maxScaleValueVotls);
  //float tensaoRms  = (adcValueTensao/ 4095)*250;
  tensaoRms  = (adcValueTensao / maxAdcValueVotls) * maxScaleValueVotls;

  //############## CALCULA DE POTENCIA (WATTS)
  potencia = tensaoRms * correnteIN;



  // Exibe o valor lido e a corrente correspondente
  Serial.print("Valor do ADC Corrente: ");
  Serial.println(adcValueCorrente);
  Serial.print("Valor Corrente: ");
  Serial.println(correnteIN);

  // Exibe o valor lido e a tensão correspondente
  Serial.print("Valor do ADC tensão: ");
  Serial.println(adcValueTensao);
  Serial.print("Valor Tensão: ");
  Serial.println(tensaoRms);

  // Exibe o valor lido e a potencia correspondente
  Serial.print("Valor Potência: ");
  Serial.println(potencia);

  // Mostra o valor de Tensão na tela 
  //tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("VOLTS", 10, 100, 2); //string, eixo x, eixo y, font
  tft.setTextColor(TFT_MAGENTA, TFT_BLACK);
  tft.drawFloat(tensaoRms , 1, 10, 120, 7); //floatNumber, casas decimal, x, y, font
  tft.drawString("VOLT ", 160, 145, 4); //string, eixo x, eixo y, font
  delay(200);

  // Mostra o valor de Corrente na tela 
  //tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("CORRENTE", 10, 170, 2); //string, eixo x, eixo y, font
  tft.setTextColor(TFT_ORANGE, TFT_BLACK);
  tft.drawFloat(correnteIN , 1, 10, 190, 7); //floatNumber, casas decimal, x, y, font
  tft.drawString("AMPERES ", 120, 215, 4); //string, eixo x, eixo y, font
  delay(200);

  // Mostra o valor de Potencia na tela 
  //tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString("POTENCIA ", 10, 240, 2); //string, eixo x, eixo y, font
  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.drawFloat(potencia , 1, 10, 260, 7); //floatNumber, casas decimal, x, y, font
  tft.drawString("WATT ", 160, 285, 4); //string, eixo x, eixo y, font
  delay(200);


  if  (WiFi.status() != WL_CONNECTED) 
    {
      // mostra o icone do sem wifi 
      //tft.fillScreen(TFT_BLACK);
      tft.setTextColor(TFT_WHITE, TFT_BLACK);
      tft.drawXBitmap(15, 5, iconSemWifi48, logoWidth48, logoHeight48, TFT_WHITE, TFT_RED);
      tft.drawString("Sem Wifi", 15, 60, 2);
      delay(200);
  }else
    {
    // mostra o icone do sem wifi 
    //tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawXBitmap(15, 5, iconComWifi48, logoWidth48, logoHeight48 , TFT_WHITE, TFT_GREEN);
    tft.drawString("Com Wifi", 15, 60, 2);
    delay(200);
    }


  if (correnteIN >= 0.1)
  {
  // mostra o icone do sem wifi 
  //tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawXBitmap(170, 5, iconTomada48, logoWidth48, logoHeight48 , TFT_WHITE, TFT_GREEN);
  tft.drawString("Com Carga", 160, 60, 2);
  delay(200);
  }else 
    {
    // mostra o icone do sem wifi 
    //tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawXBitmap(170, 5, iconTomada48, logoWidth48, logoHeight48, TFT_WHITE, TFT_RED);
    tft.drawString("Sem Carga", 160, 60, 2);
    delay(200);
  }
}

