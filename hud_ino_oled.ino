#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "base64.hpp"

#include <SoftwareSerial.h>
SoftwareSerial BTSerial(10, 11); // RX | TX

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

#define LOGO_HEIGHT   32
#define LOGO_WIDTH    32
unsigned char epd_bitmap_Frame_1 [] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

char buff[buffLength];
int buffOffset = 0;

void setup() {
  Serial.begin(9600); 
  BTSerial.begin(9600);  // HC-05 default speed in AT command more
  
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.display();
  display.cp437(true);         // Use full 256 char 'Code Page 437' font
  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text

  BTSerial.println("STARTED");
}

void loop(){
  while (BTSerial.available()) {
    char character  = BTSerial.read();

    if (character == '\n') {
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

void resetImgBuff() {
  for (int i = 0; i<128;i++) {
    epd_bitmap_Frame_1[i] = 0x00;
  }
}

void processCommand(char *_cmd) {
  String cmd(_cmd);
  Serial.println(cmd);

  if (cmd[0] == 'a') {
//    lastPx = 0;
    resetImgBuff();
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println(cmd.substring(1));
    display.display();
  }
  
  if (cmd[0] == 'b') {
    display.setCursor(0, 17);
    display.println(cmd.substring(1));
    display.display();
  }

  //  Baris 1 :: c1
  if (cmd[0] == 'c') {
    Serial.println(cmd);

    int _fromIn = cmd.indexOf(',') - 1;
    String _offStr = cmd.substring(1, _fromIn + 1);
    Serial.println("mmm " + _offStr);
    int _from = _offStr.toInt();
    
    int _untilIn = cmd.indexOf(':') - 1;
    String _untilStr = cmd.substring(_fromIn + 2, _untilIn + 1);
    Serial.println("mmm2 " + _untilStr);
    int _until = _untilStr.toInt();

    int _lastOne = _until + 1;
    int _toFill = _from;
    String _b = "";
    for (int i = _untilIn + 1; i<cmd.length(); i++) {
      if (cmd[i] == ' ') {
        Serial.println("lll:" + _b);
//        _lastOne = i + 1;
        epd_bitmap_Frame_1[_toFill] = _b.toInt();
        _b = "";
        _toFill++;
      } else {
        _b += cmd[i];
      }
    }

    Serial.println("c" + _offStr + "," + _untilStr + ":");
    BTSerial.println("c" + _offStr + "," + _untilStr + ":");
    
    display.display();
  }
}
