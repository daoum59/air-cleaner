#include <MECHA_PMS5003ST.h>          // 미세먼지센서 라이브러리
#include <SoftwareSerial.h>           // digital pin 라이브러리
#include <Wire.h>                     // i2C 통신을 위한 라이브러리
#include <LiquidCrystal_I2C.h>        // LCD 1602 I2C용 라이브러리
#include <String.h>

// uno port 뒤

#define relay 12 // touch sensor
#define good 11  // B
#define nomal 10 // G
#define bad 9    // R
#define tsen 6   // touch sensor

LiquidCrystal_I2C lcd(0x27,20,4); // 접근주소: 0x3F or 0x27

SoftwareSerial dust(2,3);   // 2, 3번핀을 통해 미세먼지센서와 시리얼 통신
SoftwareSerial wifi(4,5);   // 4, 5번핀을 통해 D1R2보드와 시리얼 통신
MECHA_PMS5003ST pms(&dust); // 미세먼지센서 라이브러리
 
int tcnt = 0;   // touch count
int last = 0;   // before state
int lastpm = 0;   // before state
int lasttm = 0;   // before state
int lasthm = 0;   // before state

void setup() {
  last = 0;
  tcnt = 1;
  lcd.init();             // lcd 초기화
  lcd.backlight();        // 백라이트 켜기
  digitalWrite(relay, HIGH);
  Serial.begin(115200);   // 통신속도 11520 baud 설정
  wifi.begin(9600);     // 통신속도 11520 baud 설정
  pms.begin();            //
//  pms.wakeUp();
 
  pms.setMode(PASSIVE);   // PASSIVE 모드로 전환
  pinMode(good, OUTPUT);  // D11
  pinMode(nomal, OUTPUT); // D10
  pinMode(bad, OUTPUT);   // D9
  pinMode(tsen, INPUT);   // D6
  pinMode(relay, OUTPUT);
  wifi.print("on");
}
 
void loop() {
  pms.request();        //센서 데이터 요청
  if(!pms.read()){      //센서 데이터 읽기
    return;             //실패 재시도
  }

  sendData();
  touch();
  ledswitch();
  setlcd();
  delay(1000);
}

void touch() {
  int value = digitalRead(tsen);   // 터치가 되었는지 안도
  String power = wifi.readString();  
  if(value != last && value == 1 || wifi.available() > 0){ // 터치가 되었을 때
    if (tcnt == 1 || power == "Off"){                // 켜져있으면 끄기
      tcnt--;
      lcd.noDisplay();
      lcd.noBacklight();
      digitalWrite(relay, LOW);
      wifi.print("off");
    }
    else if (tcnt == 0 || power == "On"){           // 꺼져있으면 켜기
      tcnt++;
      lcd.display();
      lcd.backlight();
      digitalWrite(relay, HIGH);
      wifi.print("on");
    }
  }
  delay(100);
  last = value;                    // 현재 상태를 다음 loop에 사용
} 

void ledswitch()
{
  if(tcnt == 1)
  {
    if(pms.getPmCf1(10)<=10){ // PM10 기준 LED 지시(기준 변경 가능), 먼지 좋음 파란색 발광
      digitalWrite(good, 1);
      digitalWrite(nomal, 0);
      digitalWrite(bad, 0);
    }
    else if(pms.getPmCf1(10)>10&&(pms.getPmCf1(10)<=12)){ //먼지 보통 초록색 발광
      digitalWrite(good, 0);
      digitalWrite(nomal, 1);
      digitalWrite(bad, 0);
    }
      else if(pms.getPmCf1(10)>12&&(pms.getPmCf1(10)<=14)){ //먼지 나쁨 노란색 발광
      digitalWrite(good, 0);
      digitalWrite(nomal, 1);
      digitalWrite(bad, 1);
    }
    else if(pms.getPmCf1(10)>15){ //먼지 매우나쁨 빨간색 발광
      digitalWrite(good, 0);
      digitalWrite(nomal, 0);
      digitalWrite(bad, 1);
    }
  }
  else if(tcnt == 0){
    digitalWrite(good, 0);
    digitalWrite(nomal, 0);
    digitalWrite(bad, 0);
  }
}

void setlcd()                         // lcd에 미세먼지, 온습도 표시
{  
  int pm10 = pms.getPmCf1(10);        // 처음 pm10농도 저장
  int temp = pms.getTemp();           // 처음 온도 저장
  int humi = pms.getHumi();           // 처음 습도 저장

  lcd.setCursor(0,0);                 // 1번째, 1라인
  // 자릿수가 내려갈때 이전 txt가 남아있어서 지워주기위함                                      
  if (pm10 != lastpm){
    if (lastpm >= 10 && pm10 < 10){    
      lcd.init();
      lcd.print(pms.getPmCf1(10));    // 미세먼지pm10 농도
      lcd.print("ug/m3");
    }
    else if (lastpm >= 100 && pm10 < 100){
      lcd.init();
      lcd.print(pms.getPmCf1(10));    
      lcd.print("ug/m3");
    }
  } else{
    lcd.print(pms.getPmCf1(10));      // 미세먼지pm10 농도
    lcd.print("ug/m3");
  }

  lcd.setCursor(0,1);                 // 1번째, 2라인
  if (temp != lasttm || humi != lasthm){
    if (lasttm >= 10 && temp < 10){    
      lcd.init();
      lcd.print(pms.getTemp());           // 온도
      lcd.print("'C ");
      lcd.print(pms.getHumi());           // 습도
      lcd.print("%");
    } 
    else if (lasttm >= 100 && temp < 100){
      lcd.init();
      lcd.print(pms.getTemp());           // 온도
      lcd.print("'C ");
      lcd.print(pms.getHumi());           // 습도
      lcd.print("%");
    }
    else if (lasthm >= 10 && humi < 10){
      lcd.init();
      lcd.print(pms.getTemp());           // 온도
      lcd.print("'C ");
      lcd.print(pms.getHumi());           // 습도
      lcd.print("%");
    }
    else if (lasthm >= 100 && humi < 100){
      lcd.init();
      lcd.print(pms.getTemp());           // 온도
      lcd.print("'C ");
      lcd.print(pms.getHumi());           // 습도
      lcd.print("%");
    }
  } else{
    lcd.print(pms.getTemp());           // 온도
    lcd.print("'C ");
    lcd.print(pms.getHumi());           // 습도
    lcd.print("%");
  }

  lastpm = pm10;
  lasttm = temp;
  lasthm = humi;
}

void sendData()
{
  wifi.print(pms.getPmCf1(10));
  wifi.print(",");
  wifi.print(pms.getTemp());
  wifi.print(",");
  wifi.print(pms.getHumi());
  wifi.println(""); 
  //wifi.flush()
}

/*
void readData()
{
  Serial.print("PM1.0(CF1)\t"); //PM1.0 측정
  Serial.print(pms.getPmCf1(1.0));
  Serial.print("ug/m3");
  Serial.println();
 
  Serial.print("PM2.5(CF1)\t"); //PM2.5 측정
  Serial.print(pms.getPmCf1(2.5));
  Serial.print("ug/m3");
  Serial.println();
 
  Serial.print("PM10(CF1)\t"); //PM10 측정
  Serial.print(pms.getPmCf1(10));
  Serial.print("ug/m3");
  Serial.println();
  
  Serial.print("Formalde\t"); //포름알데히드 측정
  Serial.print(pms.getForm());
  Serial.print("ug/m3");
  Serial.println();
 
  Serial.print("Temperat\t"); //온도 측정
  Serial.print(pms.getTemp());
  Serial.print("'C");
  Serial.println();
 
  Serial.print("Humidity\t"); //습도 측정
  Serial.print(pms.getHumi());
  Serial.print("%");
  Serial.println();
  Serial.println();
}
*/