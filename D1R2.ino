#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <String.h>
#define SSID "Lab 818"
#define PASSWORD "password"

//d1r2 port 앞

ESP8266WiFiMulti WiFiMulti;

SoftwareSerial uno(D1,D2);        // uno R3(TX,RX)

const char* ssid = SSID;
const char* pswd = PASSWORD;

unsigned long pre_timer = 0;  // 코드가 작동한 시점에서의 아두이노 가동시각 저장
int repeat_time = 1000;       // 반복시간 1초

String receive = "";
String send = "";
String pm10 = "";
String temp = "";
String humi = "";

void setup()
{
  Serial.begin(115200);
  uno.begin(9600);

  connectWifi();
  delay(500);
}

void loop()
{
  receiveData();
  getHTTP();
  delay(1000);
}

void receiveData()
{
  if(uno.available() <= 0)         // 데이터 유무체크, 없으면 빠져나감
    return;
  pm10 = uno.readStringUntil(','); // ',' 단위로 끊기
  temp = uno.readStringUntil(','); // ',' 단위로 끊기 
  humi = uno.readStringUntil(13);  // 공백이 있으면 끊기
  uno.flush();
}

void connectWifi(){       
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, pswd);
}

void getHTTP(){
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    WiFiClient client;
    HTTPClient http;

    String url = "http://113.198.236.238/process.php?pm10=";
    url += pm10 + "&temp=" + temp + "&humi=" + humi;
    Serial.println(url);
    http.begin(client, url);

    int httpCode = http.GET();
    if (httpCode > 0) {
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
        
      if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
        //String payload = http.getString();
        //Serial.println(payload);
      }
      } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    }
}