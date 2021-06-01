#include <DFRobot_TFmini_Nano.h>
#include <ArduinoBLE.h>

TFmini TFmini;
unsigned long duration = 10000; // millisecond
const unsigned long speechDuration = 3000;

const int numOfModes = 3;
const int modeButtons[numOfModes] = {2,3,4};

const int batt_max = 860;            // 860/1024*5 = 4.199
const int batt_min = 615;            // 615/102*5 = 3.00
const unsigned long batteryDuration = 120000;
const int batteryPin = 2;
unsigned long batteryTimer = 0;
const int batterySampleCount = 10;
int batteryPercent = 100;

bool isSpeech = false;
unsigned long lastWrite = 0;
int mode = -1;

void setup() {
  Serial.begin(9600);
  soriSerialBegin();
  TFmini.begin(Serial1);
  for(int i=0; i<numOfModes; i++){
    pinMode(modeButtons[i], INPUT);
  }
}

void loop() {
  TFmini.measure();

  soriSerialPoll();
  if(soriSerialIsMessageReceived()){
    String message = soriSerialRead();
    Serial.println("Received : " + message);
    duration = message.toInt();
  }

  if(!soriSerialIsConnected()) {
    pressed = false;
    return;
  }

  if(isBatteryTimeUp()){
    sendBatteryPercentage();
  }

  if(needToWait()){
    return;
  }
  
  lastWrite = millis();
  
  int mode = getMode();
  if(mode == -1) return;
  int distance = TFmini.getDistance();
  String message = buildMessage(mode, distance);
  Serial.println(message);
  soriSerialWrite(message);
  if(mode == 2){
    isSpeech = true;
  }
}

bool needToWait(){
  if(isSpeech){
    if(millis() - lastWrite < speechDuration){
      return true;
    }else{
      isSpeech = false;
    }
  }else{
    if(millis() - lastWrite < duration){
      return true;
    }
  }
  
  lastWrite = millis();
  return false;
}

int keepMode = -1;
int prevMode = -1;
boolean keep = false;
boolean pressed = false;
const unsigned long keepDuration = 3000;
unsigned long pressedTimer = 0;

int getMode(){
 int mode = -1;
 for(int i =0; i<numOfModes; i++){
  if(digitalRead(modeButtons[i])==LOW){
    mode = i;
  }
 }

  /*
  고려할 케이스 : 
  1. 버튼이 안눌렸을 때 : 
    - keepMode 출력
  2. 같은 버튼이 꾹 눌렸을 때 :
    3초 이상 눌렸을 때 : 
      keep : 
        keepMode = -1
      !keep : 
        keepMode = mode 
  3. keepMode와 다른 mode 눌렸을 때 :
    keepMode = -1
    keep = false;
    
  */

 if(mode == -1){ // not pressed -> return keep mode
  pressed = false;
  if(keepMode == -1){
    keep = false;
  }else{
    keep = true;
  }
  prevMode = mode;
  return keepMode;
 }

 if(keepMode != mode){ //pressed button different from keep mode -> keep off
  keepMode = -1;
  keep = false;
 }

 if(prevMode == mode){ // keep pressing same button
  if(!pressed){
    pressed = true;
    pressedTimer = millis();
  }
 }else{
  pressed = false;
 }

 if(pressed && millis() - pressedTimer > keepDuration){ //keep pressing the same button for more than 3 sec
    if(keep){
      keepMode = -1;
    }else{
      keepMode = mode;
    }
  }
 
 prevMode = mode;
 return mode;
}

boolean isBatteryTimeUp(){
  if(millis() - batteryTimer > batteryDuration){
    batteryTimer = millis();
    return true;
  }
  return false;
}

void sendBatteryPercentage(){
  int batteryPercent = getBattery();
  Serial.print("Battery : ");
  Serial.println(batteryPercent);
  soriBatteryWrite(batteryPercent);
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



String buildMessage(int mode, int distance){
  String message = "";
  message += mode;
  message += ",";
  message += distance;
  message += "\n";
  return message;
}

//-------------------------------------------------------
//SoriSerial

const char* nameOfPeripheral = "SORI_BLE";
const char* uuidOfService = "0000181a-0000-1000-8000-00805f9b34fb";
const char* uuidOfRxChar = "00002A3D-0000-1000-8000-00805f9b34fb";
const char* uuidOfTxChar = "00002A58-0000-1000-8000-00805f9b34fb";
const char* uuidOfBattery = "00002A73-0000-1000-8000-00805f9b34fb";

const int RX_BUFFER_SIZE = 256;
const bool RX_BUFFER_FIXED_LENGTH = false;
const int TX_BUFFER_SIZE = 256;
const bool TX_BUFFER_FIXED_LENGTH = false;

BLEService dataService(uuidOfService);
BLECharacteristic rxChar(uuidOfRxChar, BLEWriteWithoutResponse | BLEWrite, RX_BUFFER_SIZE, RX_BUFFER_FIXED_LENGTH);
BLECharacteristic txChar(uuidOfTxChar, BLERead | BLENotify, TX_BUFFER_SIZE, TX_BUFFER_FIXED_LENGTH);
BLECharacteristic battery(uuidOfBattery, BLERead | BLENotify, TX_BUFFER_SIZE, TX_BUFFER_FIXED_LENGTH);


const String EMPTY_MESSAGE = "";
String receivedMessage = EMPTY_MESSAGE;
bool messageReceived = false;
bool connected = false;


void soriSerialBegin(){
  if (!BLE.begin())
  {
    Serial.println("starting BLE failed!");
    while (true);
  }

    // Create BLE service and characteristics.
  BLE.setDeviceName(nameOfPeripheral);
  BLE.setLocalName(nameOfPeripheral);
  BLE.setAdvertisedService(dataService);
  dataService.addCharacteristic(rxChar);
  dataService.addCharacteristic(txChar);
  dataService.addCharacteristic(battery);

  BLE.addService(dataService);

  // Bluetooth LE connection handlers.
  BLE.setEventHandler(BLEConnected, onConnect);
  BLE.setEventHandler(BLEDisconnected, onDisconnect);

  // Event driven reads.
  rxChar.setEventHandler(BLEWritten, onReceiveMessage);

  receivedMessage = EMPTY_MESSAGE;
  messageReceived = false;
  connected = false;
    
  BLE.advertise();

  // Print out full UUID and MAC address.
  Serial.println("Peripheral advertising info: ");
  Serial.print("Name: ");
  Serial.println(nameOfPeripheral);
  Serial.print("MAC: ");
  Serial.println(BLE.address());
  Serial.print("Service UUID: ");
  Serial.println(dataService.uuid());
  Serial.print("rxCharacteristic UUID: ");
  Serial.println(uuidOfRxChar);
  Serial.print("txCharacteristics UUID: ");
  Serial.println(uuidOfTxChar);
  Serial.print("battery UUID: ");
  Serial.println(uuidOfBattery);

  Serial.println("Bluetooth device active, waiting for connections...");
}

void soriSerialPoll(){
  BLE.poll();
}

bool soriSerialIsMessageReceived(){
  return messageReceived;
}

String soriSerialRead(){
  if(!messageReceived){
    return EMPTY_MESSAGE;
  }
  messageReceived = false;
  return receivedMessage;
}

void soriBatteryWrite(int batteryPercent){
  battery.writeValue((uint8_t)batteryPercent);
}

void soriSerialWrite(String message){
  uint8_t data[256];
  message.getBytes(data, message.length());
  txChar.writeValue(data, message.length());
}

bool soriSerialIsConnected(){
  return connected;
}

void onConnect(BLEDevice central){
  Serial.println("Connected");
  sendBatteryPercentage();
  connected = true;
}

void onDisconnect(BLEDevice central){
  Serial.println("Disconnected");
  connected = false;
}

void onReceiveMessage(BLEDevice central, BLECharacteristic characteristic){
    String data = reinterpret_cast<const char*>(rxChar.value());
    receivedMessage = data.substring(0,rxChar.valueLength());
    messageReceived = true;
}
