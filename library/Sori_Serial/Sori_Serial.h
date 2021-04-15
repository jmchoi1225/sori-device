#ifndef __SORI_SERIAL_H__
#define __SORI_SERIAL_H__

#include <ArduinoBLE.h>

class SoriSerial
{
public:
    void begin();
    String read();
    void write(String message);
private:
    const char* nameOfPeripheral = "SORI";
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
    
    String receivedMessage;

    void onConnect();
    void onDisconnect();
    void onReceiveMessage();
};

#endif
