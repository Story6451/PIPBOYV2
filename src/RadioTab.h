#pragma once

#include <Arduino.h>
#include <Tab.h>

class RadioTab : public Tab //get fm radio reciever
{
  private:
	int16_t prevEncoderAValue;
	int16_t prevEncoderBValue;
  	float frequency;
	int8_t signalStrength;
	bool selected;
	
  public:
  	RadioTab(Adafruit_TFTLCD *ptft, ConfigData *pPipData);
	String ModuleName(){return "RADIO";}
  	void Setup();
  	void Loop();
	void OutputThroughSerial(String output);
	void TFTOutput();
};