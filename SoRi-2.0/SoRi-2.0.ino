#include <Sori_Serial.h>
SoriSerial SoriSerial;
double duration = 0.05;

void setup() {
  SoriSerial.begin();
}

void loop() {
  SoriSerial.poll();

  if(SoriSerial.messageReceived()){
    Serial.println("Received : " + SoriSerial.read());
  }

  if(Serial.available()>0){
    String res = Serial.readString();
    SoriSerial.write(res);
  }
}
