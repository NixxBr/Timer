/*********************************************************************
This is an example sketch for our Monochrome Nokia 5110 LCD Displays

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/products/338

These displays use SPI to communicate, 4 or 5 pins are required to
interface

Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

// Software SPI (slower updates, more flexible pin options):
// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);

// Hardware SPI (faster, but must use certain hardware pins):
// SCK is LCD serial clock (SCLK) - this is pin 13 on Arduino Uno
// MOSI is LCD DIN - this is pin 11 on an Arduino Uno
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
// Adafruit_PCD8544 display = Adafruit_PCD8544(5, 4, 3);
// Note with hardware SPI MISO and SS pins aren't used but will still be read
// and written to during SPI transfer.  Be careful sharing these pins!

#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

// switches
#define GoBtn 12
#define ShotBtn 11

int go = 0;
int shot = 0;

void setup()   {
  Serial.begin(9600);

  pinMode(GoBtn, INPUT);
  pinMode(ShotBtn, INPUT);
  
  



  display.begin();
  // init done

  // you can change the contrast around to adapt the display
  // for the best viewing!

  display.setContrast(50);

  //display.display(); // show splashscreen
  //delay(2000);
  display.clearDisplay();   // clears the screen and buffer

 // test timer Layout
 

  // text display tests
  display.setTextSize(1);
  display.setTextColor(BLACK);
  display.setCursor(0,0);
  display.print("#        Split");
  display.print("12     1:12.23");
  display.println("");
  display.println("ELAPSED ");
  
  display.setTextSize(2);
  display.setTextColor(BLACK);
  display.println("1:23.05");
  display.display();
  
  // draw a bitmap icon and 'animate' movement
  //testdrawbitmap(logo16_glcd_bmp, LOGO16_GLCD_WIDTH, LOGO16_GLCD_HEIGHT);
  
  
}


void loop() {
  delay(1000);
  go =  digitalRead(GoBtn);
  if(go > 0)
  {
    Serial.println("GoBtn 1");
  }
  else
  {
    Serial.println("GoBtn 0");
  }
  shot = digitalRead(ShotBtn);
  if(shot > 0)
  {
    Serial.println("ShotBtn 1");
  }
  else
  {
    Serial.println("ShotBtn 0");
  }
  
}



