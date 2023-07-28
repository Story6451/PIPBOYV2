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
    lastCLK = digitalRead(pPIPDATA->ENCODER_CLK);
    dataToBeSaved = "";
    timer = 0;
    verticalIndex = 0;
    oldIndex = -1;
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
        case -23214:
            colourIndex = 3;
            break;
    }
}

void OptionsTab::Loop()
{
    if (pPIPDATA->Locked == true)
    {
        verticalIndex = round((analogRead(pPIPDATA->POT_1_PIN)*3)/1023);

        currentCLK = digitalRead(pPIPDATA->ENCODER_CLK);
        if ((currentCLK != lastCLK) && (currentCLK == 1))
        {
            if (digitalRead(pPIPDATA->ENCODER_DT) != currentCLK)
            {
                horozontalIndex--;
            }
            else
            {
                horozontalIndex++;
            }
        }
        lastCLK = currentCLK;
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

    /*
    int val1 = analogRead(pPIPDATA->POT_1_PIN); // horozontal control
    int val2 = analogRead(pPIPDATA->POT_2_PIN); //vertical control
    int btnState3 = digitalRead(pPIPDATA->BTN_3_PIN);


    if (pPIPDATA->Locked)
    {
        index1 = round(val1/(1023/4));
        index2 = round(val2/(1023/totalAdjustableValues));

        if (btnState3 == HIGH)
        {
            selected = !selected;
            if (selected)
                select = index2;
            else
                select = 100;
        }

        if (index2 == oldIndex)
        {

            switch (index2)
            {
                case 0:
                    if (selected && select == 0)
                    {
                        switch (index1)
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
                            default:
                                pPIPDATA->ActiveColour = WHITE;
                                break;
                        }
                    }
                    break;
                case 1:
                    if (selected && select == 1)
                    {
                        pPIPDATA->AltitudeOffset = map(val1, 1, 1023, 1, 210);
                        if (pPIPDATA->AltitudeOffset > 200)
                            pPIPDATA->AltitudeOffset = 200;
                        else if (pPIPDATA->AltitudeOffset < 20)
                            pPIPDATA->AltitudeOffset = 0;
                    }
                    break;
                default:
                    if (selected && select == 2)
                    {
                        pPIPDATA->Volume = map(val1, 1, 1023, 1, 110);
                        if (pPIPDATA->Volume > 100)
                            pPIPDATA->Volume = 100;
                        else if (pPIPDATA->Volume < 20)
                            pPIPDATA->Volume = 0;
                    }
                    break;
            }
        } 
        else if (selected)
        {
            index2 = oldIndex;
        }
        else 
            pTFT->drawRect(10, oldIndex * 220/totalAdjustableValues + 93, 320, 30, BLACK);
 
        oldIndex = index2; 
        pTFT->drawRect(10, index2 * 220/totalAdjustableValues + 93, 320, 30, pPIPDATA->ActiveColour);
    }
    else
    {
        pTFT->drawRect(10, oldIndex * 220/totalAdjustableValues + 93, 320, 30, BLACK);
        selected = false;
        select = 100;
    }
    */
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
        SD.remove("config.txt");
        File file = SD.open("config.txt", FILE_WRITE);
        if (file)
        {
            file.close();
        }
    }
}

void OptionsTab::TFTOutput()
{
    if ((millis() - timer) > 200)
    {
        if (verticalIndex != oldIndex)
        {
            pTFT->drawRect(10, oldIndex * 220/totalAdjustableValues + 93, 320, 30, BLACK);
            oldIndex = verticalIndex;
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
            case -23214:
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