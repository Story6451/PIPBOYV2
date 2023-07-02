#pragma once

#include <Arduino.h>
#include <Tab.h>

class OptionsTab : public Tab
{
    private:
        byte index1;
        byte index2;
        byte oldIndex;
        byte select;
        bool selected; 
        String dataToBeSaved;
    public:
       	OptionsTab(Adafruit_TFTLCD *ptft, ConfigData *pPipData);
        String ModuleName(){return "OPTIONS";}
        void Setup();
        void Loop();
        void TFTOutput();
};