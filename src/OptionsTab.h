#pragma once

#include <Arduino.h>
#include <Tab.h>

class OptionsTab : public Tab
{
    private:
        const uint8_t numberOfOptions = 5;
        int32_t prevEncoderBValue;
        uint8_t currentCLK;
        uint8_t lastCLK;

        int8_t prevVerticalIndex;
        int8_t verticalIndex;
        int8_t cursorIndex;
        int8_t prevCursorIndex;
        int32_t horozontalIndex;
        int32_t prevHorozontalIndex;
        

        //options
        String options[6] = {"Colour", "Altitude Offset", "Volume", "Text Size", "Flip Screen", "LED's"};
        String optionsData[6];
        int8_t firstDisplayed;
        int8_t prevFirstDisplayed;
        int8_t colourIndex;
        int16_t altitudeOffset;
        int8_t volume;
        int8_t textSize;
        int8_t orientation;
        int8_t lightsOn;

        String dataToBeSaved;
        uint64_t timer;
    public:
       	OptionsTab(Adafruit_TFTLCD *ptft, ConfigData *pPipData);
        String ModuleName(){return "OPTIONS";}
        void Setup();
        void Loop();
        void SaveSettings();
        void TFTOutput();
};