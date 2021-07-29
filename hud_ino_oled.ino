#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "base64.hpp"

//#include <SoftwareSerial.h>
//SoftwareSerial BTSerial(10, 11); // RX | TX

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define buffLength 256

char buff[buffLength];
int buffOffset = 0;

const unsigned char glanceMapsIcon [] PROGMEM = {
  0x1f, 0xff, 0xff, 0xf8, 0x3f, 0xff, 0xff, 0xfc, 0x7f, 0xff, 0xff, 0xfe, 0xff, 0xff, 0xff, 0xff, 
  0xff, 0xff, 0xc7, 0xff, 0xf8, 0xef, 0xc3, 0xff, 0xf0, 0xc7, 0x83, 0xff, 0xf0, 0x83, 0x83, 0xff, 
  0xf1, 0x83, 0x01, 0xff, 0xe1, 0x03, 0x01, 0xff, 0xe1, 0x06, 0x00, 0xff, 0xc3, 0x06, 0x00, 0xff, 
  0xc2, 0x06, 0x00, 0x7f, 0xc6, 0x0c, 0x00, 0x7f, 0x84, 0x0c, 0x00, 0x7f, 0xc4, 0x18, 0x00, 0x3f, 
  0xec, 0x18, 0x00, 0x3f, 0xf8, 0x30, 0x00, 0x1f, 0xf8, 0x30, 0x00, 0x1f, 0xf8, 0x30, 0x00, 0x0f, 
  0xf8, 0x60, 0x00, 0x0f, 0xfd, 0xe0, 0x00, 0x0f, 0xff, 0xc0, 0x00, 0x07, 0xff, 0xc0, 0x00, 0x07, 
  0xff, 0x80, 0x00, 0x03, 0xff, 0x80, 0x00, 0x03, 0xff, 0x80, 0x7c, 0x03, 0xff, 0x83, 0xff, 0x83, 
  0xff, 0xdf, 0xff, 0xf7, 0x7f, 0xff, 0xff, 0xfe, 0x3f, 0xff, 0xff, 0xfc, 0x1f, 0xff, 0xff, 0xf8
};

void setup() {
  Serial.begin(9600); 
//  BTSerial.begin(9600);  // HC-05 default speed in AT command more
  
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

  Serial.println("STARTED");
}


long lastCmdMs = millis();
void loop(){
  if (millis() - lastCmdMs > 5000) {
    display.clearDisplay();
    lastCmdMs = millis();
    
    display.setTextSize(2);      // Normal 1:1 pixel scale
    display.setCursor(0, 0);
    display.println("Glance");
    
    display.setTextSize(1);      // Normal 1:1 pixel scale
    display.setCursor(0, 15);
    display.println("Maps");

    display.drawBitmap(
      128-32,
      0,
      glanceMapsIcon, 32, 32, 1);
    
    display.display();
  }

  if (Serial.available()) {
    String s1 = Serial.readStringUntil('\0');
    processCommand(s1);
  }
  
//  while (Serial.available()) {
//    char character  = Serial.read();
//
//    if (character == '\0' || character == '\n') {
//      buff[buffOffset+1] = '\0';
//      buffOffset = 0;
//
//      processCommand(String(buff));
//      break;
//    } else {
//      buff[buffOffset] = character;
//      buffOffset++;
//    }
//  }
}

void drawPerLine(String cmd, unsigned int line) {
  char *idk; // Karna nested strtok, perlu make strtok_r
  
  int l = cmd.length() + 1;
  char str[l];
  cmd.toCharArray(str,l);
  
  char delim[] = ",";
  char *ptr = strtok_r(str, delim, &idk);
  
  // Nilai pertama
  int _start = String(ptr).toInt();

  // Nilai kedua
  ptr = strtok_r(NULL, delim, &idk);
  int _end = String(ptr).toInt();

  // Warna
  ptr = strtok_r(NULL, delim, &idk);
  String _warna = String(ptr);
//  
//  Serial.print("ENTAH ");
//  Serial.print(_start);
//  Serial.print(" ");
//  Serial.print(_end);
//  Serial.print(" ");
//  Serial.println(_warna);

  if (_warna == "w") {
    display.drawLine(128-32 + _start, line, 128-32 + _end, line, SSD1306_WHITE);
  } else {
    display.drawLine(128-32 + _start, line, 128-32 + _end, line, SSD1306_BLACK);
  }
}

void processCommand(String cmd) {
//  Serial.println(cmd);
  lastCmdMs = millis();

  if (cmd[0] == 'a') {
    display.setTextSize(2);      // Normal 1:1 pixel scale
    display.fillRect(0,0,96,15,SSD1306_BLACK); // Untuk ngeclear
    display.setCursor(0, 0);
    display.println(cmd.substring(1));
    display.display();
  }
  
  if (cmd[0] == 'b') {
    display.setTextSize(1);      // Normal 1:1 pixel scale
    display.fillRect(0,15,96,10,SSD1306_BLACK); // Untuk ngeclear
    display.setCursor(0, 15);
    display.println(cmd.substring(1));
    display.display();
  }
  
  if (cmd[0] == 'd') {
    display.setTextSize(1);      // Normal 1:1 pixel scale
    display.fillRect(0,24,96,8,SSD1306_BLACK); // Untuk ngeclear
    display.setCursor(0, 24);
    display.println(cmd.substring(1));
    display.display();
  }

  if (cmd[0] == 'c') {
    int _fromIn = cmd.indexOf(':') - 1;
    String _offStr = cmd.substring(1, _fromIn + 1);
    int _from = _offStr.toInt();

    // Ini untuk ngerequest line selanjutnya, DIPERLUKAN
    Serial.println("c" + _offStr + ":");
    
    String renderCmd = cmd.substring(_fromIn+2);
    int renderCmdLen = renderCmd.length() + 1;

    char renderChar[renderCmdLen];
    renderCmd.toCharArray(renderChar, renderCmdLen);
    
//    Serial.println(renderChar);

    char delim[] = " ";
    
    char *ptr = strtok(renderChar, delim);
    
    while(ptr != NULL)
    {
//      Serial.println(ptr);
      drawPerLine(String(ptr), _from);
      ptr = strtok(NULL, delim);
    }

    display.display();
  }
}
