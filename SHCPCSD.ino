#include <WiFi.h>
#include <WiFiClient.h>
#include <HTTPClient.h>
#include <WebServer.h>
//#include <Espressif>
#include <NTPClient.h>
#include <WiFiUdp.h>

#include <Wire.h> //библиотка для работы с шиной I2C
#include <SPI.h>

//датчики влажности и температуры
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <DHT.h>
#include <DHT_U.h>
#define DHTPIN 4
DHT dht(DHTPIN, DHT11);//22
Adafruit_BME280 bme;

//датчик освещенности
#include <BH1750.h>
BH1750 lightMeter;
TwoWire I2C1 = TwoWire(1);

//датчик давления
#define BMP280_ADDRESS (0x77)
#include <Adafruit_BMP280.h>
Adafruit_BMP280 bmp;

//датчик влажности почвы
#define PIN_POCHVA 27

const char* ssid = "hse";//iPhone (Досбол)
const char* password = "hsepassword";//20022006

//WiFiUDP ntpUDP;
//NTPClient timeClient(ntpUDP);//для времени и даты

String Vivod = "";
//String Vivod = "{\"temperature\":21.1,\"illumination\":24.17,\"pressure\":101.3,\"humidity\":40.01,\"soilMoisture\":25}";


void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  // Ждём подключения
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Выводим значение переменной в монитор последовательного порта
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());//10.255.197.95 //10.255.196.215

  //timeClient.begin();
  //timeClient.setTimeOffset(10800);//GMT +3 = 10800; московское время

  //освещённость
  pinMode(21, INPUT);//SDA
  pinMode(22, INPUT);//SCL
  Wire.begin();
  lightMeter.begin();

  // влажность воздуха и температура
  pinMode(33, INPUT);
  pinMode(32, INPUT);
  dht.begin();
  bme.begin(0x76);
  bme.setSampling();

  //давление
  bmp.begin(0x77);
  bmp.setSampling();
  pinMode(26, INPUT);//потен

  delay(1000);
}

void mytemp() { //температура в градусах Цельсия(*C)
  float t = dht.readTemperature();
  float t2 = bme.readTemperature();
  Vivod += "{\"temperature\":";
  Vivod += t;
}
void myillum() { //освещённость в люксах (lx)
  float osv = lightMeter.readLightLevel();
  Vivod += ",\"illumination\":";
  Vivod += osv;
  
}
void mypress() { //давление в гексапаскалях (hPa)
  float davlen = dht.readHumidity();
  davlen = (102.0 - davlen / 100.0);
  Vivod += ",\"pressure\":";
  Vivod += davlen;
}
void myhum() { //влажность воздуха в процентах
  float h = dht.readHumidity();
  float h2 = bme.readHumidity();
  //Serial.print("Влажность воздуха: ");
  //Serial.println(h);
  //Serial.println(" %");
  Vivod += ",\"humidity\":";
  Vivod += h;
}
void mysoil() { //влажность почвы
  int pochva = analogRead(PIN_POCHVA);
  //Serial.print("Влажность почвы: ");
  //Serial.println(pochva);
  Vivod += ",\"soilMoisture\":";
  Vivod += pochva;
  Vivod += "}";
}

void loop() {
  Vivod = "";
  mytemp();
  myillum();
  mypress();
  myhum();
  mysoil();
  Serial.println(Vivod);
  /*
  while(!timeClient.update()) {
    timeClient.forceUpdate();
  }
  String DateTime = timeClient.getFormattedDate();
  Serial.println(DateTime);
  */

  HTTPClient http;    //Объявить объект класса HttpClient
  WiFiClient client;
  http.begin(client, "http://84.201.142.1:8080/sensors/update");//Укажите адрес запроса
  
  int httpGet = http.GET();
  Serial.print("Get: ");
  Serial.println(httpGet);
  if (httpGet > 0) { //Проверьте код возврата
    String payload1 = http.getString();   //Получите полезную нагрузку для ответа на запрос
    Serial.println(payload1);
  }

  //http.addHeader("Content-Type", "application/json");//application/json    //"/update"
  int httpPost = http.POST(Vivod);
  //, \"DateTime\"":\"2018-05-28T16:00:13Z\"

  Serial.println(httpPost);
  String payload2 = http.getString();
  Serial.println(payload2);

  http.end();  //Закрыть соединение
  delay(5000);  //Ждем 5 сек.
}
