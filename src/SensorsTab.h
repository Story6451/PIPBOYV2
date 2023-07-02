#pragma once

#include <Arduino.h>
#include <Tab.h>

class SensorsTab : public Tab //get sensors array
{
  private:
  	float temperature;
  	float pressure;
  	byte COConcentration;
  	String time;
  	String date;

	bool clockFound = false;
  public:  

   	SensorsTab(Adafruit_TFTLCD *ptft, ConfigData *pPipData);
	String ModuleName(){return "ENVIRO";}
  	void Setup();
  	void Loop();
    String GetDateTime(); //gets the time, date and arporiatly formats it before displaying it 
    float GetTemperature();
    float GetPressure();
	float GetAltitude();
	float GetCO();
	void OutputThroughSerial();
	void TFTOutput();
};