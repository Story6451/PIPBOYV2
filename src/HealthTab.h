#pragma once

#include <Arduino.h>
#include <Tab.h>

class HealthTab : public Tab //Add oxygen concentration reading features
{
  private:
    static const int BUFFER_LENGTH = 100;
    int32_t rawVal;
    int32_t values[BUFFER_LENGTH];
    int32_t times[BUFFER_LENGTH];
    int64_t time1;
    int64_t time2;
    int32_t delta;
    int16_t bpm;
    int16_t preBpm;
    int16_t bpmRepeats;
    int16_t meanBpm;
    int32_t deltas[BUFFER_LENGTH];
    int16_t meanDelta;
    int32_t validBpm[BUFFER_LENGTH];
    int32_t bpms[BUFFER_LENGTH];
    int8_t arrayPosition;
    int32_t peak;
    bool beat;
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