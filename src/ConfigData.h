#pragma once

#include <Arduino.h>
#include <Colours.h>

class ConfigData
{
    public:
        //options
        uint32_t ActiveColour;
        uint8_t AltitudeOffset;
        uint8_t Volume;
        uint8_t TextSize;
        uint8_t Orientation;
        uint8_t LightStatus;
        //cotnrols
        int32_t encoderAValue = 0;
        int32_t encoderBValue = 0;
        const uint32_t ENCODER_A_CLK = 5;
        const uint32_t ENCODER_A_DT = 3;
        const uint32_t ENCODER_B_CLK = 4;
        const uint32_t ENCODER_B_DT = 2;
        //const uint32_t POT_1_PIN = 5; 
        //const uint32_t POT_2_PIN = 7;
        //const uint32_t BTN_3_PIN = 6;
        bool Locked;

        ConfigData() :ActiveColour(GREEN), AltitudeOffset(50), Volume(50), TextSize(2), Orientation(false), LightStatus(true), Locked(false){}
};