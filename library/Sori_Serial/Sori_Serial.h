#ifndef __SORI_SERIAL_H__
#define __SORI_SERIAL_H__

#include <ArduinoBLE.h>

class SoriSerial
{
public:
    SoriSerial();
    void begin();
    void poll();
    String read();
    void write(String message);
    bool isMessageReceived();
private:
    const char* nameOfPeripheral = "SORI";
    const char* uuidOfService = "0000181a-0000-1000-8000-00805f9b34fb";
    const char* uuidOfRxChar = "00002A3D-0000-1000-8000-00805f9b34fb";
    const char* uuidOfTxChar = "00002A58-0000-1000-8000-00805f9b34fb";

    const int RX_BUFFER_SIZE = 256;
    const bool RX_BUFFER_FIXED_LENGTH = false;
    const int TX_BUFFER_SIZE = 256;
    const bool TX_BUFFER_FIXED_LENGTH = false;
    
    BLEService dataService;
    BLECharacteristic rxChar;
    BLECharacteristic txChar;
    
    const String EMPTY_MESSAGE = "";
    String receivedMessage = "";
    bool messageReceived = false;
    bool connected = false;

    void onConnect();
    void onDisconnect();
    void onReceiveMessage();
};

#endif
