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
    bool rising;
    int64_t time1;
    int64_t time2;
    int32_t deltaTime;
    float bpm;
    float previousBpm;
    float meanBpm;
    int8_t bpmArrayPosition;
    int32_t bpms[BUFFER_LENGTH];
    bool arrayIsFull;
    
    int64_t t1;
    int64_t t2;
    int16_t step;
  public:
  	HealthTab(Adafruit_TFTLCD *ptft, ConfigData *pPipData);
    String ModuleName(){return "STATS";}
  	void Setup();
  	void Loop();
    float StandardDeviation(int32_t dataset[], int32_t SIZE);
    float Mean(int32_t dataset[], int32_t SIZE);
    void OutputThroughSerial();
    virtual void TFTOutput();
};