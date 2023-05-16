#include <Wire.h> //библиотка для работы с шиной I2C
#include <SPI.h>
//датчики влажности и температуры
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <DHT.h>
#include <DHT_U.h>
#define DHTPIN 7
DHT dht(DHTPIN, DHT11);//22
Adafruit_BME280 bme;

//датчик освещённости
#include <BH1750.h>
BH1750 lightMeter;

//датчик давления
#define BMP280_ADDRESS (0x77)
#include <Adafruit_BMP280.h>
Adafruit_BMP280 bmp;

//датчик влажности почвы
#define PIN_POCHVA A0

void setup() {
  Serial.begin(9600);
  
  // влажность воздуха и температура
  pinMode(A2, INPUT);
  pinMode(A3, INPUT);
  dht.begin();
  bme.begin(0x76);
  bme.setSampling();
  // освещённость
  pinMode(A4, INPUT);
  pinMode(A5, INPUT);
  Wire.begin();
  lightMeter.begin();
  //давление
  bmp.begin(0x77);
  bmp.setSampling();
  pinMode(A1, INPUT);//потен
  
  delay(1000);
}

void mytemp() { //температура
  float t = dht.readTemperature();
  float t2 = bme.readTemperature();
  Serial.print("Температура = ");
  Serial.print(t);
  Serial.println(" *C");
}
void myillum() { //освещённость
  float osv = lightMeter.readLightLevel();
  Serial.print("Освещение: ");
  Serial.print(osv);
  Serial.println(" lx");
}
void mypress() { //давление в гексапаскалях
  float davlen = dht.readHumidity();
  Serial.print("Давление = "); 
  Serial.print(102.0 - davlen / 100.0);
  Serial.println(" hPa");
}
void myhum() { //влажность воздуха
  float h = dht.readHumidity();
  float h2 = bme.readHumidity();
  Serial.print("Влажность воздуха: ");
  Serial.print(h);
  Serial.println(" %");
}
void mysoil() { //влажность почвы
  int pochva = analogRead(PIN_POCHVA);
  Serial.print("Влажность почвы: ");
  Serial.println(pochva);
}

void loop() {
  mytemp();
  myillum();
  mypress();
  myhum();
  mysoil();
  delay(1000);
}
