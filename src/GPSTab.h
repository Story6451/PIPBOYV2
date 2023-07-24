#pragma once

#include <Arduino.h>
#include <Tab.h>

class GPSTab : public Tab
{
  private:
	const double scalingFactor = 1*pow(10, 6);
	const double maxLatRad = 1.570796309;//89.999999(degrees) needs to be 1 more 9 than there are significant 0s
	const double maxLat = 89.99999;
	const uint16_t MapWidth = 415;
	const uint16_t MapHeight = 239;
	const uint16_t XOffset = 33;
	const uint16_t YOffset = 61;
  	double latitude;
  	double longitude;
  	int8_t satellites;
  	float velocity;
  	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;
  	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint16_t altitude;
	double xPos;
	double yPos;
	int16_t xScreenPos;
	int16_t yScreenPos;
	int16_t oldXScreenPos;
	int16_t oldYScreenPos;
	const int16_t offset = 61; //distance from tabs
	uint64_t timer;
	
  public:
  	GPSTab(Adafruit_TFTLCD *ptft, ConfigData *pPipData);
	String ModuleName(){return "MAP";}
  	void Setup();
  	void Loop();
	//reads gps data
	void GPSDelay(uint64_t diff);
	//convert radians to merc radians
	double RadToMercRadians(double latRad);
	//convert merc radians to latitude
	double MercRadToLat(double mercRad);
	//converts the latitude to an y coordinate for the screen
	double LatToYPos(double lat);
	//converts the latitude to an x coordinate for the screen
	double LonToXPos(double lon);
	void OutputThroughSerial();
	void TFTOutput();
};