#ifndef SPEAKERS
#define SPEAKERS True
#define SRD_SRC_PIN 12
#define SRD_POWER_PIN 15
#define LED_PIN_MAIN 13

struct LedRGB 
{
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

struct LedPin 
{
  uint16_t pin;
  byte size;
  uint8_t* dimmer;
  //Adafruit_NeoPixel strip;
  LedRGB* rgb;
}; 



struct SensorInfo
{
  String name;
  float value;
  String unit;
};

struct WeatherInfo
{
  int size;
  SensorInfo* sensorsInfo;
};
#endif
