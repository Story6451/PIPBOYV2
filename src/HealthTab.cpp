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
    step = 0;
    rawVal = 0;
    delta = 0;
    bpm = 0;
    preBpm = 0;
    bpmRepeats = 0;
    meanBpm = 0;
    arrayPosition = 0;
    peak = 0;
    beat = false;
    t1 = 0;
    t3 = 0;

    for (int i = 0; i < BUFFER_LENGTH -1; i++)
    {
        bpms[i] = 0;
        deltas[i] = 0;
    }
    
    //need to determine the peak so...
    
    for (int i = 0; i < BUFFER_LENGTH - 1; i++)
    {
        values[i] = analogRead(heartPin);
        delay(5);
    }
    
    pTFT->drawBitmap(270, 100, StatBoy, 200, 200, pPIPDATA->ActiveColour);
}

void HealthTab::Loop()
{
    rawVal = analogRead(heartPin);
    
    if (rawVal > 150)
    {
        values[arrayPosition] = rawVal; //add data to array
        times[arrayPosition] = millis();

        if(arrayPosition == BUFFER_LENGTH - 1)        
            arrayPosition = 0;
        else
            arrayPosition++;

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
            deltas[arrayPosition] = delta;

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
    OutputThroughSerial();
    TFTOutput();

}

int32_t HealthTab::StandardDeviation(int32_t dataSet[], int32_t SIZE){}
/*{
    int32_t dataSetSquared[SIZE];
    int32_t m = 0;
    for (int i = 0; i < SIZE -1; i++)
    {
        dataSetSquared[i] = dataSet[i] * dataSet[i];
    }
    
    //calculate mean
    int32_t sum = 0;
    for (int i = 0; i < SIZE - 1; i++)
    {
        sum = sum + dataSet[i];
    }
    m = sum/SIZE;

    //calculate standard deviation
    //sum of values squared
    int32_t sumSquared = 0;
    for (int i = 0; i < SIZE - 1; i++)
    {
        sumSquared = sumSquared + dataSetSquared[i];
    }
    //apply formula
    double temp1 = sumSquared/SIZE;
    double temp2 = m * m;
    double variance = temp1 - temp2;

    return sqrt(variance);
}*/

void HealthTab::OutputThroughSerial()
{
    Serial.print(" Analog value: "); Serial.print(rawVal);
    Serial.print(" delta: "); Serial.print(delta);
    Serial.print(" Mean BPM: "); Serial.println(meanBpm);
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
        pTFT->print("AVG BPM: "); pTFT->print(meanBpm); pTFT->print("  ");
        pTFT->setCursor(20, 140);
        pTFT->print("Calulated BPM:"); pTFT->print(bpm); pTFT->print(" ");
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

/*
    pTFT->drawFastVLine(step, (320 - map(rawVal, 0, 1023, 0, 140)), map(rawVal, 0, 1023, 0, 140), pPIPDATA->ActiveColour);
    step++;

    if (step >= 240)
    {
        step = 0;
        pTFT->fillRect(0, 180, 240, 140, BLACK);
    }
    */
}