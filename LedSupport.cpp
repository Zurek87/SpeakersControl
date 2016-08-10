#include "LedSupport.h"

#define LED_PIN_DEFAULT 13

// Adafruit_NeoPixel can be only one instance (and only one create!!) in all sketch (problem with timeout interupt after sucessful set color)
Adafruit_NeoPixel ledStrip = Adafruit_NeoPixel(2, LED_PIN_DEFAULT, NEO_RGB + NEO_KHZ400);// NEO_KHZ400 is for WS2811

LedSupport::LedSupport(byte max_pins)
{
  _ledPinsInfo = new LedPin[max_pins];
  _ledPins = new uint16_t[max_pins];
  _selected_pin = LED_PIN_DEFAULT;
}

void LedSupport::init()
{
  ledStrip.begin();
  ledStrip.show(); 
}

void LedSupport::addLedPin(uint16_t pin, byte pin_size)
{
  LedPin pinInfo = {pin, pin_size, new uint8_t[pin_size], new LedRGB[pin_size]};
  for (int i = 0; i < pin_size; i++){
    pinInfo.rgb[i] = {0,0,0};
    pinInfo.dimmer[i] = 255;
  }
  
  
  int id = _pin_count;
  _ledPins[id] = pin;
  _ledPinsInfo[id] = pinInfo;
  _pin_count++;

}

void LedSupport::setColor(uint16_t pin, uint16_t id, LedRGB rgb)
{
  int pin_id = pinId(pin);
  if (pin_id >= 0) {
    rgb = updateColor(_ledPinsInfo[pin_id].dimmer[id], rgb);

    setLedColor(pin_id, id, rgb);
  }
}

void LedSupport::setLedColor(int pin_id, uint16_t id, LedRGB rgb)
{
  setPin(pin_id);
  uint32_t color = ledStrip.Color(rgb.r, rgb.g, rgb.b);
  ledStrip.setPixelColor(id, color);
  ledStrip.show();
}

int LedSupport::pinId(uint16_t pin)
{
  for (int i = 0; i < _pin_count; i++) {
    if (_ledPins[i] == pin) {
      return i;
    }
  }
  return -1;
}

void LedSupport::setPin(int pin_id)
{
  uint8_t new_pin = _ledPins[pin_id];
  
  if(_selected_pin != new_pin) {
    //ledStrip.setPin(new_pin);
    _selected_pin = new_pin;
  }
}

void LedSupport::setDimmer(uint8_t dimmer)
{
  for (int pin_id = 0; pin_id < _pin_count; pin_id++) {
    for (int i = 0; i < _ledPinsInfo[pin_id].size; i++) {
      _ledPinsInfo[pin_id].dimmer[i] = dimmer;
      LedRGB rgb = updateColor(dimmer, _ledPinsInfo[pin_id].rgb[i]);
      setLedColor(pin_id, i, rgb);
    }
  }
}

void LedSupport::setDimmer(uint8_t dimmer, uint16_t pin)
{
  int pin_id = pinId(pin);
  if (pin_id >= 0) {
    for (int i = 0; i < _ledPinsInfo[pin_id].size; i++) {
      _ledPinsInfo[pin_id].dimmer[i] = dimmer;
      LedRGB rgb = updateColor(dimmer, _ledPinsInfo[pin_id].rgb[i]);
      setLedColor(pin_id, i, rgb);
    }
  }
}

void LedSupport::setDimmer(uint8_t dimmer, uint16_t pin, uint8_t id)
{
  int pin_id = pinId(pin);
  if (pin_id >= 0) {
    _ledPinsInfo[pin_id].dimmer[id] = dimmer;
    LedRGB rgb = updateColor(dimmer, _ledPinsInfo[pin_id].rgb[id]);
    setLedColor(pin_id, id, rgb);
  }
}

LedRGB LedSupport::updateColor(uint8_t dimmer, LedRGB rgb)
{
  Serial.print("Dimmer is: ");
  Serial.println(dimmer);
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

