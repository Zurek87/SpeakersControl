#include <Adafruit_NeoPixel.h>
#include "Speakers.h"

class LedSupport
{
  public:
    LedSupport(byte max_pins = 2);
    void init();
    void addLedPin(uint16_t pin, byte pin_size);
    void setColor(uint16_t pin, uint16_t id, LedRGB rgb);
    void setDimmer(uint8_t dimmer);
    void setDimmer(uint8_t dimmer, uint16_t pin);
    void setDimmer(uint8_t dimmer, uint16_t pin, uint8_t id);
    
 // private:
    void setLedColor(int pin_id, uint16_t id, LedRGB rgb);
    void setLedColor(Adafruit_NeoPixel strip, uint16_t id, LedRGB rgb);
    void setPin(int pin_id);
    LedRGB updateColor(uint8_t dimmer, LedRGB rgb);
    int pinId(uint16_t pin);
    
    LedPin* _ledPinsInfo;
    uint16_t* _ledPins;
    uint16_t _selected_pin;
    
    byte _pin_count = 0;
};
