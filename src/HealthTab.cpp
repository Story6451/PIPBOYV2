#include <HealthTab.h>
#include <Adafruit_TFTLCD.h>
#include <SD.h>
#include <Colours.h>
#include <Graphics.h>
#include <PeakDetector.h>


#define heartPin A15

//DFRobot_Heartrate heartSensor(ANALOG_MODE);
PeakDetector pd;

HealthTab::HealthTab(Adafruit_TFTLCD *ptft, ConfigData *pPipData):Tab(ptft, pPipData)
{   
    time1 = 0;
    
}

void HealthTab::Setup()
{
    //resets variables
    step = 0;
    avgRawVal = 0;
    stdDeviation = 0;
    time1 = 0;
    time2 = 0;
    firstbeat = true;
    rawVal = 0;
    deltaTime = 0;
    bpm = 0;

    meanBpm = 0;
    valuesArrayPosition = 0;
    bpmArrayPosition = 0;

    t1 = 0;
    t2 = 0;
    t3 = 0;

    for (int i = 0; i < BUFFER_LENGTH -1; i++)
    {
        bpms[i] = 0;
    }
    
    //fills buffer
    /*
    for (int i = 0; i < BUFFER_LENGTH - 1; i++)
    {
        values[i] = analogRead(heartPin);
        delay(5);
    }
    */
    pTFT->drawBitmap(270, 100, StatBoy, 200, 200, pPIPDATA->ActiveColour);
}

bool arrayIsFull = false;

void HealthTab::Loop()
{
    rawVal = analogRead(heartPin);

    if (rawVal > 150)
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
            stdDeviation = sqrt(sumRawValSquared/BUFFER_LENGTH - (avgRawVal * avgRawVal));

        }

        if ((rawVal - avgRawVal) > 0)   //then we have a peak
        {
            if (firstbeat == true)
            {
                time1 = millis();
            }
            else
            {
                time2 = millis();
            }
            firstbeat = !firstbeat;

            deltaTime = abs(time1 - time2);

            bpm = 60000/deltaTime;
            bpms[bpmArrayPosition] = bpm;
            
            if (bpmArrayPosition == BUFFER_LENGTH - 1)
            {
                bpmArrayPosition = 0;
            }
            else
            {
                bpmArrayPosition++;
            }
            
            int32_t sumOfBpms = 0;
            for(int i = 0; i < BUFFER_LENGTH; i++)
            {
                sumOfBpms += bpms[i];
            }
            meanBpm = sumOfBpms / BUFFER_LENGTH;
        }
    }

    /*
    if (rawVal > 150)
    {
        values[arrayPosition] = rawVal; //add data to array
        times[arrayPosition] = millis();

        if(arrayPosition == BUFFER_LENGTH - 1) 
        {
            arrayPosition = 0;
        }       
        else
        {
            arrayPosition++;
        }

        bool peak = pd.addValue(values[arrayPosition]);
        if (peak)
        {
            Serial.println("peak");
            time2 = millis();
            delta = time2 - time1;
            time1 = time2;
            peak = rawVal;
            beat = true;
        }

        if ((delta < 2500) && (delta > 250))
        {
            deltas[arrayPosition] = delta;
        }

        int32_t deltaSum = 0;
        for (int i = 0; i < BUFFER_LENGTH -1; i++)
        {
            deltaSum += deltas[i];
        }
        meanDelta = deltaSum/BUFFER_LENGTH;
        bpm = 60000/meanDelta;

        bpms[arrayPosition] = bpm;

        int32_t bpmsum = 0;
        for (int i = 0; i < BUFFER_LENGTH -1; i++)
        {
            bpmsum += bpms[i];
        }
        meanBpm = bpmsum/BUFFER_LENGTH;

    }
    else
    {
        rawVal = 10;
        meanBpm = 0;
        bpm = 0;
    }
    */
    OutputThroughSerial();
    TFTOutput();

}

int32_t HealthTab::StandardDeviation(int32_t dataSet[], int32_t SIZE){}

void HealthTab::OutputThroughSerial()
{
    Serial.print(" Analog value: "); Serial.print(rawVal);
    Serial.print(" Mean value: "); Serial.print(avgRawVal);
    Serial.print(" Delta raw values: "); Serial.print(rawVal - avgRawVal);
    Serial.print(" std dev value: "); Serial.print(stdDeviation);
    Serial.print(" delta: "); Serial.print(deltaTime);
    Serial.print(" Mean BPM: "); Serial.print(meanBpm);
    Serial.print(" BPM: "); Serial.println(bpm);
}

void HealthTab::TFTOutput()
{
    t2 = millis();
    if ((t2 - t1) > 1000) //check to see if a second has passed
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
    }

    pTFT->drawFastVLine(step, (320 - map(rawVal, 0, 1023, 0, 140)), map(rawVal, 0, 1023, 0, 140), pPIPDATA->ActiveColour);
    step++;

    if (step >= 240)
    {
        step = 0;
        pTFT->fillRect(0, 180, 240, 140, BLACK);
    }
    if ((t2 - t3) > 50)
    {
        t3 = t2;
    }
}