#include <Adafruit_NeoPixel.h>
#include "Speakers.h"

class LedSupport
{
  public:
    LedSupport();
    void init();
    void setColor(uint16_t id, LedRGB rgb);
    void setDimmer(uint8_t dimmer);
    void setDimmer(uint8_t dimmer, uint8_t id);
    
 // private:
    void setLedColor(uint16_t id, LedRGB rgb);
    void setLedColor(Adafruit_NeoPixel strip, uint16_t id, LedRGB rgb);
    LedRGB updateColor(uint8_t dimmer, LedRGB rgb);
   
    LedPin _ledPinInfo;

};

