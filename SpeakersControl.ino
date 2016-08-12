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
byte last_led_color = 255;
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

void setInfoColor(byte i)
{
  if (last_led_color == i) return;
  LedRGB colors[8] = {
    {0, 0, 0},       // 0 -> if "0" and powerOff
    {0, 0, 255},     // 1 -> Init in progress
    {255, 140, 0},   // 2 -> WiFi problem
    {255, 0, 0},     // 3 -> big error
    {255, 255, 255}, // 4 white, reserved for future idiea ;) 
    {45, 255, 215},  // 5 -> if "0" and powerOn and source "0" (PC)
    {25, 255, 80},   // 6 -> if "0" and powerOn and source "1" (RPi3/Phone)
    {255, 0, 255}    // 7 -> if powerOff and error (for blinking)
  };
  ledSupport.setColor(LED_PIN_MAIN, 0, colors[i]);
  last_led_color = i;
}

void setInfoLed()
{
  if (!power_on and system_status==0) {
    setInfoColor(0);
    return;
  }
  if (not (blink or power_on)) {
    setInfoColor(7);
  } else if (system_status == 0) {
    setInfoColor(5 + source);
  } else if ( system_status < 5){
    setInfoColor(system_status);
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
  last_led_color = 255;
  // restet wachdog,: give time to see message :)
  timer0_write(ESP.getCycleCount() + WACHDOG_INTERVAL);
}

void setBackColor(LedRGB rgb)
{
  ledSupport.setColor(LED_PIN_MAIN, 1, rgb);
}

void setLedColor(uint16_t pin, uint8_t id, LedRGB rgb)
{
  ledSupport.setColor(pin, id, rgb);
}

void changeSource(String src)
{
  if (src=="PC") {
    digitalWrite(SRD_SRC_PIN, HIGH);
    powerOn();
  } else if(src == "RPi") {
    digitalWrite(SRD_SRC_PIN, LOW);
    powerOn();
  }
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
  
  //ledSupport.setColor(LED_PIN_MAIN, 1, {255, 140, 0});
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

void initWachdog() 
{
  Serial.println("Init Wachdog");
  noInterrupts();
  timer0_isr_init();
  timer0_attachInterrupt(wachdogLed);
  timer0_write(ESP.getCycleCount() + WACHDOG_INTERVAL);
  interrupts();
}

void initRichServer()
{
  Serial.println("Init RichServer");
  serverRich.powerOffHandler = powerOff;
  serverRich.powerOnHandler = powerOn;
  serverRich.setMainColorHandler = setMainColor;
  serverRich.setBackColorHandler = setBackColor;
  serverRich.getWeatherHandler = getWeather;
  serverRich.changeSourceHandler = changeSource;
}

void initRelayModules()
{
  pinMode(SRD_SRC_PIN, OUTPUT);
  pinMode(SRD_POWER_PIN, OUTPUT);
  digitalWrite(SRD_SRC_PIN, HIGH);//select default source
  digitalWrite(SRD_POWER_PIN, HIGH);// power on !!
}

void setup(void)
{
  system_update_cpu_freq(SYS_CPU_160MHZ);
  initRelayModules();
  
  Serial.begin(115200);
  Serial.println("");
  Serial.println("Zurek ESP starting!");

  initWiFi();
  initLed();
  initRichServer();

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  initWachdog();
  system_status = 0;
  
  setInfoLed();
  Serial.println("Setup End");
}

void loop(void)
{
  serverRich.handleClient();
}



