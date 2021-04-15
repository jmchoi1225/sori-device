#include <Sori_Serial.h>

void SoriSerial::begin(){
    if (!BLE.begin()){
        while (1);
    }

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
    
    receivedMessage = EMPTY_MESSAGE;
    messageReceived = false;
    connected = false;
    
    BLE.advertise();
}

void SoriSerial::poll(){
    BLE.poll();
}

bool SoriSerial::messageReceived(){
    return messageReceived;
}

String SoriSerial::read(){
    if(!messageReceived){
        return EMPTY_MESSAGE;
    }
    messageReceived = false;
    return receivedMessage;
}

void SoriSerial::write(String message){
    uint8_t data[256];
    message.getBytes(data, message.length());
    txChar.writeValue(data, message.length());
}

void SoriSerial::onConnect(){
    connected = true;
}

void SoriSerial::onDisconnect(){
    connected = false;
}

void SoriSerial::onReceiveMessage(BLEDevice central, BLECharacteristic characteristic){
    String data = reinterpret_cast<const char*>(rxChar.value());
    receivedMessage = String data.substring(0,rxChar.valueLength());
    messageReceived = true;
}

