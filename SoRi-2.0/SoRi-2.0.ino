#include <DFRobot_TFmini_Nano.h>
#include <Sori_Serial.h>

SoriSerial SoriSerial;
TFmini TFmini;
double duration = 0.05;
double lastWrite = 0;
int TEST_mode = -1;

void setup() {
  Serial.begin(9600);
  SoriSerial.begin();
  TFmini.begin(Serial1);
}

void loop() {
  SoriSerial.poll();
  TFmini.measure();

  if(SoriSerial.messageReceived()){
    Serial.println("Received : " + SoriSerial.read());
  }

  if(millis() - lastWrite < duration) return;

  int mode = getMode();
  if(mode == -1) return;
  int distance = TFmini.getDistance();
  String message = buildMessage(mode, distance);
  SoriSerial.write(message);
  lastWrite = millis();
}

int getMode(){
  if(Serial.available()>0){
    TEST_mode = Serial.parseInt();
  }
  return TEST_mode;
}

String buildMessage(int mode, int distance){
  String message = "";
  message += mode;
  message += ",";
  message += distance;
  return message;
}
