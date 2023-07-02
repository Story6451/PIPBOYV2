#include <GPSTab.h>
#include <Adafruit_TFTLCD.h>
#include <SD.h>
#include <Colours.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>

TinyGPSPlus gps;

static const int RXPin = 11;
static const int TXPin = 10;
static const uint32_t GPSBaud = 9600;

SoftwareSerial ss(RXPin, TXPin);


GPSTab::GPSTab(Adafruit_TFTLCD *ptft, ConfigData *pPipData):Tab(ptft, pPipData)
{
}

void GPSTab::Setup()
{
    latitude = 0;
    longitude = 0;
    velocity = 0;
    satellites = 0;
    hours = 0;
	minutes = 0;
	seconds = 0;
  	year = 0;
	month = 0;
	day = 0;   
    altitude = 0; 
    xPos = 0;
    yPos = 0;
    xScreenPos = 0;
    yScreenPos = 0;
    oldXScreenPos = 0;
    oldYScreenPos = 0;

    timer = 0;
}

void GPSTab::Loop()
{
    if ((millis() - timer) > 500)
    {
        ss.begin(GPSBaud);
        timer = millis();
        if (gps.charsProcessed() > 0)
        {

            if (gps.satellites.isValid())
                satellites = gps.satellites.value();
            //GPSDelay(0);

            if (gps.altitude.isValid())
                altitude = gps.altitude.meters();
            //GPSDelay(0);

            if (gps.location.isValid())
            {
                latitude = gps.location.lat();
                longitude = gps.location.lng();
            }
            //GPSDelay(0);

            double correctedLat = latitude;
            if (abs(latitude) > maxLat)
            {
                correctedLat = maxLat;
                if (latitude < 0)
                    correctedLat = -latitude;
                
            }

            //yPos = LatToScreenYPos(correctedLat);
            xPos = LonToXPos(longitude);

            //calculate screen position
            //xScreenPos = map(xPos, 0, 2 * PI, 0, 480);//480 needs to be changed to be the drawing box width
            //yScreenPos = map(yPos, RadToMercRadians(maxLat * PI/180), 0, 0, 320);//320 needs to be changed to be the drawing box height
            //yScreenPos = (yPos - RadToMercRadians(maxLat * (PI/180)))/(-RadToMercRadians(maxLat * PI/180)) * 320;
            xScreenPos = xPos * ((480 - (2 * offset))/(2 * PI));

            //yScreenPos = (320/2)-(320 * RadToMercRadians((correctedLat * PI/180)/(2 * PI)));
            //xScreenPos = (longitude + 180) - (480/360);
            
            yScreenPos = LatToScreenYPos(correctedLat);
            //xScreenPos = LonToXPos(longitude);
            OutputThroughSerial();
            TFTOutput();
        }
        else
        {
            Serial.println("Waiting for gps data!");
            pTFT->setCursor(90, 130);
            pTFT->setTextSize(3);
            pTFT->setTextColor(pPIPDATA->ActiveColour, BLACK);
            pTFT->print("Waiting for gps data!");
            pTFT->fillRect(0, 61, 480, 259, BLACK);
        }
        GPSDelay(500);

        ss.end();
    }
    
}

void GPSTab::GPSDelay(uint64_t diff)
{
    uint64_t start = millis();
    do
    {
        while(ss.available())
            gps.encode(ss.read());
    } while ((millis() - start) < diff);
}

double GPSTab::RadToMercRadians(double latRad)
{
    //Serial.print("A1: "); Serial.print(latRad, 4); Serial.print(" ");
    double temp = tan((PI/4) + (latRad/2));
    //Serial.print("A2: "); Serial.print(temp, 4); Serial.print(" ");
    if (temp == 0)
    {
        latRad = latRad + __DBL_EPSILON__;
        temp = tan((PI/4) + (latRad/2));
    }
    //Serial.print("A3: "); Serial.print(log(temp), 4); Serial.print(" ");
    return log(temp);
}

double GPSTab::MercRadToLat(double mercRad)
{
    return 2 * atan(exp(mercRad)) - PI/2;
}

double GPSTab::LatToScreenYPos(double lat)
{
    //return (RadToMercRadians(maxLat * PI/180) - RadToMercRadians(lat * PI/180)) * scalingFactor;
    //return (RadToMercRadians(maxLat * PI/180) - RadToMercRadians(lat * PI/180));
    //return (RadToMercRadians(maxLat * PI/180) - RadToMercRadians(lat * PI/180));// * scalingFactor;
    return (239/2)-(239 * RadToMercRadians((lat * PI/180)/(2 * PI)));//239 being 320 - 61(offset from tabs) - 20(offset from displayed gps data at bottom)
}

double GPSTab::LonToXPos(double lon)
{
    //return ((longitude * PI/180) + PI) * scalingFactor;
    return (longitude * PI/180) + PI;
    //return (longitude + 180) - (480/360);
}

void GPSTab::OutputThroughSerial()
{
    Serial.print("lat: "); Serial.print(latitude, 7); Serial.print(" ");
    Serial.print("long: "); Serial.print(longitude, 7); Serial.print(" ");
    Serial.print("NoSatellites: "); Serial.print(satellites); Serial.println(" ");
    /*
    Serial.print("raw x: "); Serial.print(xPos, 7); Serial.print(" ");
    Serial.print("raw y: "); Serial.print(yPos, 7); Serial.print(" ");
    Serial.print("screen x: "); Serial.print(xScreenPos); Serial.print(" ");
    Serial.print("screen y: "); Serial.print(yScreenPos); Serial.println(" ");
    */
    
}

void GPSTab::TFTOutput()
{
    
    pTFT->fillCircle(xScreenPos + offset, yScreenPos + offset, 2, pPIPDATA->ActiveColour);
    if ((oldXScreenPos != xScreenPos) || (oldYScreenPos != yScreenPos))
    {
        pTFT->fillCircle(oldXScreenPos, oldYScreenPos, 2, BLACK);
        oldXScreenPos = xScreenPos;
        oldYScreenPos = yScreenPos;
    }
    
    //pTFT->setTextSize(2);
    //pTFT->setCursor(100, 100);
    //pTFT->print(ModuleName());

    pTFT->setTextColor(pPIPDATA->ActiveColour, BLACK);
    pTFT->setCursor(0, 300);
    pTFT->setTextSize(1);
    pTFT->print("Latitude: "); pTFT->print(latitude, 6); pTFT->print("   ");
    pTFT->print("Longitude: "); pTFT->print(longitude, 6); pTFT->print("   ");
    pTFT->print("Altitude: "); pTFT->print(altitude, 2); pTFT->print("   ");
    pTFT->print("Satellites: "); pTFT->print(satellites); pTFT->print("   ");

}
/*
latitude    = 41.145556; // (φ)
longitude   = -73.995;   // (λ)

mapWidth    = 200;
mapHeight   = 100;

// get x value
x = (longitude+180)*(mapWidth/360)

// convert from degrees to radians
latRad = latitude*PI/180;

// get y value
mercN = ln(tan((PI/4)+(latRad/2)));
y     = (mapHeight/2)-(mapHeight*mercN/(2*PI));
*/

//for zoomed in maps like map of uk values of pixel position need a multiplier so that it will be shifted the appropriate amoung based on the position of the pointer on the map