#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "base64.hpp"

//#include <SoftwareSerial.h>
//SoftwareSerial BTSerial(10, 11); // RX | TX

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define buffLength 512

char buff[buffLength];
int buffOffset = 0;

void setup() {
  Serial.begin(9600); 
//  BTSerial.begin(9600);  // HC-05 default speed in AT command more
  
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.display();
  //display.cp437(true);         // Use full 256 char 'Code Page 437' font
  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text

  Serial.println("STARTED");
}

void loop(){
  while (Serial.available()) {
    char character  = Serial.read();

    if (character == '\0') {
      buff[buffOffset] = '\0';
      buffOffset = 0;

      processCommand(buff);
      break;
    } else {
      buff[buffOffset] = character;
    }

    buffOffset++;
    if (buffOffset >= buffLength) {
      Serial.println("Running out of buffer");
    }
  }
}

void processCommand(char *_cmd) {
  String cmd(_cmd);
  Serial.println(cmd);

  if (cmd[0] == 'a') {
//    display.clearDisplay();
    display.setTextSize(2);      // Normal 1:1 pixel scale
    display.fillRect(0,0,96,32,SSD1306_BLACK);
    display.setCursor(0, 0);
    display.println(cmd.substring(1));
    display.display();
  }
  
  if (cmd[0] == 'b') {
    display.setTextSize(1);      // Normal 1:1 pixel scale
    display.setCursor(0, 17);
    display.println(cmd.substring(1));
    display.display();
  }

  if (cmd[0] == 'c') {
    int _fromIn = cmd.indexOf(':') - 1;
    String _offStr = cmd.substring(1, _fromIn + 1);
    int _from = _offStr.toInt();

    Serial.println("c" + _offStr + ":");
//    BTSerial.println("c" + _offStr + ":");
    
    for (int i = 0;i<32;i++) {
      char v = cmd[i+_fromIn+1];
      Serial.println(i + "-" + v);
      
      if (v == '1') {
        display.drawPixel(display.width() - 34 + i, _from, SSD1306_WHITE);
      } else {
         display.drawPixel(display.width() - 34 + i, _from, SSD1306_BLACK);
      }
    }
    
    display.display();
  }
}
