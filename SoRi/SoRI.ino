#include <SoftwareSerial.h>//시리얼 통신 라이브러리

#define BT_TX 13 //블루투스 모듈 TX핀 8번
#define BT_RX 12 //블루투스 모듈 TX핀 7번

int chotrig1 = 11, choecho1 = 10; //꽂는 번호에 따라 바꾸기(초음파)
int chotrig2 = 9, choecho2 = 8;
int mode;//가변저항 모드 설정
int check = 0; //2번 꾹 누르고 있으면 안되게
int button = 7; //꽂는 번호에 따라 바꾸기(버튼)

SoftwareSerial BT(BT_TX, BT_RX);//블투 TX,RX설정

void setup() {
  pinMode(chotrig1, OUTPUT); //아두이노가 초음파에 전력(신호)주기
  pinMode(choecho1, INPUT); //아두이노가 초음파에게 전력(신호)받기
  pinMode(chotrig2, OUTPUT); //아두이노가 초음파에 전력(신호)주기
  pinMode(choecho2, INPUT); //아두이노가 초음파에게 전력(신호)받기
  pinMode(button, INPUT); //아두이노가 버튼에게 신호 받기

  mode = 0; //가변저항 모드

    
  Serial.begin(9600);//시리얼 통신 속도
  BT.begin(9600); //블루투스 통신도 동일하게 설정
}


void loop() {
  
  int vr = analogRead(A0);     //가변저항 아날로그핀에 꽂아야함(A0)
  vr = map(vr, 0, 1023, 0, 255);
  int cho_distance1=0;
  int cho_distance2=0;
  String msg = "";
  //Serial.println("sdfads");
  if (vr < 64) mode = 0;  
  else if (vr < 128) mode = 1;
  else if (vr < 192) mode = 2;
  else if (vr < 256) mode = 3;
  //Serial.print("dsfdsaf");
  //가변저항으로 모드 설정 끝

  if(mode == 0){ //0번 모드 라이다 센서 쭉 적힘
    if(digitalRead(button) == HIGH){
        //Serial.print(mode);
        //BT.print(mode);
        msg += mode;
        //Serial.print(",");
        //BT.print(",");
        msg += ",";
        delay(20);
         
        digitalWrite(chotrig2, HIGH); //초음파
        delayMicroseconds(10);
        digitalWrite(chotrig2, LOW); 
        cho_distance2 = pulseIn(choecho2, HIGH)*340/2/10000;
        //Serial.print(cho_distance2);
        //BT.print(cho_distance2);
        //BT.print(",");
        //BT.println("0");
        msg += cho_distance2;
        msg += ",0";
        BT.println(msg);
    }
    delay(10);
  }
  
    if(mode == 1){ //1번 모드, 일단은 0번이랑 같지만 혹시 몰라 적어놈
      if(digitalRead(button) == HIGH){
        //Serial.print(mode);
        //BT.print(mode);
        msg += mode;
        //Serial.print(",");
        //BT.print(",");
        msg += ",";
        delay(20);
         
        digitalWrite(chotrig2, HIGH); //초음파
        delayMicroseconds(10);
        digitalWrite(chotrig2, LOW); 
        cho_distance2 = pulseIn(choecho2, HIGH)*340/2/10000;
        //Serial.println(cho_distance2);
        //BT.print(cho_distance2);
        //BT.print(",");
        //BT.println("0");
        msg += cho_distance2;
        msg += ",0";
        BT.println(msg);
      }
      delay(10);
    }
    
    if(mode == 2){ //2번모드 꾹 누르고 있음 신호 없음

      if(digitalRead(button) == HIGH) {
        //Serial.print(mode);
        //BT.print(mode);
        //Serial.print(",");
        //BT.print(",");
        msg += mode;
        msg += ",";
        delay(20);
         
        digitalWrite(chotrig2, HIGH); //초음파
        delayMicroseconds(10);
        digitalWrite(chotrig2, LOW); 
        cho_distance2 = pulseIn(choecho2, HIGH)*340/2/10000;
        //Serial.println(cho_distance2);
        //BT.print(cho_distance2);
        //BT.print(",");
        //BT.println("0");
        msg += cho_distance2;
        msg += ",0";
        BT.println(msg);
        delay(2000);
      }      
    }

  if(mode == 3){ //3번모드 라이다랑 초음파 순으로 센서값 적힘
    
    //Serial.print(mode);
    //BT.print(mode);
    //Serial.print(",");
    //BT.print(",");
    msg += mode;
    msg += ",";
    delay(20);
         
    digitalWrite(chotrig2, HIGH); //초음파
    delayMicroseconds(10);
    digitalWrite(chotrig2, LOW); 
    cho_distance2 = pulseIn(choecho2, HIGH)*340/2/10000;
    //Serial.print(cho_distance2);
    //BT.print(cho_distance2);
    msg += cho_distance2;
    
    //Serial.print(",");
    //BT.print(",");
    msg += ",";

    delay(20);
    
    digitalWrite(chotrig1, HIGH); //초음파
    delayMicroseconds(10);
    digitalWrite(chotrig1, LOW);
 
    cho_distance1 = pulseIn(choecho1, HIGH)*340/2/10000;

    //Serial.println(cho_distance1);
    //BT.println(cho_distance1);
    msg += cho_distance1;
    BT.println(msg);  
  }

  delay(100);
}
