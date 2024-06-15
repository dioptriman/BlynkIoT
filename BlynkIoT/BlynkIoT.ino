#define BLYNK_TEMPLATE_ID "TMPL6D_txRXST" //Tulis nama template ID Blynk
#define BLYNK_TEMPLATE_NAME "Wheather Station" //Tulis nama template Blynk
#define BLYNK_AUTH_TOKEN "4eETHw0mNrBQLEqPSBjUzzhJk3BLY-37" //Tulis token Blynk 


#define BLYNK_PRINT Serial //Kita menggunakan Blynk serial


#include <WiFi.h> //Library WiFi
#include <WiFiClient.h> //Library WiFiClient
#include <BlynkSimpleEsp32.h> //Library BlynkESP32


#include "DHT.h"
#include <LiquidCrystal_I2C.h>

#define ANALOG_SOIL 38
#define DIGITAL_SOIL 37
#define TRIG 48
#define ECHO 47
#define SDA 8
#define SCL 9
#define REL_1 41
#define REL_2 45
#define REL_3 21
#define DHTPIN 46


#define button1_vpin    V1
#define button2_vpin    V2
#define button3_vpin    V3 


#define DHTTYPE DHT22 

DHT dht(DHTPIN, DHTTYPE);

// set the LCD number of columns and rows
int lcdColumns = 20;
int lcdRows = 4;

LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  

char ssid[] = "Wokwi-GUEST"; //Nama WiFi yang digunakan
char pass[] = ""; //Password WiFi yang digunakan

BlynkTimer timer;

float t;
float h;
float moisture;
float distance;

int relay1_state = 0;
int relay2_state = 0;
int relay3_state = 0;

BLYNK_CONNECTED() {
  Blynk.syncVirtual(button1_vpin);
  Blynk.syncVirtual(button2_vpin);
  Blynk.syncVirtual(button3_vpin);
}

BLYNK_WRITE(button1_vpin) {
  relay1_state = param.asInt();
  digitalWrite(relay1_pin, relay1_state);
}

BLYNK_WRITE(button2_vpin) {
  relay2_state = param.asInt();
  digitalWrite(relay2_pin, relay2_state);
}

BLYNK_WRITE(button3_vpin) {
  relay3_state = param.asInt();
  digitalWrite(relay3_pin, relay3_state);
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  //DHT
  dht.begin();

  //RELAY
  pinMode(REL_1, OUTPUT);
  pinMode(REL_2, OUTPUT);
  pinMode(REL_3, OUTPUT);


  //HC-SR04
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  // initialize LCD
  lcd.init();
  // turn on LCD backlight                      
  lcd.backlight();

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  timer.setInterval(1000L, myTimerEvent); 
}



void loop() {
  // put your main code here, to run repeatedly:
  Blynk.run();
  timer.run();

  readHigh();
  readAir();
  readMoist();

  if(t > 25 || moisture < 15 || h < 15 || distance > 100){
    relay1_state = 1;
    digitalWrite(REL_1, relay1_state);
    Blynk.virtualWrite(button1_vpin, relay1_state);

  }

  lcd.setCursor(0, 0);
  lcd.print(t);
  lcd.setCursor(0, 1);
  lcd.print(h);
  lcd.setCursor(0, 2);
  lcd.print(moisture);
  lcd.setCursor(0, 3);
  lcd.print(distance);
  lcd.clear();

  delay(1000);
  Blynk.virtualWrite(V0, suhu); //Mengirim data suhu menurut pin V0 di Blynk
  Blynk.virtualWrite(V1, kelembaban); //Mengirim data kelembaban pin V1

}

void readHigh(){
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  float duration = pulseIn(ECHO, HIGH);
  float distance = (duration*.0343)/2;
  Serial.print("Distance: ");
  Serial.println(distance);
  delay(100);
}

void readAir(){
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
}

void readMoist(){
  float sensor_analog = analogRead(ANALOG_SOIL);
  float moisture = ( 100 - ( (sensor_analog/4095.00) * 100 ) );
  delay(1000);
}
