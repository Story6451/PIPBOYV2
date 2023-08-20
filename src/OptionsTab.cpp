#include <OptionsTab.h>
#include <Colours.h>
#include <Adafruit_TFTLCD.h>
#include <SD.h>

OptionsTab::OptionsTab(Adafruit_TFTLCD *ptft, ConfigData *pPipData):Tab(ptft, pPipData)
{
    verticalIndex = 0;
    prevVerticalIndex = 0;
    cursorIndex = 0;
    prevCursorIndex = 0;
    horozontalIndex = 0;
    prevHorozontalIndex = 0;
    firstDisplayed = 0;
    prevFirstDisplayed = 0;
}

void OptionsTab::Setup()
{
    dataToBeSaved = "";
    timer = 0;

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
    textSize = pPIPDATA->TextSize;
    flipScreen = pPIPDATA->FlipScreen;
}

void OptionsTab::Loop()
{
    if (pPIPDATA->Locked == true)
    {
        //verticalIndex = round((analogRead(pPIPDATA->POT_1_PIN)*3)/1023);
        //verticalIndex = pPIPDATA->encoderBValue;
        if (pPIPDATA->encoderBValue > prevEncoderBValue)
        {
            verticalIndex++;
            cursorIndex++;
        }
        else if (pPIPDATA->encoderBValue < prevEncoderBValue)
        {
            verticalIndex--;
            cursorIndex--;
        }

        if (cursorIndex > 2)
        {
            firstDisplayed++;
            cursorIndex = 2;
        }
        else if (cursorIndex < 0)
        {
            firstDisplayed--;
            cursorIndex = 0;
        }

        if (verticalIndex > 3)
        {
            firstDisplayed = 2;
        }
        else if (verticalIndex <= 0)
        {
            firstDisplayed = 0;
            verticalIndex = 0;
        }
        
        if (verticalIndex > 4)
        {
            verticalIndex = 4;
        }

        horozontalIndex = pPIPDATA->encoderAValue;

        Serial.print("  Vertical Index: "); Serial.print(verticalIndex);
        Serial.print("  Cursor Index: "); Serial.print(cursorIndex);
        Serial.print("  first displayed Index: "); Serial.println(firstDisplayed);

        Serial.print("  Horozontal Index: "); Serial.print(horozontalIndex);

        //checks which item is selected
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
                else if (altitudeOffset <= 0)
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
                break;
            case 3:
                if (horozontalIndex > prevHorozontalIndex)
                {
                    textSize++;
                }
                else if (horozontalIndex < prevHorozontalIndex)
                {
                    textSize--;
                }

                if (textSize > 3)
                {
                    textSize = 3;
                }
                else if (textSize < 0)
                {
                    textSize = 0;
                }
                pPIPDATA->TextSize = textSize;
                break;
            case 4:
                if ((horozontalIndex > prevHorozontalIndex) || (horozontalIndex < prevHorozontalIndex))
                {
                    if (flipScreen == 1)
                    {
                        flipScreen = 0;
                    }
                    else
                    {
                        flipScreen = 1;
                    }
                
                    if (flipScreen == 1)
                    {
                        pTFT->setRotation(3);
                    }
                    else
                    {
                        pTFT->setRotation(1);
                    }
                }
                pPIPDATA->FlipScreen = flipScreen;
                break;
        }
        prevHorozontalIndex = horozontalIndex;
        prevEncoderBValue = pPIPDATA->encoderBValue;
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
    dataToBeSaved += pPIPDATA->TextSize;
    dataToBeSaved += ",";
    dataToBeSaved += pPIPDATA->FlipScreen;
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
    pTFT->setTextColor(pPIPDATA->ActiveColour);
    pTFT->setTextSize(pPIPDATA->TextSize);
    //if ((millis() - timer) > 100)
    {
        if (cursorIndex != prevCursorIndex)
        {
            pTFT->drawRect(10, prevCursorIndex * 220/3 + 93, 460, 33, BLACK);
            prevCursorIndex = cursorIndex;
        }
        pTFT->drawRect(10, cursorIndex * 220/3 + 93, 460, 33, pPIPDATA->ActiveColour);

        if (firstDisplayed != prevFirstDisplayed)
        {
            pTFT->fillRect(10, 61, 470, 259, BLACK);
            prevFirstDisplayed = firstDisplayed;
        }

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

        String flipped;
        if (flipScreen == 1)
        {
            flipped = "TRUE";
        }
        else
        {
            flipped = "FALSE";
        }

        optionsData[0] = currentColour;
        optionsData[1] = pPIPDATA->AltitudeOffset;
        optionsData[2] = pPIPDATA->Volume;
        optionsData[3] = pPIPDATA->TextSize;
        optionsData[4] = flipped;

        pTFT->setCursor(20, 100);
        pTFT->print(options[firstDisplayed]); pTFT->print(": "); pTFT->print(optionsData[firstDisplayed]); pTFT->print("  ");

        pTFT->setCursor(20, 173);
        pTFT->print(options[firstDisplayed + 1]); pTFT->print("(m): "); pTFT->print(optionsData[firstDisplayed + 1]); pTFT->print("  ");

        pTFT->setCursor(20, 246);
        pTFT->print(options[firstDisplayed + 2]); pTFT->print(": "); pTFT->print(optionsData[firstDisplayed + 2]); pTFT->print("  ");

        timer = millis();
    }
}