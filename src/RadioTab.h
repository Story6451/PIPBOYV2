#pragma once

#include <Arduino.h>
#include <Tab.h>

class RadioTab : public Tab //get fm radio reciever
{
  private:
  	float frequency;
	bool selected;
	bool radioFound = false;
  public:
  	RadioTab(Adafruit_TFTLCD *ptft, ConfigData *pPipData);
	String ModuleName(){return "RADIO";}
  	void Setup();
  	void Loop();
	void OutputThroughSerial(String output);
	void TFTOutput();
};