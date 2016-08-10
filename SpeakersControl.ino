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


#define WACHDOG_INTERVAL 160000000 * 5 // cpu speed ;)
byte system_status = 1;
byte error_count = 0;
byte source = 0;
bool power_on = true;
/*
 * 0 -> normal no errors
 * 1 -> connection/setup in progress
 * 2 -> wi-fi problem
 * 3 -> other error
 */


bool blink = false;

RichServer serverRich(80);
// bug on esp-12e ???
// need create server before create Adafruit_NeoPixel! -> server no handle clients!

/*  -------  Led Suport  -------  */

#include "LedSupport.h"
LedSupport ledSupport;

/*  -------  Other  -------  */
WeatherStation weatherStation;

void checkSystem()
{
  uint8_t status = WiFi.status();
  if (status == WL_CONNECTED) {
    system_status = 0;
    error_count = 0;
  } else {
    system_status = 2;
    Serial.print("WiFi Error :");
    Serial.println(status);
    reconnectWiFi();
    error_count++;
    if (error_count > 250) error_count = 5; //for 5+ start power if is off!
    blink = !blink;
  }
 
}

void setInfoLed()
{
  if (!power_on and system_status==0) {
    ledSupport.setColor(LED_PIN_MAIN, 0, {0, 0, 0});
    return;
  }
  if (not (blink or power_on)) {
    ledSupport.setColor(LED_PIN_MAIN, 0, {255, 0, 255});
  } else {
    switch (system_status) {
      case 0:
        if (source == 0) {
          ledSupport.setColor(LED_PIN_MAIN, 0, {45, 255, 215});
        } else {
          ledSupport.setColor(LED_PIN_MAIN, 0, {25, 255, 80});
        }
        break;
      case 1:
        ledSupport.setColor(LED_PIN_MAIN, 0, {0, 0, 255});
        break;
      case 2:
        ledSupport.setColor(LED_PIN_MAIN, 0, {255, 140, 0});
        break;
      case 3:
        ledSupport.setColor(LED_PIN_MAIN, 0, {255, 0, 0});
        break;
      default:
        ledSupport.setColor(LED_PIN_MAIN, 0, {255, 255, 255});
    }
  }
  
}

void powerOff()
{
  power_on = false;
  digitalWrite(SRD_POWER_PIN, LOW);
  setInfoLed();
}

void powerOn()
{
  power_on = true;
  digitalWrite(SRD_POWER_PIN, HIGH);
  setInfoLed();
}

void wachdogLed()
{
  checkSystem();
  setInfoLed();
  if ((!power_on) and (error_count >= 5)) powerOn();
  timer0_write(ESP.getCycleCount() + WACHDOG_INTERVAL);
}

void setMainColor(LedRGB rgb)
{
  setLedColor(LED_PIN_MAIN, 0, rgb);
  // restet wachdog,: give time to see message :)
  timer0_write(ESP.getCycleCount() + WACHDOG_INTERVAL);
}

void setBackColor(LedRGB rgb)
{
  Serial.println("Back Light");
  //ledSupport.setColor(LED_PIN_MAIN, 0, {255, 140, 0});
  ledSupport.setColor(LED_PIN_MAIN, 1, rgb);
  //setLedColor(LED_PIN_MAIN, 1, rgb);
}

void setLedColor(uint16_t pin, uint8_t id, LedRGB rgb)
{
  ledSupport.setColor(pin, id, rgb);
}

WeatherInfo getWeather()
{
  return weatherStation.getSensorsReading();
}

void initLed()
{
  Serial.println("Init LedSupport");
  ledSupport.init();
  ledSupport.addLedPin(LED_PIN_MAIN, 2);
  Serial.println("Set Init Collor");
  ledSupport.setColor(LED_PIN_MAIN, 1, {255, 140, 0});
  ledSupport.setDimmer(20, LED_PIN_MAIN, 0);
  setInfoLed();
  Serial.println("LedSupport Initialized");
}

void initWiFi() 
{
  Serial.print("Connecting to: ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  WiFi.config(ip, gateway, subnet);
  Serial.println("Wait for connection");

  while (WiFi.status() != WL_CONNECTED) {
    delay(50);
    Serial.print(".");
  }
  Serial.println("Connected !");
}

void reconnectWiFi()
{
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  WiFi.config(ip, gateway, subnet);
}

void setup(void){
  system_update_cpu_freq(SYS_CPU_160MHZ);
  pinMode(SRD_SRC_PIN, OUTPUT);
  pinMode(SRD_POWER_PIN, OUTPUT);
  digitalWrite(SRD_SRC_PIN, HIGH);//select default source
  digitalWrite(SRD_POWER_PIN, HIGH);// power on !!
  
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Zurek ESP starting!");

  initWiFi();

  initLed();
  
  Serial.println("Init RichServer");
  serverRich.setMainColorHandler = setMainColor;
  serverRich.setBackColorHandler = setBackColor;
  serverRich.getWeatherHandler = getWeather;


  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  Serial.println("Setup End");
  ledSupport.setColor(LED_PIN_MAIN, 0, {25, 107, 100});

  pinMode(0, INPUT);
  //attachInterrupt(0, green, CHANGE);
  //timer0_attachInterrupt(green);
  noInterrupts();
  timer0_isr_init();
  timer0_attachInterrupt(wachdogLed);
  timer0_write(ESP.getCycleCount() + WACHDOG_INTERVAL);
  interrupts();
  system_status = 0;
  setInfoLed();
}

void loop(void){
  serverRich.handleClient();
}



