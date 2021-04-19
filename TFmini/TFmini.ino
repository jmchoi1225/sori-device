#include <DFRobot_TFmini_Nano.h>

TFmini  TFmini;
uint16_t distance,strength;
unsigned long lastPrint = 0;
unsigned long const DELAY_TIME = 1000;

void setup(){
  Serial.begin(9600);
  TFmini.begin(Serial1);
}

void loop(){
  if(TFmini.measure()){
    if(millis() - lastPrint > DELAY_TIME){
      printResult();
    }
  }
}

void printResult(){
  distance = TFmini.getDistance();       //Get distance data
  strength = TFmini.getStrength();       //Get signal strength data
  Serial.print("Distance = ");
  Serial.print(distance);
  Serial.println("mm");
  Serial.print("Strength = ");
  Serial.println(strength);
  lastPrint = millis();
}
