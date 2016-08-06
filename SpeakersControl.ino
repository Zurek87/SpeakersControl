#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266mDNS.h>
#ifdef ESP8266
  extern "C" {
    #include "user_interface.h"
  }
#endif

#include "mySettings.h"
/*
 * Sample mySettings.h file:
 * 
 * #define WIFI_SSID "ssid"
 * #define WIFI_PASSWORD "password"
 * #define LED_PIN_MAIN 14
 * #define IP_ADDR (192,168,0,128); 
 * #define IP_GATEWAY (192,168,0,1);
 * #define IP_SUBNET (255,255,255,0);
 */
#include "WeatherStation.h"
#include "RichServer.h"

RichServer serverRich(80);
// bug on esp-12e ???
// need create server before create Adafruit_NeoPixel! -> server no handle clients!

/*  -------  Led Suport  -------  */

#include "LedSupport.h"
LedSupport ledSupport;

/*  -------  Other  -------  */
WeatherStation weatherStation;

void setColor(String action, LedRGB rgb = {0,0,0})
{
  uint16_t pin = LED_PIN_MAIN;
  uint8_t id = 0;
  if (action == "ERROR") {
    pin = LED_PIN_MAIN;
    rgb = {200,0,0};
  }
  setLedColor(pin, id, rgb);
}

void setLedColor(uint16_t pin, uint8_t id, LedRGB rgb)
{
  ledSupport.setColor(pin, id, rgb);
}

WeatherInfo getWeather()
{
  return weatherStation.getSensorsReading();
}


void setup(void){
  system_update_cpu_freq(SYS_CPU_160MHZ);
  //pinMode(SRD_PIN, OUTPUT);
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Zurek ESP starting!");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  WiFi.config(ip, gateway, subnet);


  Serial.println("Init LedSupport");
  ledSupport.init();
  ledSupport.addLedPin(LED_PIN_MAIN, 1);
  ledSupport.addLedPin(13, 1);
  Serial.println("Set Init Collor");
  ledSupport.setColor(LED_PIN_MAIN, 0, {137, 49, 159});
  
  Serial.println("Init RichServer");
  serverRich.setColorHandler = setColor;
  serverRich.getWeatherHandler = getWeather;

  Serial.println("Wait for connection");

  while (WiFi.status() != WL_CONNECTED) {
    delay(50);
    Serial.print(".");
  }

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  Serial.println("Setup End");
  ledSupport.setColor(LED_PIN_MAIN, 0, {25, 107, 100});

  pinMode(0, INPUT);
  //attachInterrupt(0, green, CHANGE);
  //timer0_attachInterrupt(green);
//  noInterrupts();
//  timer0_isr_init();
//  timer0_attachInterrupt(green_a);
//  timer0_write(ESP.getCycleCount() + 141660000);
//  interrupts();
}

void loop(void){
  serverRich.handleClient();
}



