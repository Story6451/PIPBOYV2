#include <GPSTab.h>
#include <Adafruit_TFTLCD.h>
#include <SD.h>
#include <Colours.h>
#include <TinyGPS++.h>
#include <SoftwareSerial.h>
#include <Graphics.h>

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

    pTFT->drawBitmap(XOffset, YOffset, WorldMap2, MapWidth, MapHeight, pPIPDATA->ActiveColour);
}

void GPSTab::Loop()
{
    if ((millis() - timer) > 500)
    {
        ss.begin(GPSBaud);
        timer = millis();
        if (gps.charsProcessed() > 0)
        {

            //if (gps.satellites.isValid())
            {
                satellites = gps.satellites.value();
            }
            //GPSDelay(0);

            //if (gps.altitude.isValid())
            {
                altitude = gps.altitude.meters();
            }
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
                {
                    correctedLat = -latitude;
                }

            }

            //calculate screen position
            ///*SUDAN 16.465764445765306, 29.54627685797427
            //correctedLat = 16.465764445765306;
            //longitude = 29.54627685797427;
            
            ///*JAPAN 36.54343554246808, 139.25918808522488
            //correctedLat = 36.54343554246808;
            //longitude = 139.25918808522488;
            
            //NEW ZEALAND -42.088667969077676, 172.22672190419098
            //correctedLat = -42.088667969077676;
            //longitude = 172.22672190419098;
            
            ///*CANADA 58.231708432811345, -101.43666168828908
            //correctedLat = 58.231708432811345;
            //longitude = -101.43666168828908;
            
            //CHILE -26.505485584708655, -69.43743084284017
            //correctedLat = -26.505485584708655;
            //longitude = -69.43743084284017;
            
            //yScreenPos = MercLatToYPos(correctedLat);
            //xScreenPos = MercLonToXPos(longitude, correctedLat);
            xScreenPos = EquirectangularLongToXPos(longitude, correctedLat);
            yScreenPos = EquirectangularLatToYPos(correctedLat);
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
        GPSDelay(100);

        ss.end();
    }
    
}

void GPSTab::GPSDelay(uint64_t diff)
{
    uint64_t start = millis();
    do
    {
        while(ss.available())
        {   
            gps.encode(ss.read());
        }
    } while ((millis() - start) < diff);
}

double GPSTab::RadToMercRadians(double latRad)
{
    double temp = tan((PI/4) + (latRad/2));
    if (temp == 0)
    {
        latRad = latRad + __DBL_EPSILON__;
        temp = tan((PI/4) + (latRad/2));
    }
    return log(temp);
}

double GPSTab::MercRadToLat(double mercRad)
{
    return 2 * atan(exp(mercRad)) - PI/2;
}

double GPSTab::MercLatToYPos(double lat)
{
    double mercRad = RadToMercRadians((lat * PI/180));
    return ((double)MapHeight/2)-((double)MapWidth * mercRad/(2 * PI));//239 being 320 - 61(offset from tabs) - 20(offset from displayed gps data at bottom)
}

double GPSTab::MercLonToXPos(double lon, double lat)
{
    return (lon + 180) * ((double)MapWidth/360); 
}

double GPSTab::EquirectangularLongToXPos(double lon, double lat)
{
    return ((double)MapWidth/360) * (lon + 180);
}

double GPSTab::EquirectangularLatToYPos(double lat)
{
    return (double)MapHeight-((lat + 90) * (double)MapHeight/180);
}

void GPSTab::OutputThroughSerial()
{
    Serial.print("lat: "); Serial.print(latitude, 7); 
    Serial.print(" long: "); Serial.print(longitude, 7); 
    Serial.print(" ypos: "); Serial.print(yScreenPos); 
    Serial.print(" xpos: "); Serial.print(xScreenPos); 
    Serial.print(" Altitude: "); Serial.print(altitude); 
    Serial.print(" No.Satellites: "); Serial.print(satellites); Serial.println(" ");
}

void GPSTab::TFTOutput()
{
    if ((oldXScreenPos != xScreenPos) || (oldYScreenPos != yScreenPos))
    {
        pTFT->fillRect(oldXScreenPos + XOffset - 6, oldYScreenPos + YOffset - 6, 12, 12, BLACK);

        pTFT->drawBitmap(XOffset, YOffset, WorldMap2, MapWidth, MapHeight, pPIPDATA->ActiveColour);

        oldXScreenPos = xScreenPos;
        oldYScreenPos = yScreenPos;
    }
    
    pTFT->drawBitmap(xScreenPos + XOffset - 6, yScreenPos + YOffset - 6, CrossHair, 12, 12, 0xFFFF);

    pTFT->setTextColor(pPIPDATA->ActiveColour, BLACK);
    pTFT->setCursor(10, 300);
    pTFT->setTextSize(1);
    pTFT->print(" Latitude: "); pTFT->print(latitude, 6); 
    pTFT->print(" Longitude: "); pTFT->print(longitude, 6); 
    pTFT->print(" Altitude: "); pTFT->print(altitude, 2); 
    pTFT->print(" Satellites: "); pTFT->print(satellites); pTFT->print(" ");

}
