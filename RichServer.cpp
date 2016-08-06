/*
 * Small class for pack response to user in HTML5 template
 */
#include "RichServer.h"


RichServer *richServerObj;

void handleNotFound()
{
  
  richServerObj->urlNotFound();
}

void handleGetTemp()
{
  richServerObj->urlGetTemp();
}

void handleRoot()
{
  
}

RichServer::RichServer(int port)
{
  richServerObj = this;
  _espServer = ESP8266WebServer(port);

  _espServer.onNotFound(handleNotFound);
  _espServer.on("/temp", handleGetTemp);
  
  _espServer.begin();
}

void RichServer::handleClient()
{
  _espServer.handleClient();
}

void RichServer::urlGetRoot()
{
  template404();
}

void RichServer::urlNotFound()
{
  template404();
}

void RichServer::urlGetTemp()
{
  if(getWeatherHandler != NULL) {
    WeatherInfo weather_info = getWeatherHandler();
    String weather = "<table><tbody>";
    Serial.println(weather);
    Serial.println(weather_info.size);
    for (int i = 0; i <  weather_info.size; i++) {
       SensorInfo info = weather_info.sensorsInfo[i];
       Serial.println("info.name");
       Serial.println(info.name);
       Serial.println(info.value);
       Serial.println(info.unit);
       weather += "<tr>";
       weather += "<td>" + info.name + "</td><td>";
       weather += info.value;
       weather += " " + info.unit + "</td>";
       weather += "</tr>";
    }
    weather += "</tbody></table>";
    templateOk(weather, "Climate");
  } else {
    template404("No sensors sets", "No Sensors!");
  }
}

String RichServer::responseTemplate(String content, String title)
{
  // @todo add css for bootsrap hosted from static server (Nginx on RPI) 
  return "<html><head><title>Zurek ESP12 - " + title + "</title></head><body>" + content + "</body></html>";
}

void RichServer::templateOk(String content, String title, int httpCode)
{
  String response = responseTemplate(content, title);
  _espServer.send(httpCode, "text/html", response);
}

void RichServer::template404(String content, String title)
{
  if (content == ""){
    content = "File Not Found<br>";
    content += "URI: ";
    content += _espServer.uri();
    content += "<br>Method: ";
    content += (_espServer.method() == HTTP_GET)?"GET":"POST";
    content += "<br>Arguments: ";
    content += _espServer.args();
    content += "<br>";
    for (uint8_t i=0; i<_espServer.args(); i++){
      content += " " + _espServer.argName(i) + ": " + _espServer.arg(i) + "<br>";
    }
  }
  if (title == ""){
    title = "File Not Found!";
  }
  setColorHandler("404", {123,56,32});
  String response = responseTemplate(content, title);
  _espServer.send(404, "text/html", response);
}


