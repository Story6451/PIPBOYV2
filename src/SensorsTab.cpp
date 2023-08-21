#include <SensorsTab.h>
#include <SD.h>
#include <RTClib.h>
#include "SparkFunMPL3115A2.h"
//#include <MQ7.h>
#include <Adafruit_TFTLCD.h>
#include <Colours.h>

//#define COSensorPin 6
//#define voltage 5

RTC_DS3231 clock;
MPL3115A2 barometer;
//MQ7 mq7(COSensorPin, voltage);

SensorsTab::SensorsTab(Adafruit_TFTLCD *ptft, ConfigData *pPipData):Tab(ptft, pPipData)
{
    if (!clock.begin()) 
    {
        pTFT->println("Could not find clock");
        Serial.println("Could not find clock");
        //abort();
    }
    else
    {
        clockFound = true;
        pTFT->println("Found Clock");
        Serial.println("Found Clock");
    }
    clock.adjust(DateTime(F(__DATE__), F(__TIME__)));
    barometer.begin();

    barometer.setModeBarometer();

    barometer.setOversampleRate(7);  

    barometer.enableEventFlags(); 
}

void SensorsTab::Setup()
{
    temperature = 0;
    pressure = 0;
    COConcentration = 0;
    date = "";
    time = "";
}

void SensorsTab::Loop()
{
    //OutputThroughSerial();
    TFTOutput();
}

float SensorsTab::GetPressure()
{
    return barometer.readPressure();
}

float SensorsTab::GetTemperature()
{
    return barometer.readTemp();
}

float SensorsTab::GetAltitude() //uses a formula to calculate the altitude from the pressure reading
{
    float pressureAtSeaLevel = 101325;
    float x = (-8.31432 * -0.0065)/(9.80665 * 0.0289644);
    float alt = (288.15/-0.0065) * (pow(barometer.readPressure()/pressureAtSeaLevel, x) - 1);
    return alt + pPIPDATA->AltitudeOffset;
}

float SensorsTab::GetCO()
{
    //return mq7.readPpm();
}

String SensorsTab::GetDateTime()
{
    DateTime now = clock.now();
    
    String year;
    year = now.year();

    String month;
    month = "0";
    if (now.month() < 10)
        month += now.month();
    else
        month = now.month();
    
    String day;
    day = "0";
    if (now.day() < 10)
        day += now.day();
    else
        day = now.day();

    date = day + '/' + month + '/' + year + ' ';

    String hour;
    hour = "0";
    if (now.hour() < 10)
        hour += now.hour(); 
    else
        hour = now.hour();

    time = hour + ':';

    String minutes;
    minutes = "0";
    if (now.minute() < 10)
        minutes += now.minute();
    else
        minutes = now.minute();

    time += minutes + ':';
    
    String seconds;
    seconds = '0';
    if (now.second() < 10)
        seconds +=now.second();
    else
        seconds = now.second();

    time += seconds;

    return date + time;       
}

void SensorsTab::OutputThroughSerial()
{
    Serial.print(GetDateTime());
    Serial.print(" Pressure: ");
    Serial.print(GetPressure());
    Serial.print(" Pa");
    Serial.print(" Altitude: ");
    Serial.print(GetAltitude());
    Serial.print(" m");
    Serial.print(" Temperature: ");
    Serial.print(GetTemperature());
    Serial.println("°C ");
    //Serial.print(" CO PPM: ");
    //Serial.println(GetCO());

    //delay(100);
}

void SensorsTab::TFTOutput()
{
    pTFT->setTextColor(pPIPDATA->ActiveColour, BLACK);
    pTFT->setTextSize(pPIPDATA->TextSize);
    
    //displays date and time
    pTFT->setCursor(10, 100);
    pTFT->print("Date: "); pTFT->println(GetDateTime());
    //displays pressure
    pTFT->setCursor(10, 130);
    pTFT->print("Pressure: "); pTFT->print(GetPressure()); pTFT->println(" Pa ");
    //displays altitude
    pTFT->setCursor(10, 160);
    pTFT->print("Altitude: "); pTFT->print(GetAltitude()); pTFT->println(" m ");

    //displays temperature
    pTFT->setCursor(10, 190);
    pTFT->print("Temperature: "); pTFT->print(GetTemperature()); pTFT->println(" 'C "); 
    
    Serial.println("Testing data tab");
}