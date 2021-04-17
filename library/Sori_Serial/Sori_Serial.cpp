#include <Sori_Serial.h>


SoriSerial::SoriSerial(){
    dataService = BLEService(uuidOfService);
    rxChar = BLECharacteristic(uuidOfRxChar, BLEWriteWithoutResponse | BLEWrite, RX_BUFFER_SIZE, RX_BUFFER_FIXED_LENGTH);
    txChar = BLECharacteristic(uuidOfTxChar, BLERead | BLENotify, TX_BUFFER_SIZE, TX_BUFFER_FIXED_LENGTH);
}

void SoriSerial::begin(){
    if (!BLE.begin()){
        while (true);
    }

    BLE.setDeviceName(nameOfPeripheral);
    BLE.setLocalName(nameOfPeripheral);
    BLE.setAdvertisedService(dataService);
    dataService.addCharacteristic(rxChar);
    dataService.addCharacteristic(txChar);

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
}

void SoriSerial::poll(){
    BLE.poll();
}

bool SoriSerial::isMessageReceived(){
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

