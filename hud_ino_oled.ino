#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

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

#define buffLength 200

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
  }
}

void processCommand(char *cmd) {
  Serial.println(cmd);
  display.clearDisplay();
  
  char _copyBuff[buffLength];
  char _str[buffLength];
  strcpy(_copyBuff, cmd);

  char delim[] = ";";
  char *ptr = strtok(_copyBuff, delim);

  int _offset = 0;
  int _line = 0;

  while (ptr != NULL)
  {
    strcpy(_str, ptr);
    
    if (_offset == 0) {
      // Baris pertama
      display.setCursor(0, 0);     // Start at top-left corner
      display.println(_str);
    } else if (_offset == 1) {
      // Baris kedua
      display.setCursor(0, 16);     // Start at top-left corner
      display.println(_str);
    } else {
      // Gambar
      Serial.println(strlen(_str));

      for (int i=0;i<128;i++) {
        epd_bitmap_Frame_1[i] = 0b00000000;
      }

      int lastFilled = 0;
      byte x = 0b00000000;
      for (int i = 0; i<strlen(_str); i++) {
        if (_str[i] == '0') {
          bitWrite(x, 7 - (i % 8), 0);
        } else {
          bitWrite(x, 7 - (i % 8), 1);
        }

        if (i != 0 && i % 8 == 0) {
          epd_bitmap_Frame_1[lastFilled] = x;
          x = 0b00000000;
          lastFilled++;
        }
      }

      display.drawBitmap(
        (SCREEN_WIDTH  - LOGO_WIDTH ),
        0,
        epd_bitmap_Frame_1, LOGO_WIDTH, LOGO_HEIGHT, 1);
    }

    ptr = strtok(NULL, delim);
    _offset++;
    _line++;
  }

  display.display();
}
