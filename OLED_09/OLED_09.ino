#include <Wire.h>
#include <SSD1306Ascii.h>
#include <SSD1306AsciiWire.h>

SSD1306AsciiWire oled;

void setup() {
  Serial.begin(115200);
  Wire.begin();
  // Wire.setClock(400000L);  // Increase I2C speed to 400kHz
  oled.begin(&Adafruit128x32, 0x3C);  // Address 0x3C
  oled.setFont(System5x7);
  oled.clear();
  oled.println(F("Hello Nano!"));
}

void loop() {
    for(int i=0; i<100; i++){
      oled.setCursor(0, 0);
      oled.print(i);
      // oled.println("Hello Arduino Nano!" + String(i));
      // delay(1000);
    }
    Serial.println("Passed");
}
