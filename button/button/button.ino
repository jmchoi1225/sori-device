#include <OneButton.h>

const int buttonPin=2;

OneButton button(buttonPin, true);
unsigned long pressedTimer = 0;

void setup() {
  Serial.begin(9600);
  button.attachDoubleClick(doubleClick);
  button.attachLongPressStart(longPressStart);
  button.attachLongPressStop(longPressStop);
}

void loop() {
  button.tick();
  delay(10);
}

void doubleClick(){
  Serial.println("Double Click");
}

void longPressStart(){
  Serial.println("Long Press Start");
  pressedTimer = millis();
}

void longPressStop(){
  Serial.println("Long Press Stop");
  Serial.println(millis() - pressedTimer);
}
