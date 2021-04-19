#include <DFRobot_TFmini_Nano.h>

void      TFmini::begin(HardwareSerial &s_)
{
    TFSerial = &s_;
    TFSerial->begin(115200);
}

bool      TFmini::measure(void)
{
    uint8_t       TFbuff[9] = {0};
    long          checksum  = 0 ;

    while(TFSerial->available()){
        TFbuff[0] = this->getNext();
        if(TFbuff[0] != 'Y') continue;
        checksum = TFbuff[0];

        TFbuff[1] = this->getNext();
        if(TFbuff[1] != 'Y') continue;
        checksum += TFbuff[1];
        
        for(int i = 2;i < 8;i++){
            TFbuff[i] = this->getNext();
            checksum += TFbuff[i];
        }
        TFbuff[8] = this->getNext();
        checksum &= 0xff;
        
        if(checksum == TFbuff[8]){
            distance = TFbuff[2]+TFbuff[3]*256;
            strength = TFbuff[4]+TFbuff[5]*256;
            return true;
        }else{
            checksum  = 0;
        }
    }
    return false;
}

uint8_t   TFmini::getNext(void){
    while(!TFSerial->available());
    return TFSerial->read();
}

uint16_t  TFmini::getDistance(void)
{
    return distance;
}

uint16_t TFmini::getStrength(void)
{
    return strength;
}
