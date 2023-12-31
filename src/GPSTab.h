#pragma once

#include <Arduino.h>
#include <Tab.h>

class GPSTab : public Tab
{
  private:
	const double scalingFactor = 1*pow(10, 6);
	const double maxLatRad = 1.570796309;//89.999999(degrees) needs to be 1 more 9 than there are significant 0s
	const double maxMercLat = 89.99999;
	const uint16_t YOffset = 61;//61

	const uint16_t worldMapWidth = 480;
	const uint16_t worldMapHeight = 239;
	const uint16_t worldMapXOffset = 0;
	const int16_t worldXCursurOffset = - 3;
	const int16_t worldYCursurOffset = - 11;

	const uint16_t euMapWidth = 480;
	const uint16_t euMapHeight = 239;
	const uint16_t euMapXOffset = 0;
	const double euMaxLat = 73.5;
	const double euMinLat = 34;
	const double euMaxLong = 65;
	const double euMinLong = -25;
	const int16_t euXCursurOffset = -6;
	const int16_t euYCursurOffset = -8;

	const uint16_t brMapWidth = 480;
	const uint16_t brMapHeight = 239;
	const uint16_t brMapXOffset = 0;
	const double brMaxLat = 61.1;
	const double brMinLat = 49.4;
	const double brMaxLong = 8.3;
	const double brMinLong = -14.5;
	const int16_t brXCursurOffset = -6;
	const int16_t brYCursurOffset = -8;

	uint16_t mapWidth;
	uint16_t mapOffset;
	double maxLat;
	double minLat;
	double maxLong;
	double minLong;

	const uint8_t numberOfMaps = 3;

	int16_t xCursurOffset = -3; //-6 - an amount
	int16_t yCursurOffset = -9;
  	double latitude;
  	double longitude;
	int8_t verticalIndex = 0;
	int8_t prevVerticalIndex = 0;
	int32_t prevEncoderBValue;
  	int8_t satellites;
	uint16_t altitude;
	int16_t xScreenPos;
	int16_t yScreenPos;
	int16_t oldXScreenPos;
	int16_t oldYScreenPos;
	uint64_t timer;
	bool prevLockedState;
	
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
	//converts the latitude to an y coordinate for the screen with mercator projections
	double MercLatToYPos(double lat); 
	//converts the latitude to an x coordinate for the screen with mercator projections
	double MercLonToXPos(double lon, double lat);
	//converts the latitude to an y coordinate for the screen with equirectangular projections
	double EquirectangularYPos(double lat);
	//converts the longitude to an x coordinate for the screen with equirectangular projections
	double EquirectangularXPos(double lon, double lat);
	void OutputThroughSerial();
	void TFTOutput();
	void TFTDrawOptions();
	void TFTDrawMap();
};