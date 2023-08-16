#pragma once

#include <Arduino.h>
#include <Tab.h>

class OptionsTab : public Tab
{
    private:
        const uint8_t totalAdjustableValues = 3;
        int8_t prevVerticalIndex;
        int8_t verticalIndex;
        int32_t horozontalIndex;
        int32_t prevHorozontalIndex;
        int32_t prevEncoderBValue;
        uint8_t currentCLK;
        uint8_t lastCLK;

        uint64_t timer;
        int8_t colourIndex;
        int16_t altitudeOffset;
        int8_t volume;
        String dataToBeSaved;
    public:
       	OptionsTab(Adafruit_TFTLCD *ptft, ConfigData *pPipData);
        String ModuleName(){return "OPTIONS";}
        void Setup();
        void Loop();
        void SaveSettings();
        void TFTOutput();
};