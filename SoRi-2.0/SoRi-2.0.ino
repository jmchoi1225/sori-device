#include <OneButton.h>
#include <DFRobot_TFmini_Nano.h>
#include <ArduinoBLE.h>


TFmini TFmini;
unsigned long duration = 100; // millisecond
const unsigned long speechDuration = 2000;

const int numOfModes = 3;
const int modePin = 2;
const unsigned long pressedTime = 1000;
OneButton button(modePin, true);
int mode = 0;
boolean keep = false;
boolean play = false;
unsigned long pressedTimer = 0;

const int batt_max = 860;            // 860/1024*5 = 4.199
const int batt_min = 615;            // 615/102*5 = 3.00
const unsigned long batteryDuration = 120000;
const int batteryPin = 2;
unsigned long batteryTimer = 0;
const int batterySampleCount = 10;
int batteryPercent = 100;

bool isSpeech = false;
unsigned long lastWrite = 0;

void setup() {
  Serial.begin(9600);
  soriSerialBegin();
  TFmini.begin(Serial1);
  
  button.attachDoubleClick(doubleClick);
  button.attachLongPressStart(longPressStart);
  button.attachLongPressStop(longPressStop);
}

void loop() {
  TFmini.measure();
  button.tick();
  soriSerialPoll();
  
  if(soriSerialIsMessageReceived()){
    String message = soriSerialRead();
    Serial.println("Received : " + message);
    duration = message.toInt();
  }

  if(!soriSerialIsConnected()) {
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

int getMode(){
  if(!play){
    return -1;
  }
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

void doubleClick(){
  mode = (mode+1)%numOfModes;
  Serial.print("Change Mode : ");
  Serial.println(mode);
}

void longPressStart(){
  Serial.println("Long Press Start");
  play = true;
  pressedTimer = millis();
}

void longPressStop(){
  Serial.println("Long Press Stop");
  if(millis() - pressedTimer > pressedTime){
    keep = false;
  }else{
    keep = !keep;
  }
  play = keep;
  Serial.print("Keep : ");
  Serial.println(keep);
  Serial.print("Play : ");
  Serial.println(play);
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
