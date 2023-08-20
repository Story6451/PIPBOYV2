#include <RadioTab.h>
#include <Adafruit_TFTLCD.h>
#include <SD.h>
#include <Colours.h>
#include <TEA5767N.h>

TEA5767N radio = TEA5767N();


RadioTab::RadioTab(Adafruit_TFTLCD *ptft, ConfigData *pPipData):Tab(ptft, pPipData)
{
    prevEncoderAValue = 0;
    frequency = 0;
    signalStrength = 0;
}

void RadioTab::Setup()
{
    radio.setMonoReception();
    radio.setStereoNoiseCancellingOn();
    
}

void RadioTab::Loop()
{
    if (pPIPDATA->Locked == true)
    {
        if (pPIPDATA->encoderAValue > prevEncoderAValue)
        {
            frequency += 0.1;
        }
        else if (pPIPDATA->encoderAValue < prevEncoderAValue)
        {
            frequency -= 0.1;
        }

        if (pPIPDATA->encoderBValue > prevEncoderBValue)
        {
            frequency += 1;
        }
        else if (pPIPDATA->encoderBValue < prevEncoderBValue)
        {
            frequency -= 1;
        }

        if (frequency > 109)
        {
            frequency = 109;
        }
        else if (frequency < 87)
        {
            frequency = 87;
        }

        prevEncoderAValue = pPIPDATA->encoderAValue;
        prevEncoderBValue = pPIPDATA->encoderBValue;
        signalStrength = radio.getSignalLevel();
        radio.selectFrequency(frequency);
    }

    TFTOutput();
}

void RadioTab::OutputThroughSerial(String output)
{
    Serial.println(output);
}

void RadioTab::TFTOutput()
{
    pTFT->setTextSize(pPIPDATA->TextSize);
    pTFT->setTextColor(pPIPDATA->ActiveColour, BLACK);

    pTFT->setCursor(10, 100);
    pTFT->print("Frequency: "); pTFT->print(frequency); pTFT->print("MHz  ");
    pTFT->setCursor(10, 130);
    pTFT->print("Signal Strength: "); pTFT->print(signalStrength); pTFT->print("  ");
}