#include <OptionsTab.h>
#include <Colours.h>
#include <Adafruit_TFTLCD.h>
#include <SD.h>

OptionsTab::OptionsTab(Adafruit_TFTLCD *ptft, ConfigData *pPipData):Tab(ptft, pPipData)
{
    horozontalIndex = 0;
    prevHorozontalIndex = 0;
}

void OptionsTab::Setup()
{
    pTFT->setTextSize(2);
    pTFT->setTextColor(pPIPDATA->ActiveColour, BLACK);  

    dataToBeSaved = "";
    timer = 0;
    verticalIndex = 0;
    prevVerticalIndex = -1;
    prevEncoderBValue = -1;
    altitudeOffset = pPIPDATA->AltitudeOffset;
    volume = pPIPDATA->Volume; 
    switch (pPIPDATA->ActiveColour)
    {
        case 64512:
            colourIndex = 0;
            break;
        case 7836:
            colourIndex = 1;
            break;
        case 9537:
            colourIndex = 2;
            break;
        case 42322:
            colourIndex = 3;
            break;
    }
}

void OptionsTab::Loop()
{
    if (pPIPDATA->Locked == true)
    {
        //verticalIndex = round((analogRead(pPIPDATA->POT_1_PIN)*3)/1023);
        verticalIndex = pPIPDATA->encoderBValue;
        if (pPIPDATA->encoderBValue > prevEncoderBValue)
        {
            verticalIndex++;
        }
        else if (pPIPDATA->encoderBValue < prevEncoderBValue)
        {
            verticalIndex--;
        }

        if (verticalIndex > 3)
        {
            verticalIndex = 3;
        }
        else if (verticalIndex < 0)
        {
            verticalIndex = 0;
        }

        horozontalIndex = pPIPDATA->encoderAValue;

        Serial.print("  Vertical Index: "); Serial.print(verticalIndex);
        Serial.print("  Horozontal Index: "); Serial.print(horozontalIndex);
        Serial.print("  Encoder B value: "); Serial.print(pPIPDATA->encoderBValue);
        Serial.print("  Previous encoder B value: "); Serial.println(prevEncoderBValue);

        switch (verticalIndex)
        {
            case 0:
                if (horozontalIndex > prevHorozontalIndex)
                {
                    colourIndex++;
                }
                else if (horozontalIndex < prevHorozontalIndex)
                {
                    colourIndex--;
                }

                if (colourIndex > 3)
                {
                    colourIndex = 3;
                }
                else if (colourIndex < 0)
                {
                    colourIndex = 0;
                }
                switch (colourIndex)
                {
                    case 0:
                        pPIPDATA->ActiveColour = AMBER;
                        break;
                    case 1:
                        pPIPDATA->ActiveColour = BLUE;
                        break;
                    case 2:
                        pPIPDATA->ActiveColour = GREEN;
                        break;
                    case 3:
                        pPIPDATA->ActiveColour = WHITE;
                        break;
                }
                break;
            case 1:
                if (horozontalIndex > prevHorozontalIndex)
                {
                    altitudeOffset += 10;
                }
                else if (horozontalIndex < prevHorozontalIndex)
                {
                    altitudeOffset -= 10;
                }
                if (altitudeOffset > 200)
                {
                    altitudeOffset = 200;
                }
                else if (altitudeOffset == 0)
                {
                    altitudeOffset = 0;
                }
                pPIPDATA->AltitudeOffset = altitudeOffset;
                break;
            case 2:
                if (horozontalIndex > prevHorozontalIndex)
                {
                    volume += 10;
                }
                else if (horozontalIndex < prevHorozontalIndex)
                {
                    volume -= 10;
                }
                if (volume > 100)
                {
                    volume = 100;
                }
                else if (volume < 0)
                {
                    volume = 0;
                }
                pPIPDATA->Volume = volume;
        }
        prevHorozontalIndex = horozontalIndex;
    }
    SaveSettings();
    TFTOutput();
}

void OptionsTab::SaveSettings()
{
    dataToBeSaved = pPIPDATA->ActiveColour;
    dataToBeSaved += ",";
    dataToBeSaved += pPIPDATA->AltitudeOffset;
    dataToBeSaved += ",";
    dataToBeSaved += pPIPDATA->Volume;
    dataToBeSaved += ",";

    if (SD.exists("config.txt"))
    {
        //Serial.println("File exists");
        SD.remove("config.txt");
        File file = SD.open("config.txt", FILE_WRITE);
        if (file)
        {
            file.print(dataToBeSaved);
            file.close();
        }
    }
}

void OptionsTab::TFTOutput()
{
    if ((millis() - timer) > 200)
    {
        if (verticalIndex != prevVerticalIndex)
        {
            pTFT->drawRect(10, prevVerticalIndex * 220/totalAdjustableValues + 93, 320, 30, BLACK);
            prevVerticalIndex = verticalIndex;
        }
        pTFT->drawRect(10, verticalIndex * 220/totalAdjustableValues + 93, 320, 30, pPIPDATA->ActiveColour);

        String currentColour;

        switch (pPIPDATA->ActiveColour)
        {
            case 64512:
                currentColour = "AMBER";
                break;
            case 7836:
                currentColour = "BLUE";
                break;
            case 9537:
                currentColour = "GREEN";
                break;
            case 42322:
                currentColour = "WHITE";
                break;
            default:
                currentColour = pPIPDATA->ActiveColour;
                break;
        }
        pTFT->setCursor(20, 100);
        pTFT->print("Colour Mode: "); pTFT->print(currentColour);

        pTFT->setCursor(20, 173);
        pTFT->print("Altitude Offset (m): "); pTFT->print(pPIPDATA->AltitudeOffset); pTFT->println("  ");

        pTFT->setCursor(20, 246);
        pTFT->print("Volume: "); pTFT->print(pPIPDATA->Volume); pTFT->println("  ");

        timer = millis();
    }
}