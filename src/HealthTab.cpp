#include <HealthTab.h>
#include <Adafruit_TFTLCD.h>
#include <SD.h>
#include <Colours.h>
#include <Graphics.h>

#define heartPin A15

HealthTab::HealthTab(Adafruit_TFTLCD *ptft, ConfigData *pPipData):Tab(ptft, pPipData){}

void HealthTab::Setup()
{
    //resets variables
    step = 0;
    avgRawVal = 0;
    stdDeviation = 0;
    time1 = 0;
    time2 = 0;
    rising = false;
    rawVal = 0;
    deltaTime = 0;
    bpm = 0;
    previousBpm = -1;

    meanBpm = 0;
    valuesArrayPosition = 0;
    bpmArrayPosition = 0;

    t1 = 0;
    t2 = 0;

    for (int i = 0; i < BUFFER_LENGTH -1; i++)
    {
        bpms[i] = 0;
    }
    
    pTFT->drawBitmap(270, 100, StatBoy, 200, 200, pPIPDATA->ActiveColour);
}

bool arrayIsFull = false;

void HealthTab::Loop()
{
    rawVal = analogRead(heartPin);

    if (rawVal > 150) //if fingure is on sensor
    {
        values[valuesArrayPosition] = rawVal;
        times[valuesArrayPosition] = millis();

        if (valuesArrayPosition == BUFFER_LENGTH - 1)
        {
            arrayIsFull = true;
            valuesArrayPosition = 0;
        }
        else
        {
            valuesArrayPosition++;
        }

        if (arrayIsFull == true)
        {
            int64_t sumRawVal = 0;
            int64_t sumRawValSquared = 0;
            for(byte i = 0; i < BUFFER_LENGTH - 1; i++)
            {
                sumRawVal += values[i];
                sumRawValSquared += values[i] * values[i];
            }
            
            avgRawVal = sumRawVal / BUFFER_LENGTH;
        }

        if ((rawVal - avgRawVal) > 0)   //we have a peak
        {
            if (rising == false)
            {
                int32_t temp = time1;
                time1 = millis();
                time2 = temp;

            }
            rising = true;

            deltaTime = abs(time1 - time2);

            bpm = 60000/deltaTime;

            if ((bpm < 300) && (bpm != previousBpm))
            {

                bpms[bpmArrayPosition] = bpm;
                
                if (bpmArrayPosition == BUFFER_LENGTH - 1)
                {
                    bpmArrayPosition = 0;
                }
                else
                {
                    bpmArrayPosition++;
                }
                
                previousBpm = bpm;
                meanBpm = Mean(bpms, BUFFER_LENGTH);
            }
        }
        else
        {
            rising = false;
        }
    }

    //OutputThroughSerial();
    TFTOutput();
}

float HealthTab::StandardDeviation(int32_t dataset[], int32_t SIZE)
{
    int64_t sum = 0;
    int64_t sumSquared = 0;
    for(int i = 0; i < SIZE; i++)
    {
        sum += dataset[i];
        sumSquared += dataset[i] * dataset[i];
    }

    float mean = sum / SIZE;

    return sqrtf((sumSquared/SIZE) - (mean * mean));
}

float HealthTab::Mean(int32_t dataset[], int32_t SIZE)
{
    int64_t sum = 0;
    int16_t emptyItems = 0;
    for (int i = 0; i < SIZE; i++)
    {
        if (dataset[i] == 0)
        {
            emptyItems++;
        }
        sum += dataset[i];
    }

    return sum / (SIZE - emptyItems);
}

void HealthTab::OutputThroughSerial()
{
    Serial.print(" Analog value: "); Serial.print(rawVal);
    Serial.print(" Mean value: "); Serial.print(avgRawVal);
    Serial.print(" Delta raw values: "); Serial.print(abs(rawVal - avgRawVal));
    Serial.print(" delta: "); Serial.print(deltaTime);
    Serial.print(" Mean BPM: "); Serial.print(meanBpm);
    Serial.print(" BPM: "); Serial.println(bpm);
}

void HealthTab::TFTOutput()
{
    t2 = millis();
    if ((t2 - t1) > 10) //check to see if a second has passed
    {
        pTFT->setTextSize(2);
        pTFT->setTextColor(pPIPDATA->ActiveColour, BLACK);

        //displays bpm
        pTFT->setCursor(20, 100);
        pTFT->print("AVG BPM: "); 
        if (meanBpm < 300)
        {
            pTFT->print(meanBpm); pTFT->print("  "); 
        }
        else
        {
            pTFT->print("error"); 
        }
        pTFT->print("  ");
        pTFT->setCursor(20, 140);
        pTFT->print("BPM:"); 
        if (bpm < 300)
        {
            pTFT->print(bpm);  pTFT->print("  "); 
        }
        else
        {
            pTFT->print("error"); 
        }
        pTFT->print(" ");
        t1 = t2;

        pTFT->drawFastVLine(step, (320 - map(rawVal, 0, 1023, 0, 140)), map(rawVal, 0, 1023, 0, 140), pPIPDATA->ActiveColour);
        step++;
    }


    if (step >= 240)
    {
        step = 0;
        pTFT->fillRect(0, 180, 240, 140, BLACK);
    }
}