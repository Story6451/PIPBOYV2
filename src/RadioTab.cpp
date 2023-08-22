#include <RadioTab.h>
#include <Adafruit_TFTLCD.h>
#include <SD.h>
#include <Colours.h>
#include <TEA5767N.h>

TEA5767N radio = TEA5767N();


RadioTab::RadioTab(Adafruit_TFTLCD *ptft, ConfigData *pPipData):Tab(ptft, pPipData)
{
    prevEncoderAValue = 0;
    timer = 0;
    radio.setStandByOn();
    radio.setMonoReception();
    radio.setStereoNoiseCancellingOn();
    mode = 0;
}

void RadioTab::Setup()
{
    signalStrength = 0;
    frequency = 76;
    channelFrequency = 76;
    TFTOutput();
}

void RadioTab::Loop()
{
    
    if (pPIPDATA->Locked == true)
    {
        radio.setStandByOff();
        if ((pPIPDATA->encoderBValue > prevEncoderBValue) || (pPIPDATA->encoderBValue < prevEncoderBValue))
        {
            if (mode == 1)
            {
                mode = 0;
            }
            else
            {
                mode = 1;
            }
            TFTOutput();
        }

        if (mode == 0)
        {
            if (pPIPDATA->encoderAValue > prevEncoderAValue)
            {
                frequency += 0.2;
            }
            else if (pPIPDATA->encoderAValue < prevEncoderAValue)
            {
                frequency -= 0.2;
            }
            if (frequency > 108)
            {
                frequency = 108;
            }
            else if (frequency < 76)
            {
                frequency = 76;
            }
            channelFrequency = frequency;
        }
        else
        {
            //dont works
            bool flag = false;
            while ((signalStrength < 10) && (frequency < 108))
            {
                if (flag == false)
                {
                    pTFT->setCursor(10, 100); pTFT->print("Frequency: SEARCHING   ");
                    flag = true;
                }
            
                radio.selectFrequency(frequency);
                signalStrength = radio.getSignalLevel();
                frequency += 0.1;
                if (frequency > 108)
                {
                    frequency = 108;
                }
                if (signalStrength >= 10)
                {
                    channelFrequency = frequency;
                }
                //delay(100);
            }

        }

        prevEncoderAValue = pPIPDATA->encoderAValue;
        prevEncoderBValue = pPIPDATA->encoderBValue;
        signalStrength = radio.getSignalLevel();
        radio.selectFrequency(channelFrequency);
    }
    else
    {
        radio.setStandByOn();
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
    pTFT->print("Frequency: "); pTFT->print(channelFrequency); pTFT->print("MHz  ");
    pTFT->setCursor(10, 130);
    pTFT->print("Signal Strength: "); pTFT->print(signalStrength); pTFT->print("  ");
    pTFT->setCursor(10, 160);
    pTFT->print("Mode: "); 

    if (mode == 0)
    {
        pTFT->print("Manual");
    }
    else
    {
        pTFT->print("Automatic");
    }

    pTFT->print("     ");
}