#include <ArduinoBLE.h>

const char* nameOfPeripheral = "SORI";
const char* uuidOfService = "0000181a-0000-1000-8000-00805f9b34fb";
const char* uuidOfRxChar = "00002A3D-0000-1000-8000-00805f9b34fb";
const char* uuidOfTxChar = "00002A58-0000-1000-8000-00805f9b34fb";

const int RX_BUFFER_SIZE = 256;
bool RX_BUFFER_FIXED_LENGTH = false;

BLEService microphoneService(uuidOfService);
BLECharacteristic rxChar(uuidOfRxChar, BLEWriteWithoutResponse | BLEWrite, RX_BUFFER_SIZE, RX_BUFFER_FIXED_LENGTH);
BLEByteCharacteristic txChar(uuidOfTxChar, BLERead | BLENotify | BLEBroadcast);

short sampleBuffer[256];

// Number of samples read
volatile int samplesRead;

void setup() {
  startBLE();

  // Create BLE service and characteristics.
  BLE.setLocalName(nameOfPeripheral);
  BLE.setAdvertisedService(microphoneService);
  microphoneService.addCharacteristic(rxChar);
  microphoneService.addCharacteristic(txChar);
  BLE.addService(microphoneService);

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
  Serial.println(microphoneService.uuid());
  Serial.print("rxCharacteristic UUID: ");
  Serial.println(uuidOfRxChar);
  Serial.print("txCharacteristics UUID: ");
  Serial.println(uuidOfTxChar);
  

  Serial.println("Bluetooth device active, waiting for connections...");
}

void loop() {
BLEDevice central = BLE.central();
  
  if (central)
  {
    // Only send data if we are connected to a central device.
    while (central.connected()) {

      // Send the microphone values to the central device.
      if (samplesRead) {
        // print samples to the serial monitor or plotter
        for (int i = 0; i < samplesRead; i++) {
          txChar.writeValue(sampleBuffer[i]);      
        }
        // Clear the read count
        samplesRead = 0;
      }
    }
  } else {
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
  // central wrote new value to characteristic, update LED
  Serial.print("Characteristic event, read: ");
  byte tmp[256];
  int dataLength = rxChar.readValue(tmp, 256);

  for(int i = 0; i < dataLength; i++) {
    Serial.print((char)tmp[i]);
  }
  Serial.println();
  Serial.print("Value length = ");
  Serial.println(rxChar.valueLength());
}

void onBLEConnected(BLEDevice central) {
  Serial.print("Connected event, central: ");
  Serial.println(central.address());
}

void onBLEDisconnected(BLEDevice central) {
  Serial.print("Disconnected event, central: ");
  Serial.println(central.address());
}
