// Build upon 302CEM_ArduinoWeatherStation project

#include <WiFi.h>
#include "time.h"
#include "MQ135.h"
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define BME_SCK 5       // SCK to SCK
#define BME_MISO 19     // SDO to MISO
#define BME_MOSI 18     // SDI to MOSI
#define BME_CS 32       // CS to Digital Pin 0
#define RED_LED 14
#define GREEN_LED 15
#define BLUE_LED 33
#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 64    // OLED display height, in pixels
#define OLED_RESET    32    // Reset pin
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address;
#define PIN_MQ135     A2

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET); // OLED software I2C
Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK);                 // Sensor software SPI

// Air condition variables
const unsigned offset = 1622;  // variable taken from the min-co2 calibration
int co2Level;

// WiFi details
#define WIFI_SSID ""
#define WIFI_PASSWORD ""
#define SECOND_WIFI_SSID ""
#define SECOND_WIFI_PASSWORD ""

// Time variables
const char* ntpServer = "pool.ntp.org";
const int gmtOffset_sec = 0;
struct tm timeinfo;
 
void setup(){

  // Define LEDs as output
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  pinMode(PIN_MQ135, INPUT);

  // Light up the LEDs
  digitalWrite(RED_LED, HIGH);
  digitalWrite(GREEN_LED, HIGH);
  digitalWrite(BLUE_LED, HIGH);

  // Init the serial communication
  Serial.begin(1000000);
  while(!Serial);     // Get the serial running
  delay(1000);        // Wait before accessing the sensor
  
  digitalWrite(BLUE_LED, LOW);
  digitalWrite(GREEN_LED, LOW);
  Serial.println(F("Hello World"));

  // Init the BME sensor
  unsigned status;
  status = bme.begin(0x76, &Wire);

  // Init the OLED
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);          // Don't proceed, loop forever
  }
  display.display();
  delay(500);         // Pause for 0.5 second
  display.clearDisplay();               // Clear the buffer
  display.setTextSize(1);               // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);  // Draw white text
  display.setCursor(0,0);               // Start at top-left corner

  // Draw a single pixel in white
  display.drawPixel(10, 10, SSD1306_WHITE);
  display.display();
  delay(500);
  display.clearDisplay();   // Clear the buffer
  
  // Init the WiFI
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  display.print("Connecting to Wi-Fi");
  display.display();
  int wait = 0;
  while (WiFi.status() != WL_CONNECTED) {
    if (wait == 15) {
      WiFi.begin(SECOND_WIFI_SSID, SECOND_WIFI_PASSWORD);
    } else if (wait > 30) {
      display.clearDisplay();                     // Clear the buffer
      display.setTextSize(1);                     // Normal 1:1 pixel scale
      display.setTextColor(SSD1306_WHITE);        // Draw white text
      display.setCursor(10,4);                     // Start at top-left corner
      display.print("Cannot find Wi-Fi");
      display.display();
      for(;;);
    }
    display.print(".");
    display.display();
    delay(333);
    wait++;
  }
  display.clearDisplay();   // Clear the buffer
  display.println();
  display.println("Connected with IP: ");
  display.println(WiFi.localIP());  // Display the system's IP
  display.display();

  // Init and get the time
  configTime(gmtOffset_sec, 0, ntpServer);
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  
  digitalWrite(RED_LED, LOW);
  digitalWrite(BLUE_LED, HIGH);
  display.println();
  display.print(&timeinfo, "%b %d %Y, %H:%M:%S");  // Display time
  display.display();
  delay(2000);
 
} // End "setup()"

void loop(){ // Start of Program
  printReadings();
  delay(1000);
}

void printReadings() {
  
  display.clearDisplay();                     // Clear the buffer
  display.setTextSize(1);                     // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE);        // Draw white text
  display.setCursor(0,4);                     // Start at top-left corner

  display.print("Temperature: ");
  display.print(bme.readTemperature(), 2);
  display.println(" C");

  display.print("Pressure: ");
  display.print(bme.readPressure() / 100.0F, 2);
  display.println(" hPa");

  display.print("Humidity: ");
  display.print(bme.readHumidity(), 2);
  display.println(" %");

  co2Level = map(analogRead(PIN_MQ135) - offset, 0, 1024, 400, 5000);
  display.print("co2 PPM: ");
  display.print(co2Level);
  display.println(" PPM");

  getLocalTime(&timeinfo);                    // Get time info from the ntp server
  display.println();
  display.print(&timeinfo, "%b %d %Y, %H:%M:%S");
  display.display();
  
  if (co2Level >= 1000) {
    digitalWrite(GREEN_LED, LOW);
    digitalWrite(RED_LED, HIGH);
    digitalWrite(BLUE_LED, HIGH);
  } else {
    digitalWrite(GREEN_LED, HIGH);
    digitalWrite(RED_LED, LOW);
    digitalWrite(BLUE_LED, HIGH);
  }
}
