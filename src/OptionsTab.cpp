#include <OptionsTab.h>
#include <Colours.h>
#include <Adafruit_TFTLCD.h>
#include <SD.h>


const byte totalAdjustableValues = 3;

OptionsTab::OptionsTab(Adafruit_TFTLCD *ptft, ConfigData *pPipData):Tab(ptft, pPipData)
{
    select = 0;
    selected = false;
    dataToBeSaved = "";
}

void OptionsTab::Setup()
{
}

void OptionsTab::Loop()
{
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

    dataToBeSaved = pPIPDATA->ActiveColour;
    dataToBeSaved += ",";
    dataToBeSaved += pPIPDATA->AltitudeOffset;
    dataToBeSaved += ",";
    dataToBeSaved += pPIPDATA->Volume;
    dataToBeSaved += ",";
    Serial.println(dataToBeSaved);

    if (SD.exists("config.txt"))
    {
        SD.remove("config.txt");
        Serial.println("file exists");
        File file = SD.open("config.txt", FILE_WRITE);
        if (file)
        {
            Serial.println("writing");
            file.print(dataToBeSaved);
            file.close();
        }
    }
    TFTOutput();
}


void OptionsTab::TFTOutput()
{
    pTFT->setTextSize(2);
    pTFT->setTextColor(pPIPDATA->ActiveColour, BLACK);

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
}