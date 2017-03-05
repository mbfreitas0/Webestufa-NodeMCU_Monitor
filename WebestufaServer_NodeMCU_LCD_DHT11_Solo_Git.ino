//www.arduinesp.com
//
// Plot DTH11 data on thingspeak.com using an ESP8266
// April 11 2015
// Author: Jeroen Beemster
// Website: www.arduinesp.com

// Adaptação Display LCD16x2 I2C e Sensor Higrometro: Marcelo | Webestufa
// webestufa.blogspot.com.br
// webestufa.com.br

#include <DHT.h>
#include <ESP8266WiFi.h>
#include <Wire.h>  // This library is already built in to the Arduino IDE
#include <LiquidCrystal_I2C.h> //Biblioteca adicioada para utilizar o Display 

LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3,POSITIVE);

// Insira a sua API key, criada na geração do Canal (Write API Key)
String apiKey = "Sua API key";
const char* ssid = "Nome da sua rede";
const char* password = "Senha da sua rede";

const char* server = "api.thingspeak.com";
#define DHTPIN 2 // SO CONNECT THE DHT11/22 SENSOR TO PIN D4 OF THE NODEMCU

DHT dht(DHTPIN, DHT11,15); //CHANGE DHT11 TO DHT22 IF YOU ARE USING DHT22
WiFiClient client;

void setup() {
Serial.begin(115200);
pinMode(A0,INPUT);
delay(10);
dht.begin();


/*----------- Impressão Serial -----------*/
lcd.begin(16,2);   // initializing the LCD

WiFi.begin(ssid, password);

Serial.println();
Serial.println();
Serial.print("Connecting to ");
Serial.println(ssid);

WiFi.begin(ssid, password);

while (WiFi.status() != WL_CONNECTED) {
delay(500);
Serial.print(".");
}
Serial.println("");
Serial.println("WiFi connected");

}

void loop() {

float h = dht.readHumidity();
float t = dht.readTemperature();
float s = analogRead(A0); // Porta de comunicação com o Sensor de Umidade do Solo (Higrometro)




/*----------- Impressões no LCD 16x2 -----------*/
lcd.clear();
lcd.setCursor(3, 0);
lcd.print("WEBESTUFA"); // Start Print text to Line 1
lcd.setCursor(0, 1);      
lcd.print("Horta e Estufa"); // Start Print Test to Line 2
delay(2000);

lcd.clear();
lcd.setCursor(0,0);
lcd.print("Umidade: ");
lcd.print(h);
lcd.print("%");
lcd.setCursor(0,1);
lcd.print("Temper.:");
lcd.print(t);
lcd.write(B11011111);
delay(3000);

lcd.clear();
lcd.setCursor(0,0);
lcd.print("Solo: ");
lcd.print(s);
lcd.print(" %");
lcd.setCursor(0,1);
lcd.print("Enviando dados ");
delay(3000);
lcd.clear();



if (isnan(h) || isnan(t) || isnan(s)) { // checa as variáves para envio
Serial.println("Falha na leitura dos Sensores!");
return;
}

if (client.connect(server,80)) { // "184.106.153.149" or api.thingspeak.com
String postStr = apiKey;

// Converte as Variáveis para "String" para serem enviadas para o Canal

postStr +="&field1="; // Alimenta o campo Temperatura para geração do Gráfico
postStr += String(t);
postStr +="&field2="; // Alimenta o campo Umidade para geração do Gráfico
postStr += String(h);
postStr +="&field3="; // Alimenta o campo Umidade do Solo para geração do Gráfico
postStr += String(s); 
postStr += "\r\n\r\n";

client.print("POST /update HTTP/1.1\n");
client.print("Host: api.thingspeak.com\n");
client.print("Connection: close\n");
client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
client.print("Content-Type: application/x-www-form-urlencoded\n");
client.print("Content-Length: ");
client.print(postStr.length());
client.print("\n\n");
client.print(postStr);

Serial.print("Temperatura: ");
Serial.print(t);
Serial.write(B11011111);
Serial.print(" Umidade relativa do ar: ");
Serial.print(h);
Serial.print(" %");
Serial.print(" Umidade do solo: ");
Serial.print(s);
Serial.println("% send to Thingspeak");
}
client.stop();

Serial.println("Waiting…");
// thingspeak needs minimum 15 sec delay between updates
delay(20000);
}
