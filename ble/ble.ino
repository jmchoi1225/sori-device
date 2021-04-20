#include <ArduinoBLE.h>

const char* nameOfPeripheral = "SORI_BLE";
const char* uuidOfService = "0000181a-0000-1000-8000-00805f9b34fb";
const char* uuidOfRxChar = "00002A3D-0000-1000-8000-00805f9b34fb";
const char* uuidOfTxChar = "00002A58-0000-1000-8000-00805f9b34fb";

const int RX_BUFFER_SIZE = 256;
const bool RX_BUFFER_FIXED_LENGTH = false;
const int TX_BUFFER_SIZE = 256;
const bool TX_BUFFER_FIXED_LENGTH = false;


BLEService dataService(uuidOfService);
BLECharacteristic rxChar(uuidOfRxChar, BLEWriteWithoutResponse | BLEWrite, RX_BUFFER_SIZE, RX_BUFFER_FIXED_LENGTH);
BLECharacteristic txChar(uuidOfTxChar, BLERead | BLENotify, TX_BUFFER_SIZE, TX_BUFFER_FIXED_LENGTH);

void setup() {
  startBLE();

  // Create BLE service and characteristics.
  BLE.setDeviceName(nameOfPeripheral);
  BLE.setLocalName(nameOfPeripheral);
  BLE.setAdvertisedService(dataService);
  dataService.addCharacteristic(rxChar);
  dataService.addCharacteristic(txChar);

  BLE.addService(dataService);

  // Bluetooth LE connection handlers.
  BLE.setEventHandler(BLEConnected, onBLEConnected);
  BLE.setEventHandler(BLEDisconnected, onBLEDisconnected);

  // Event driven reads.
  rxChar.setEventHandler(BLEWritten, onRxCharValueUpdate);
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
  

  Serial.println("Bluetooth device active, waiting for connections...");
}

void loop() {
  BLE.poll();
  BLEDevice central = BLE.central();
  
  if (central)
  {
    // Only send data if we are connected to a central device.
    while (central.connected()) {
      if (Serial.available()>0) {
        String res = Serial.readString();
        Serial.println("Sending : " + res);
        uint8_t data[256];
        res.getBytes(data, res.length());
        txChar.writeValue(data, res.length());
      }
    }
  }
}

void startBLE() {
  if (!BLE.begin())
  {
    Serial.println("starting BLE failed!");
    while (1);
  }
}

void onRxCharValueUpdate(BLEDevice central, BLECharacteristic characteristic) {
  String data = reinterpret_cast<const char*>(rxChar.value());
  Serial.println("Received : " + data.substring(0,rxChar.valueLength()));
}

void onBLEConnected(BLEDevice central) {
  Serial.print("Connected event, central: ");
  Serial.println(central.address());
}

void onBLEDisconnected(BLEDevice central) {
  Serial.print("Disconnected event, central: ");
  Serial.println(central.address());
}
