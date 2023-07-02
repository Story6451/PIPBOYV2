#pragma once

#include <ConfigData.h>

//#include <Adafruit_TFTLCD.h>
class Adafruit_TFTLCD;

class Tab
{
    public:
        static int Total;

    protected:
        Adafruit_TFTLCD *pTFT;

        ConfigData *pPIPDATA;
    public:
        Tab(Adafruit_TFTLCD *ptft, ConfigData *pPipData){pTFT = ptft; pPIPDATA = pPipData; Total++;}
        virtual String ModuleName() = 0;
        virtual void TFTOutput() = 0;
        virtual void Setup() = 0;
        virtual void Loop() = 0;
};