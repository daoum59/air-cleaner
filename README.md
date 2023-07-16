# air-cleaner
아두이노기반 공기청정기

1. UNO보드
공기품질센서(PMS5003ST)에서 측정한 미세먼지농도(pm10)
온도('C ), 습도(%)를 측정 후 LCD, D!R2로 전달
```
#include <MECHA_PMS5003ST.h>          // 미세먼지센서 라이브러리
#include <SoftwareSerial.h>           // digital pin 라이브러리
#include <Wire.h>                     // i2C 통신을 위한 라이브러리
#include <LiquidCrystal_I2C.h>        // LCD 1602 I2C용 라이브러리
```

2. D1R2보드
UNO보드에서 전달받은 센서값을 서버를 통해서 db에 전달
```
#include <SoftwareSerial.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <String.h>
```
