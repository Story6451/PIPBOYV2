#pragma once

#include <Arduino.h>
#include <Tab.h>

class HealthTab : public Tab
{
  private:
    static const int BUFFER_LENGTH = 100;
    float avgRawVal;
    double stdDeviation;
    int32_t rawVal;
    int8_t valuesArrayPosition;
    int32_t values[BUFFER_LENGTH];
    int32_t times[BUFFER_LENGTH];
    bool firstbeat;
    int64_t time1;
    int64_t time2;
    int32_t deltaTime;
    int16_t bpm;
    int16_t meanBpm;
    int8_t bpmArrayPosition;
    int32_t bpms[BUFFER_LENGTH];

    
    int64_t t1;
    int64_t t2;
    int64_t t3;
    int16_t step;

    //statistics stuff
    //int32_t standardDeviation;

  public:
  	HealthTab(Adafruit_TFTLCD *ptft, ConfigData *pPipData);
    String ModuleName(){return "STATS";}
  	void Setup();
  	void Loop();
    int32_t StandardDeviation(int32_t dataset[], int32_t SIZE);
    void OutputThroughSerial();
    virtual void TFTOutput();
};