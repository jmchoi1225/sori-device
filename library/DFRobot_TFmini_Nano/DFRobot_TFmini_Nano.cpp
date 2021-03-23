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
        TFbuff[0] = TFSerial->read();
        checksum += TFbuff[0];
        if(TFbuff[0] == 'Y'){
            TFbuff[1] = TFSerial->read();
            checksum += TFbuff[1];
            if(TFbuff[1] == 'Y'){
                for(int i = 2;i < 8;i++){
                    TFbuff[i] = TFSerial->read();
                    checksum += TFbuff[i];
                }
                TFbuff[8] = TFSerial->read();
                checksum &= 0xff;
                if(checksum == TFbuff[8]){
                    distance = TFbuff[2]+TFbuff[3]*256;
                    strength = TFbuff[4]+TFbuff[5]*256;
                    return true;
                }else{
                    checksum  = 0;
                }
            }else{
                checksum  = 0;
            }
        }else{
            checksum  = 0;
        }
    }
    return false;
}

uint16_t  TFmini::getDistance(void)
{
    return distance;
}

uint16_t TFmini::getStrength(void)
{
    return strength;
}
