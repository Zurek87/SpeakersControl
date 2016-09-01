#include <ESP8266WebServer.h>
#include "Speakers.h"

class RichServer
{
  public:
    RichServer(int port);
    
    typedef std::function<void(void)> THandlerFunction;
    typedef std::function<void(String)> THandlerSourceFunction;
    typedef std::function<String(void)> THandlerGetFunction;
    typedef std::function<WeatherInfo(void)> THandlerGetWeather;
    typedef std::function<void(LedRGB)> THandlerRGBFunction;
    
    THandlerFunction powerOnHandler;
    THandlerFunction powerOffHandler;
    THandlerSourceFunction changeSourceHandler;
    THandlerRGBFunction setMainColorHandler;
    THandlerRGBFunction setBackColorHandler;
    THandlerGetWeather getWeatherHandler;
    
    void urlNotFound();
    void urlGetTemp();
    void urlGetRoot();
    void urlSetLedLight();
    void urlSetPower();
    void urlSetSource();
    void handleClient();
    
  private:
    ESP8266WebServer _espServer;
    
    void templateOk(String content="", String title="", int httpCode=200);
    void template404(String content="", String title="");
    String responseTemplate(String content="", String title="");

};


