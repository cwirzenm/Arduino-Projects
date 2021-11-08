// Credits to Mona Samsam. Source: https://circuitdigest.com/microcontroller-projects/interfacing-mq135-gas-sensor-with-arduino-to-measure-co2-levels-in-ppm

#include "MQ135.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define OLED_RESET    32    // Reset pin
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); // OLED software I2C

void setup () {
  Serial.begin(9600);
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);          // Don't proceed, loop forever
  }
  display.display();
  delay(500);         // Pause for 0.5 second
  display.clearDisplay();               // Clear the buffer
  display.setTextSize(2);               // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.setCursor(0,4);

  // Draw a single pixel in white
  display.drawPixel(10, 10, SSD1306_WHITE);
  display.display();
  delay(500);
  display.clearDisplay();   // Clear the buffer
}

void loop() {
  display.clearDisplay();                     // Clear the buffer
  display.setTextSize(2);                     // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(10,24);
  
  MQ135 gasSensor = MQ135(25); 
  float rzero = gasSensor.getRZero();
  display.print(rzero);
  display.println(" PPM");
  display.display();
  delay(1000);
}
