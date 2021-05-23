const int batt_max = 860; // 860/1024*5 = 4.199
const int batt_min = 615; // 615/102*5 = 3.00
const int batteryPin = 2;
const long batteryDuration = 60000;
const int batterySampleCount = 10;
int batteryPercent = 100;

void setup() {
  Serial.begin(9600);
  pinMode(batteryPin, OUTPUT);
}

void loop() {
  Serial.println(getBattery());
}


int getBattery(){
  int vSum = 0;
  for(int i =0; i<batterySampleCount; i++){
    vSum += analogRead(batteryPin);
  }
  int v = vSum / batterySampleCount;
  
  int b = map(v,batt_min,batt_max,0,100);
  if (v<batt_min) b=0;
  if (v>batt_max) b=100;
  return b;
}
