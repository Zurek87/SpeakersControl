#ifndef SPEAKERS
#define SPEAKERS True
#define SRD_PIN 16
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
  byte dimmer;
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
