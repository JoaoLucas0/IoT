#include "EmonLib.h" 
#include <WiFi.h>                        
#include <IOXhop_FirebaseESP32.h>          
#include <ArduinoJson.h>    
EnergyMonitor SCT013;

const int ledPin = 5;      // Define o pino para o LED
const int ldrPin = 35;    // Define o pino para o LDR (sensor de luz)
const int pirPin = 15;   // Define o pino para o sensor PIR
const int pinSCT = 34;  // Define o pino para o sensor de corrente SCT013

#define WIFI_SSID  ""
#define WIFI_PASSWORD ""
#define FIREBASE_HOST "https://trabalhodeiot-default-rtdb.firebaseio.com/"   
#define FIREBASE_AUTH "V6A4nxESZ1wckecptSD9vCZhaQ0EG8eigCkyEbUn"   

const int tensao = 230;                  // Define o valor da tensão de alimentação (em Volts)
const unsigned long delayTime = 15000;  // Define o tempo de espera sem movimento antes de desligar o LED (em milissegundos)

unsigned long lastMotionTime = 0;  // Variável para armazenar o tempo da última detecção de movimento
int potencia = 0;                 // Variável para armazenar a potência calculada
int ldrValue = 0;                // Variável para armazenar o valor lido pelo LDR
int pirState = 0;               // Variável para armazenar o estado do sensor PIR
float current = 0;             // Variável para armazenar a corrente medida


void setup() {

  pinMode(ledPin, OUTPUT);           // Configura o pino do LED como saída
  pinMode(pirPin, INPUT);           // Configura o pino do sensor PIR como entrada
  SCT013.current(pinSCT, 1.055);   // Inicializa o sensor de corrente SCT013 com um fator de calibração
  Serial.begin(9600);             // Inicializa a comunicação serial com taxa de 9600 bps
  
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);     
  Serial.print("Conectando ao wifi"); 
  
  while (WiFi.status() != WL_CONNECTED)  
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println(); 
  
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

String input = Firebase.getString("input/");// puxar o input do firebase que só vem em string

int inputInt = input.toInt(); //o firebase só estava aceitando em string ai coloquei essa função pra converter pra int


void loop() {

  Serial.println(inputInt);
  tarefa1();     // Chama a função que calcula a corrente e a potência
  tarefa2();    // Chama a função que lida com o sensor PIR e o LED
}

void tarefa1() {

  double Irms = SCT013.calcIrms(1480);   // Calcula a corrente RMS medida pelo sensor SCT013
  potencia = Irms * tensao;             // Calcula a potência utilizando a corrente medida e a tensão definida
  
  Serial.print("CORRENTE = ");           // Imprime a mensagem de corrente na porta serial
  Serial.print(Irms);                   // Imprime o valor da corrente
  Serial.println(" (A)");              // Imprime a unidade de corrente (Amperes)
  Firebase.setFloat("/sc13/Irms", Irms); // altera o valor da Irms
  
  Serial.print("POTÊNCIA = ");           // Imprime a mensagem de potência na porta serial
  Serial.print(potencia);               // Imprime o valor da potência
  Serial.println(" (W)");              // Imprime a unidade de potência (Watts)
  Firebase.setFloat("/sc13/potencia", potencia); // altera o valor da potencia no firebase
  delay(1000);                        // Aguarda por um segundo

}

void tarefa2() 
{
  pirState = digitalRead(pirPin);  // Lê o estado do sensor PIR
  Firebase.setBool("/sensor/pir", false);
  if (pirState == HIGH) 
  {                               // Se o sensor PIR detectar movimento
    ldrValue = analogRead(ldrPin);                     // Lê o valor do sensor LDR
    int brightness = map(ldrValue, 0, 4095, 255, 0);  // Mapeia o valor do LDR para ajustar a luminosidade do LED
    analogWrite(ledPin, brightness);  // Define a luminosidade do LED
    Firebase.pushBool("/sensor/pir", true);  // registra que ouve presença
    
    Serial.print("LDR Value: ");
    Serial.println(ldrValue);           // Imprime o valor lido pelo sensor LDR
    Firebase.setFloat("/sensor/LDR", ldrValue);  // altera o valor do sensor de LDR pra um novo
    Serial.print("LED Brightness: ");  // Imprime a luminosidade do LED na porta serial
    Serial.println(brightness);       // Imprime o valor da luminosidade do LED
    lastMotionTime = millis();       // Registra o tempo do último movimento detectado
  } 
  
    else 
    {
    if (millis() - lastMotionTime >= delayTime) 
      {  // Se não houver movimento após um determinado período de tempo
      analogWrite(ledPin, 0);                     // Desliga o LED
      Firebase.pushBool("/sensor/presença", false); // registra que o sensor de presença foi "deligado" 
      }
    }
}

