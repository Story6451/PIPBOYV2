#include <Arduino.h>
#include <SPI.h>

#include <Adafruit_TFTLCD.h>
#include <Adafruit_GFX.h>
#include <Colours.h>

#include <SD.h>

#include <Tab.h>
#include <OptionsTab.h>
#include <ConfigData.h>

#include <HealthTab.h>
#include <SensorsTab.h>
#include <GPSTab.h>
#include <RadioTab.h>

#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0

#define LCD_RESET A4

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

ConfigData pipData;

Tab* Tabs[5];

int Tab::Total = -1;

//button pins
const int BTN_1_PIN = 5;//refreshing button
const int BTN_2_PIN = 7;//locking button


//led pins (all unused)
const int LED_1_PIN = 6; 
const int LED_2_PIN = 7;
const int LED_3_PIN = 8;

int val1 = 0;
int val2 = 0;

bool locked = false;
int prevBtnState1 = LOW;
int oldIndex = 0;
int index = 0;
int prevPipColour;

word incrementalStep;

Sd2Card card;
SdVolume volume;

const int chipSelect = 53;

void SerialSetupLogger() //logs tft info to the serial monitor
{
  Serial.println("Hello, world!");

  if (tft.readID() == 0x8357)
    Serial.println("Found LCD");
  else 
  {
    Serial.print("Could not find LCD Instead found tft with identifier: "); Serial.println(tft.readID());
  }
  
  //screen information
  Serial.print("Identifier: "); Serial.println(tft.readID());
  Serial.print("Height: "); Serial.println(tft.height());
  Serial.print("Width: ");Serial.println(tft.width());

}

void PeripheralSetup() //initialises the external hardware and logs it to the tft and console
{
  //tft setup
  tft.reset();
  tft.begin(tft.readID());
  tft.fillScreen(BLACK);
  tft.setTextSize(2);
  tft.setRotation(1);
  tft.setCursor(0,0);
  tft.setTextColor(pipData.ActiveColour, BLACK);
  tft.println("Pip boy initialisation has begun...");
  Serial.println("Pip boy initialisation has begun...");

  //Sd card setup
  if (card.init(SPI_HALF_SPEED, chipSelect))
  {
    tft.println("SD card 1 initialisation success");
    Serial.println("SD card 1 initialisation success");
  }
  else
  {
    tft.println("SD card 1 initialisation failed");
    Serial.println("SD card 1 initialisation failed");
  }

  if (SD.begin(chipSelect))
  {
    tft.println("SD card 2 initialisation success");
    Serial.println("SD card 2 initialisation success");
  }
  else
  {
    tft.println("SD card 2 initialisation failed");
    Serial.println("SD card 2 initialisation failed");
  }

  //Button inputs
  pinMode(BTN_1_PIN, INPUT); 
  pinMode(BTN_2_PIN, INPUT);
  pinMode(pipData.BTN_3_PIN, INPUT);
  tft.println("Buttons connected");
  Serial.println("Buttons connected");

  //LED Outputs
  pinMode(LED_1_PIN, OUTPUT);
  pinMode(LED_2_PIN, OUTPUT);
  pinMode(LED_3_PIN, OUTPUT);
  tft.println("LEDs connected");
  Serial.println("LEDs connected");

  //initialises tab objects
  Tabs[Tab::Total] = new HealthTab(&tft, &pipData); 
  Tabs[Tab::Total] = new SensorsTab(&tft, &pipData);
  Tabs[Tab::Total] = new GPSTab(&tft, &pipData);
  Tabs[Tab::Total] = new RadioTab(&tft, &pipData);
  Tabs[Tab::Total] = new OptionsTab(&tft, &pipData);
  tft.println("Class objects built");
  Serial.println("Class objects built");

  delay(500);
  incrementalStep = 1023/(Tab::Total+1); 

  for (int i = 0; i < Tab::Total + 1; i++) //initialises all of the tabs
  {
    //Tabs[i]->Setup();
    tft.print("Module: "); tft.print(Tabs[i]->ModuleName()); tft.println(", setup complete");
    Serial.print("Module: "); Serial.print(Tabs[i]->ModuleName()); Serial.println(", setup complete");
  }
  tft.fillScreen(BLACK);


  //classic pip boy os splash screen from fallout 4
  tft.setCursor(0,0);
  tft.println("*********** PIP-OS V1.0.0.0 ************");
  tft.println();
  tft.println("COPYRIGHT 2073");
  tft.println("8KB RAM SYSTEM");
  tft.println("256KB INTERNAL FLASH");
  if (volume.init(card))
  {
    tft.print("VOLUME TPYE: "); tft.println(volume.fatType(), DEC);
    float volumeSize = 0;
    volumeSize = volume.blocksPerCluster();
    volumeSize *= volume.clusterCount();
    volumeSize /= 2;
    tft.print("SD CARD VOLUME SIZE (KB): "); tft.println(volumeSize);
    volumeSize /= 1024;
    tft.print("SD CARD VOLUME SIZE (MB): "); tft.println(volumeSize);
    volumeSize /= 1024;
    tft.print("SD CARD VOLUME SIZE (GB): "); tft.println(volumeSize);
  }
  else
  {
    tft.println("NO PARTITION DETECTED");
  }

  tft.println("NO HOLOTAPE FOUND");

  delay(1000);
  tft.fillScreen(BLACK);
}

void DrawMenuCursur(int index, int oldIndex)
{
  tft.fillRect(0, 61, 480, 259, BLACK);

  tft.drawRoundRect(oldIndex * tft.width()/(Tab::Total + 1) + 1, 10, (tft.width()/Tab::Total + 1) - (tft.width()/(Tab::Total + 1 * Tab::Total + 1)) + 20, 40, 10, BLACK);
  tft.drawRoundRect(index * tft.width()/(Tab::Total + 1) + 1, 10, (tft.width()/Tab::Total + 1) - (tft.width()/(Tab::Total + 1 * Tab::Total + 1)) + 20, 40, 10, pipData.ActiveColour);
}

void DrawTabs() //draws the top menu bar
{
  tft.setTextColor(pipData.ActiveColour, BLACK);
  //Tab Line
  tft.drawFastHLine(0, 60, 480, pipData.ActiveColour);

  //Tab Labels
  tft.setTextSize(2);
  for (byte i = 0; i < Tab::Total + 1; i++)
  {
    tft.setCursor((i * tft.width()/(Tab::Total + 1)) + 5, 20);
    tft.print(Tabs[i]->ModuleName());
  }
}

void GetSettings()
{
  Serial.println("in");
  String data = "";
  if (SD.exists("config.txt"))
  {
    File file = SD.open("config.txt");
    
    String datas[3];
    if (file)
    {
      Serial.println("file available");
      int count = 0;
      while (file.available())
      {
        char stream = file.read();
        Serial.print(stream);        
        if (stream == ',')
        {
          datas[count] = data;
          count++;
          data = "";
        }
        else
        {
          data += stream;
        }
        
      }
      file.close();
    }

    Serial.println(" ");
    for (int i = 0; i < 3; i++)
    {
      Serial.print("data: "); Serial.println(datas[i].toInt());
    }
    
    if (datas[0] == 0)
    {
      datas[0] = 9537;
    }
    pipData.ActiveColour = datas[0].toInt();
    pipData.AltitudeOffset = datas[1].toInt();
    pipData.Volume = datas[2].toInt();
    
  }
  else
  {
    File file = SD.open("config.txt");
    file.close();
  }
}

void setup() 
{
  //serial setup
  Serial.begin(9600);

  SerialSetupLogger();
  PeripheralSetup();
  DrawTabs();
  GetSettings();
}

void loop() 
{
  //Serial.println(analogRead(A15));

  tft.setTextColor(pipData.ActiveColour, BLACK);

  int btnState1 = digitalRead(BTN_1_PIN); //locks the screen preventing the us from changing tab
  if (btnState1 != prevBtnState1) //checks to see if the btn has been pressed
  {
    if (btnState1 == HIGH)
      pipData.Locked = !pipData.Locked;
      //locked = !locked;
    
    prevBtnState1 = btnState1;
  }

  index = val1/incrementalStep;
  if (index > Tab::Total) //sets index = to the current value of the horozontal potentiometer
    index = Tab::Total;
  
  int btnState2 = digitalRead(BTN_2_PIN); //restarts the current module
  if (btnState2 == HIGH)
  {
    DrawMenuCursur(index, oldIndex);
    Tabs[index]->Setup();
  } 

  if (!pipData.Locked) //main routine for running through the tabs loop
  {
    val1 = analogRead(pipData.POT_1_PIN);

    if (index == oldIndex)
      Tabs[index]->Loop();
    else
    {
      DrawMenuCursur(index, oldIndex);
      Tabs[index]->Setup();
    }

    oldIndex = index;
    tft.setCursor(0, 302); 
    tft.setTextSize(2);
    tft.print(" ");
  }
  else
  {
    tft.setCursor(0, 302); 
    tft.setTextSize(2);
    tft.print("L");
    Tabs[index]->Loop();
  }

  if (pipData.ActiveColour != prevPipColour)
  {
    DrawTabs();
    DrawMenuCursur(index, oldIndex);
    Tabs[index]->Setup();
  }

  prevPipColour = pipData.ActiveColour;
}

