#include "LedSupport.h"

#define LED_PIN_DEFAULT 13
#ifdef __AVR__
  #include <avr/power.h>
#endif
// Adafruit_NeoPixel can be only one instance (and only one create!!) in all sketch (problem with timeout interupt after sucessful set color)
Adafruit_NeoPixel ledStrip = Adafruit_NeoPixel(2, LED_PIN_DEFAULT, NEO_RGB + NEO_KHZ400);// NEO_KHZ400 is for WS2811

LedSupport::LedSupport()
{
  byte pin_size = 2;
  _ledPinInfo = {pin_size, new uint8_t[pin_size], new LedRGB[pin_size]};
  for (int i = 0; i < pin_size; i++){
    _ledPinInfo.rgb[i] = {0,0,0};
    _ledPinInfo.dimmer[i] = 255;
  }
}

void LedSupport::init()
{
  ledStrip.begin();
  ledStrip.show(); 
}

void LedSupport::setColor(uint16_t id, LedRGB rgb)
{
  rgb = updateColor(_ledPinInfo.dimmer[id], rgb);
  setLedColor(id, rgb);
}

void LedSupport::setLedColor(uint16_t id, LedRGB rgb)
{
  uint32_t color = ledStrip.Color(rgb.r, rgb.g, rgb.b);
  ledStrip.setPixelColor(id, color);
  ledStrip.show();
}


void LedSupport::setDimmer(uint8_t dimmer)
{
  for (int i = 0; i < _ledPinInfo.size; i++) {
    _ledPinInfo.dimmer[i] = dimmer;
    LedRGB rgb = updateColor(dimmer, _ledPinInfo.rgb[i]);
    setLedColor(i, rgb);
  }
}

void LedSupport::setDimmer(uint8_t dimmer, uint8_t id)
{
  _ledPinInfo.dimmer[id] = dimmer;
  LedRGB rgb = updateColor(dimmer, _ledPinInfo.rgb[id]);
  setLedColor(id, rgb);
}

LedRGB LedSupport::updateColor(uint8_t dimmer, LedRGB rgb)
{
  if (dimmer >= 255) {
    return rgb;
  }
  if (dimmer <= 0) {
     return {0, 0, 0};
  }
  uint8_t r = rgb.r * dimmer / 255;
  uint8_t g = rgb.g * dimmer / 255;
  uint8_t b = rgb.b * dimmer / 255;
  return {r, g, b};
}

