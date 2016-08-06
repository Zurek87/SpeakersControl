#include "WeatherStation.h"

#define DHTTYPE DHT11
#define DHTPIN 13

DHT dht(DHTPIN, DHTTYPE);

WeatherStation::WeatherStation()
{
  
}


WeatherInfo WeatherStation::getSensorsReading()
{
  SensorInfo* sensorsInfo = new SensorInfo[2];
  
    sensorsInfo[0] = readTemp();
    sensorsInfo[1] = readHud();
  

  return {2, sensorsInfo};
}

SensorInfo WeatherStation::readTemp()
{
   float h = dht.readHumidity();
   Serial.println(dht.readTemperature());
   return {"Humidity", h, "%"};
}

SensorInfo WeatherStation::readHud()
{
   float t = dht.readTemperature();
   return {"Temperature", t, "'C"};
}

