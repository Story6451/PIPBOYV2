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
    satellites = 0; 
    altitude = 0; 
    xScreenPos = 0;
    yScreenPos = 0;
    oldXScreenPos = 0;
    oldYScreenPos = 0;
    prevLockedState = pPIPDATA->Locked;
    timer = 0;
    //verticalIndex = 0;
    //oldIndex = 0;
    prevEncoderBValue = -1;
    mapWidth = 0;
	mapOffset = 0;
	maxLat = 0;
	minLat = 0;
	maxLong = 0;
	minLong = 0;
    TFTDrawMap();
}

void GPSTab::Loop()
{
    if ((millis() - timer) > 500)
    {
        if (pPIPDATA->Locked != prevLockedState)
        {
            pTFT->fillRect(0, 61, 480, 259, BLACK);
            prevLockedState = pPIPDATA->Locked;
            if (pPIPDATA->Locked == false)
            {
                TFTDrawMap();
            }
        }

        if (pPIPDATA->Locked == false)
        {

            ss.begin(GPSBaud);
            timer = millis();
            if (gps.charsProcessed() > 0)
            {
                if (gps.satellites.isValid())
                {
                    satellites = gps.satellites.value();
                }

                if (gps.altitude.isValid())
                {
                    altitude = gps.altitude.meters();
                }

                if (gps.location.isValid())
                {
                    latitude = gps.location.lat();
                    longitude = gps.location.lng();
                }

                double correctedLat = latitude;
                if (abs(latitude) > maxMercLat)
                {
                    correctedLat = maxMercLat;
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

                uint16_t posX = EquirectangularXPos(longitude, correctedLat);
                uint16_t posY = EquirectangularYPos(correctedLat);
                uint16_t minX = EquirectangularXPos(minLong, minLat);
                uint16_t minY = EquirectangularYPos(minLat);
                uint16_t maxX = EquirectangularXPos(maxLong, maxLat);
                uint16_t maxY = EquirectangularYPos(maxLat);
                xScreenPos = (double)mapOffset + (double)mapWidth * (double)(posX - minX)/(maxX - minX);
                yScreenPos = (double)worldMapHeight - (double)worldMapHeight * (double)(posY - minY)/(maxY - minY);
                
                /*
                Serial.print("minlat, maxlat: "); Serial.print(minLat); Serial.print(" "); Serial.println(maxLat);
                Serial.print("minlong, maxlong: "); Serial.print(minLong); Serial.print(" "); Serial.println(maxLong);
                Serial.print("miny, maxy: "); Serial.print(minY); Serial.print(" "); Serial.println(maxY);
                Serial.print("minx, maxx: "); Serial.print(minX); Serial.print(" "); Serial.println(maxX);
                Serial.print("posy, yscreenpos "); Serial.print(posY); Serial.print(" "); Serial.println(yScreenPos);
                Serial.print("posx, xscreenpos "); Serial.print(posX); Serial.print(" "); Serial.println(xScreenPos);
                */
                //OutputThroughSerial();
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
            GPSDelay(50);

            ss.end();
        }
        else
        {
            //verticalIndex = round((analogRead(pPIPDATA->POT_1_PIN)*numberOfMaps)/1023);
            //verticalIndex = pPIPDATA->encoderBValue;
            if (pPIPDATA->encoderBValue > prevEncoderBValue)
            {
                verticalIndex++;
            }
            else if (pPIPDATA->encoderBValue < prevEncoderBValue)
            {
                verticalIndex--;
            }

            if (verticalIndex > 2)
            {
                verticalIndex = 2;
            }
            else if (verticalIndex < 0)
            {
                verticalIndex = 0;
            }
            /*
            Serial.print("  Vertical Index: "); Serial.print(verticalIndex);
            Serial.print("  Encoder B value: "); Serial.print(pPIPDATA->encoderBValue);
            Serial.print("  Previous encoder B value: "); Serial.println(prevEncoderBValue);
            */
            prevEncoderBValue = pPIPDATA->encoderBValue;
            TFTDrawOptions();
        }
    }
}

void GPSTab::TFTDrawOptions()
{
    pTFT->setTextSize(pPIPDATA->TextSize);
    pTFT->setCursor(20, 75);
    pTFT->print("World Map");
    pTFT->setCursor(20, 161);
    pTFT->print("Continental Map");
    pTFT->setCursor(20, 247);
    pTFT->print("Country Map");

    if (verticalIndex != prevVerticalIndex)
    {
        pTFT->drawRect(10, prevVerticalIndex * 255/numberOfMaps + 70, 460, 33, BLACK);
        prevVerticalIndex = verticalIndex;
        
    }
    pTFT->drawRect(10, verticalIndex * 255/numberOfMaps + 70, 460, 33, pPIPDATA->ActiveColour);

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
    return ((double)worldMapHeight/2)-((double)mapWidth * mercRad/(2 * PI));//239 being 320 - 61(offset from tabs) - 20(offset from displayed gps data at bottom)
}

double GPSTab::MercLonToXPos(double lon, double lat)
{
    return (lon + 180) * ((double)mapWidth/360); 
}

double GPSTab::EquirectangularXPos(double lon, double lat)
{
    return ((double)mapWidth/360) * (lon + 180);
}

double GPSTab::EquirectangularYPos(double lat)
{
    return ((lat + 90) * (double)worldMapHeight/180);
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
        pTFT->fillRect(oldXScreenPos + xCursurOffset, oldYScreenPos + YOffset + yCursurOffset, 12, 12, BLACK);

        TFTDrawMap();


        oldXScreenPos = xScreenPos;
        oldYScreenPos = yScreenPos;
    }
    
    pTFT->drawBitmap(xScreenPos + xCursurOffset, yScreenPos + YOffset + yCursurOffset, CrossHair, 12, 12, 0xFFFF);

    pTFT->setTextColor(pPIPDATA->ActiveColour, BLACK);
    pTFT->setCursor(10, 300);
    pTFT->setTextSize(1);
    pTFT->print(" Latitude: "); pTFT->print(latitude, 6); 
    pTFT->print(" Longitude: "); pTFT->print(longitude, 6); 
    pTFT->print(" Altitude: "); pTFT->print(altitude, 2); 
    pTFT->print(" Satellites: "); pTFT->print(satellites); pTFT->print(" ");
    Serial.println("Testing maps tab");
}

void GPSTab::TFTDrawMap()
{
    
    switch (verticalIndex)
    {
        case 1:
            pTFT->drawBitmap(euMapXOffset, YOffset, EuropeMap, euMapWidth, euMapHeight, pPIPDATA->ActiveColour);
            maxLat = euMaxLat;
            minLat = euMinLat;
            maxLong = euMaxLong;
            minLong = euMinLong;
            mapWidth = euMapWidth;
            mapOffset = euMapXOffset;
            xCursurOffset = euXCursurOffset;
            yCursurOffset = euYCursurOffset;
            break;
        case 2:
            pTFT->drawBitmap(brMapXOffset, YOffset, BritianMap, brMapWidth, brMapHeight, pPIPDATA->ActiveColour);
            maxLat = brMaxLat;
            minLat = brMinLat;
            maxLong = brMaxLong;
            minLong = brMinLong;
            mapWidth = brMapWidth;
            mapOffset = brMapXOffset;
            xCursurOffset = brXCursurOffset;
            yCursurOffset = brYCursurOffset;
            break;
        default:
            pTFT->drawBitmap(worldMapXOffset, YOffset, WorldMap, worldMapWidth, worldMapHeight, pPIPDATA->ActiveColour);
            maxLat = 90;
            minLat = -90;
            maxLong = 180;
            minLong = -180;
            mapWidth = worldMapWidth;
            mapOffset = worldMapXOffset;
            xCursurOffset = worldXCursurOffset;
            yCursurOffset = worldYCursurOffset;
            break;
    }
    
}
