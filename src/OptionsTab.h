#pragma once

#include <Arduino.h>
#include <Tab.h>

class OptionsTab : public Tab
{
    private:
        const uint8_t totalAdjustableValues = 3;
        uint8_t oldIndex;
        uint8_t verticalIndex;
        uint8_t currentCLK;
        uint8_t lastCLK;
        int32_t horozontalIndex;
        int32_t prevHorozontalIndex;
        uint64_t timer;
        uint8_t colourIndex;
        uint8_t altitudeOffset;
        uint8_t volume;
        String dataToBeSaved;
    public:
       	OptionsTab(Adafruit_TFTLCD *ptft, ConfigData *pPipData);
        String ModuleName(){return "OPTIONS";}
        void Setup();
        void Loop();
        void SaveSettings();
        void TFTOutput();
};