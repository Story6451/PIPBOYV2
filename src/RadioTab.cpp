#include <RadioTab.h>
#include <Adafruit_TFTLCD.h>
#include <SD.h>
#include <Colours.h>
#include <radio.h>
#include <TEA5767.h>

//#define FIX_BAND RADIO_BAND_FM

TEA5767 radio;

RadioTab::RadioTab(Adafruit_TFTLCD *ptft, ConfigData *pPipData):Tab(ptft, pPipData)
{
    frequency = 0;

    if (radio.init())
    {
        pTFT->println("Found radio");
        Serial.println("Found radio");
        radioFound = true;
    }
    else
    {
        pTFT->println("Could not find radio");
        Serial.println("Could not find radio");
        radioFound = false;
    }
}

void RadioTab::Setup()
{
    if (radioFound)
        radio.setMono(false);
    
}

void RadioTab::Loop()
{
    
    int val = analogRead(pPIPDATA->POT_2_PIN);
    frequency = map(val, 0, 1023, 8700, 10800);
    byte vol = map(pPIPDATA->Volume, 0, 100, 0, 15);

    radio.setBandFrequency(RADIO_BAND_FM, frequency);

    radio.setVolume(vol);


    OutputThroughSerial(ModuleName());
    TFTOutput();
}

void RadioTab::OutputThroughSerial(String output)
{
    Serial.println(output);
}

void RadioTab::TFTOutput()
{
    pTFT->setTextSize(2);
    pTFT->setTextColor(pPIPDATA->ActiveColour, BLACK);
    pTFT->setCursor(0, 100);
    //pTFT->print("Frequency: "); pTFT->print(frequency/100); pTFT->print(" Hz ");
    pTFT->print(ModuleName());
}