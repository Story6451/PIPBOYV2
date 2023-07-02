#pragma once

#include <Arduino.h>
#include <Colours.h>

class ConfigData
{
    public:
        int ActiveColour;
        byte AltitudeOffset;
        byte Volume;
        byte Height;
        int Mass;
        //const byte TotalAdjustableValues = 3;

        const int BTN_3_PIN = 6;
        bool Locked;
        const int POT_1_PIN = 5; 
        const int POT_2_PIN = 7;

        ConfigData() :ActiveColour(GREEN), AltitudeOffset(50), Volume(50), Locked(false){}
};