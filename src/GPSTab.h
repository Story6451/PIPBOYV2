#pragma once

#include <Arduino.h>
#include <Tab.h>

class GPSTab : public Tab
{
  private:
	const double scalingFactor = 1*pow(10, 6);
	const double maxLatRad = 1.570796309;//89.999999(degrees) needs to be 1 more 9 than there are significant 0s
	const double maxLat = 89.99999;
	const uint16_t WorldMapWidth = 459;
	const uint16_t WorldMapHeight = 239;
	const uint16_t AfricaMapWidth = 231;
	const uint16_t AfricaMapHeight = 239;
	const uint16_t AsiaMapWidth = 0;
	const uint16_t AsiaMapHeight = 0;
	const uint16_t AustraliaMapWidth = 0;
	const uint16_t AustraliaMapHeight = 0;
	const uint16_t EuropeMapWidth = 480;
	const uint16_t EuropeMapHeight = 239;
	const uint16_t NorthAmericaMapWidth = 480;
	const uint16_t NorthAmericaHeight = 239;
	const uint16_t SouthAmericaMapWidth = 230;
	const uint16_t SouthAmericaMapHeight = 239;

	const uint16_t XOffset = 10;//10
	const uint16_t YOffset = 61;//61
	const int16_t xCursurOffset = -10; //-6 - an amount
	const int16_t yCursurOffset = -9;
	const uint8_t numberOfMaps = 7;
  	double latitude;
  	double longitude;
	uint8_t verticalIndex = 0;
	uint8_t oldIndex = 0;
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
	double EquirectangularLatToYPos(double lat);
	//converts the longitude to an x coordinate for the screen with equirectangular projections
	double EquirectangularLongToXPos(double lon, double lat);
	void OutputThroughSerial();
	void TFTOutput();
	void TFTDrawOptions();
	void TFTDrawMap();
};