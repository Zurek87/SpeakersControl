#include <DHT.h>
#include "Speakers.h"


//struct SensorInfo
//{
//  String name;
//  float value;
//  String unit;
//}
//
//struct WeatherInfo
//{
//  int size;
//  SensorInfo* sensorsInfo;
//}

class WeatherStation
{
  public:
    WeatherStation();
    
    WeatherInfo getSensorsReading();
    
  private:
    DHT* _dht;
    SensorInfo readTemp();
    SensorInfo readHud();
};


