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
        uint8_t horozontalIndex;
        uint8_t prevHorozontalIndex;
        uint64_t timer;
        uint8_t colourIndex;
        uint8_t altitudeOffset;
        uint8_t volume;
        //uint8_t index1;
        //uint8_t index2;
        //uint8_t select;
        //bool selected; 
        String dataToBeSaved;
    public:
       	OptionsTab(Adafruit_TFTLCD *ptft, ConfigData *pPipData);
        String ModuleName(){return "OPTIONS";}
        void Setup();
        void Loop();
        void SaveSettings();
        void TFTOutput();
};