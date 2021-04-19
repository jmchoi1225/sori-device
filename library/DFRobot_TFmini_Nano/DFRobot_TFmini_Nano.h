#ifndef __DFRobot_TFmini_NANO_H__
#define __DFRobot_TFmini_NANO_H__

#include <Arduino.h>
#include <stdint.h>
#include <Wire.h>
#include "stdio.h"

class TFmini
{
public:
    void                begin(HardwareSerial &s_);
    bool                measure(void);
    uint16_t            getDistance(void);
    uint16_t            getStrength(void);
private:
    HardwareSerial*     TFSerial;
    uint16_t            distance = 0;
    uint16_t            strength = 0;
    uint8_t             getNext(void);
};

#endif
