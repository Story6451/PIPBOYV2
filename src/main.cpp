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

uint32_t Tab::Total = -1;

//encoder variables
uint8_t currentACLK;
uint8_t lastACLK;
uint32_t prevEncoderAValue = 0;
uint8_t currentBCLK;
uint8_t lastBCLK;
uint32_t prevEncoderBValue = 0;

bool increment = false;
bool decrement = false;

//button pins
const uint32_t REFRESH_BTN_PIN = 19;//refreshing button
const uint32_t LOCK_BTN_PIN = 18;//locking button

//led pins 
const uint32_t LED_1_PIN = 6; 
const uint32_t LED_2_PIN = 7;
const uint32_t LED_3_PIN = 8;

int32_t val1 = 0;
int32_t val2 = 0;

bool locked = false;
int32_t prevBtnState = LOW;
int32_t oldIndex = 0;
int32_t index = 0;

int32_t prevPipColour;
int8_t prevTextSize;
int8_t prevOrientation;
int8_t prevLightStatus;

Sd2Card card;
SdVolume volume;

const uint32_t chipSelect = 53;

void SerialSetupLogger() //logs tft info to the serial monitor
{
  Serial.println("Hello, world!");

  tft.reset();
  if (tft.readID() == 0x8357)
  {
    Serial.println("Found LCD");
  }
  else 
  {
    Serial.print("Could not find LCD Instead found display with identifier: "); Serial.println(tft.readID());
  }
  
  //screen information
  Serial.print("Identifier: "); Serial.println(tft.readID());
  Serial.print("Height: "); Serial.println(tft.height());
  Serial.print("Width: ");Serial.println(tft.width());

}

void PeripheralSetup() //initialises the external hardware and logs it to the tft and console
{
  //tft setup
  delay(100);
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
    tft.println("SD card test 1 success");
    Serial.println("SD card test 1 success");
  }
  else
  {
    tft.println("SD card test 1 failed");
    Serial.println("SD card test 1 failed");
  }

  if (SD.begin(chipSelect))
  {
    tft.println("SD card test 2 success");
    Serial.println("SD card test 2 success");
  }
  else
  {
    tft.println("SD card test 2 failed");
    Serial.println("SD card test 2 failed");
  }

  //digital encoders setup
  pinMode(pipData.ENCODER_A_CLK, INPUT);
  pinMode(pipData.ENCODER_A_DT, INPUT);
  pinMode(pipData.ENCODER_B_CLK, INPUT);
  pinMode(pipData.ENCODER_B_DT, INPUT);
  lastACLK = digitalRead(pipData.ENCODER_A_CLK);
  lastBCLK = digitalRead(pipData.ENCODER_B_CLK);
  tft.println("Encoder A connected");
  Serial.println("Encoder A connected");
  tft.println("Encoder B connected");
  Serial.println("Encoder B connected");

  //Button inputs
  pinMode(REFRESH_BTN_PIN, INPUT); 
  pinMode(LOCK_BTN_PIN, INPUT);
  //pinMode(pipData.BTN_3_PIN, INPUT);
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
    
    String datas[6];
    if (file)
    {
      Serial.println("file available");
      int count = 0;
      while ((file.available()) && (count < 6))
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
    for (int i = 0; i < 6; i++)
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
    pipData.TextSize = datas[3].toInt();
    pipData.Orientation = datas[4].toInt();
    pipData.LightStatus = datas[5].toInt();
  }
  else
  {
    File file = SD.open("config.txt");
    file.close();
  }
}

void ISR_refresh_button()
{
  int btnState = digitalRead(REFRESH_BTN_PIN); //restarts the current module
  if (btnState == HIGH)
  {
    DrawMenuCursur(index, oldIndex);
    Tabs[index]->Setup();
  } 
}

void ISR_lock_button()
{
  int btnState = digitalRead(LOCK_BTN_PIN); //locks the screen preventing the us from changing tab
  if (btnState != prevBtnState) //checks to see if the btn has been pressed
  {
    if (btnState == HIGH)
    {
      pipData.Locked = !pipData.Locked;
    }
    
    prevBtnState = btnState;
  }
  increment = false;
  decrement = false;
}

void ISR_encoder_a()
{
  currentACLK = digitalRead(pipData.ENCODER_A_CLK);
  if ((currentACLK != lastACLK) && (currentACLK == 1))
  {
    if (digitalRead(pipData.ENCODER_A_DT) != currentACLK)
    {
      pipData.encoderAValue--;
      decrement = true;
    }
    else
    {
      pipData.encoderAValue++;
      increment = true;
    }
  }
  lastACLK = currentACLK;
}

void ISR_encoder_b()
{
  currentBCLK = digitalRead(pipData.ENCODER_B_CLK);
  if ((currentBCLK != lastBCLK) && (currentBCLK == 1))
  {
    if (digitalRead(pipData.ENCODER_B_DT) != currentBCLK)
    {
      pipData.encoderBValue--;
    }
    else
    {
      pipData.encoderBValue++;
    }
  }
  lastBCLK = currentBCLK;
}

void setup() 
{
  //serial setup
  Serial.begin(9600);

  SerialSetupLogger();
  PeripheralSetup();
  GetSettings();
  DrawTabs();
  tft.setRotation(pipData.Orientation);
  attachInterrupt(digitalPinToInterrupt(REFRESH_BTN_PIN), ISR_refresh_button, RISING);
  attachInterrupt(digitalPinToInterrupt(LOCK_BTN_PIN), ISR_lock_button, RISING);
  attachInterrupt(digitalPinToInterrupt(pipData.ENCODER_A_DT), ISR_encoder_a, CHANGE);
  attachInterrupt(digitalPinToInterrupt(pipData.ENCODER_B_DT), ISR_encoder_b, CHANGE);
}


void loop() 
{
  tft.setTextColor(pipData.ActiveColour, BLACK);

  if (pipData.Locked == false) //main routine for running through the tabs loop
  {

    //Serial.print("  encoder value: "); Serial.print(pipData.encoderAValue); Serial.print("   ");
    //Serial.print("  prev encoder value: "); Serial.print(prevEncoderAValue); Serial.print("   ");
    //Serial.print("  Index: "); Serial.print(index); Serial.println("   ");
    if (increment == true)
    {
      index++;
      increment = false;
    }
    else if (decrement == true)
    {
      index--;
      decrement = false;
    }
    if (index > (int32_t)Tab::Total) //sets index = to the current value of the horozontal potentiometer
    {
      index = Tab::Total;
    }
    if (index < 0)
    {
      index = 0;
    }

    if (index == oldIndex)
    {
      Tabs[index]->Loop();
    }
    else
    {
      DrawMenuCursur(index, oldIndex);
      Tabs[index]->Setup();
    }

    oldIndex = index;
    //tft.setCursor(0, 302); 
    //tft.setTextSize(2);
    //tft.print(" ");
    tft.drawRect(0, 61, 480, 259, BLACK);
  }
  else
  {
    //tft.setCursor(0, 302); 
    //tft.setTextSize(2);
    //tft.print("L");
    tft.drawRect(0, 61, 480, 259, pipData.ActiveColour);
    Tabs[index]->Loop();
  }

  if ((pipData.ActiveColour != prevPipColour) || (pipData.TextSize != prevTextSize) || (pipData.Orientation != prevOrientation))
  {
    tft.fillRect(0, 0, 480, 320, BLACK);
    DrawTabs();
    DrawMenuCursur(index, oldIndex);
    Tabs[index]->Setup();
  }

  if (pipData.LightStatus != prevLightStatus)
  {
    if (pipData.LightStatus == 1)
    {
      digitalWrite(LED_1_PIN, HIGH);
      digitalWrite(LED_2_PIN, HIGH);
      digitalWrite(LED_3_PIN, HIGH);
    }
    else
    {
      digitalWrite(LED_1_PIN, LOW);
      digitalWrite(LED_2_PIN, LOW);
      digitalWrite(LED_3_PIN, LOW);
    }
  }

  prevLightStatus = pipData.LightStatus;
  prevPipColour = pipData.ActiveColour;
  prevTextSize = pipData.TextSize;
  prevOrientation = pipData.Orientation;
  prevEncoderAValue = pipData.encoderAValue;
}

