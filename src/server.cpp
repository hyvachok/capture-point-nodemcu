#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

extern "C"
{
#include <user_interface.h>
}

#define RED_LED D3
#define GREEN_LED 2
#define BLUE_BUTTON 4
#define BLACK_BUTTON 5

const char *ssid = "BEREG-WIFI";
const char *password = "12345678";

IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

int counter = 0;
long lastClientsCheck = 0;
long lastCapturingCheck = 0;
bool capturing = false;
long timeToCapture = 1000 * 60 * 5; //Defines the time required to capture

ESP8266WebServer server(80); //Server on port 80

void defaultRoute()
{
  String web = "<!DOCTYPE html><html lang=\"ru-RU\"> <head> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\"/> <meta charset=\"UTF-8\"/> <title>Bereg point</title> <style>html{font-family: Arial, Helvetica, sans-serif; display: inline-block; margin: 0px auto; text-align: center;}body{margin-top: 50px; background-color: #141414;}h1{color: #444444; margin: 50px auto 30px;}h3{color: #444444; margin-bottom: 50px;}.button{display: block; background-color: #1abc9c; border: none; color: rgb(150, 150, 150); padding: 13px 30px; text-decoration: none; font-size: 25px; margin: 0px auto 35px; cursor: pointer; border-radius: 4px;}.button-on{background-color: #c92800;}.button-on:active{background-color: #af2300;}.button-off{background-color: #34495e;}.button-off:active{background-color: #2c3e50;}p{font-size: 14px; color: #888; margin-bottom: 10px;}.point-digit{color: #960000;}</style> </head> <body>\"";

  web += "<h1>Точка номер <span class=\"point-digit\">1</span></h1>";
  web += "<h3>Для старта захвата точки нажмите кнопку \"ЗАХВАТ\"</h3>";
  web += "<p> Состояние: ";
  if (capturing)
  {
    web += "Идёт захват</p>";
    web += "<a class=\"button button-on\" href=\"/stop\">ОСТАНОВИТЬ ЗАХВАТ</a> ";
  }
  else
  {
    web += "Свободно</p>";
    web += "<a class=\"button button-off\" href=\"/start\">ЗАХВАТ</a>";
  }
  web += "</body>";
  web += "</html>";
  server.send(200, "text/html", web);
}

void startRoute()
{
  capturing = true;
  String web = "<meta http-equiv=\"refresh\" content=\"0; url=/\" />";
  server.send(200, "text/html", web);
}

void stopRoute()
{
  capturing = false;
  String web = "<meta http-equiv=\"refresh\" content=\"0; url=/\" />";
  server.send(200, "text/html", web);
}

void initRoutes()
{
  server.on("/", defaultRoute);
  server.on("/start", startRoute);
  server.on("/stop", stopRoute);
}

void client_status()
{

  unsigned char number_client;
  struct station_info *stat_info;

  struct ip4_addr *IPaddress;
  IPAddress address;
  int i = 1;

  number_client = wifi_softap_get_station_num();
  stat_info = wifi_softap_get_station_info();

  Serial.print(" Total Connected Clients are = ");
  Serial.println(number_client);

  while (stat_info != NULL)
  {
    IPaddress = &stat_info->ip;
    address = IPaddress->addr;

    Serial.print("client= ");

    Serial.print(i);
    Serial.print(" IP adress is = ");
    Serial.print((address));
    Serial.print(" with MAC adress is = ");

    Serial.print(stat_info->bssid[0], HEX);
    Serial.print(" ");
    Serial.print(stat_info->bssid[1], HEX);
    Serial.print(" ");
    Serial.print(stat_info->bssid[2], HEX);
    Serial.print(" ");
    Serial.print(stat_info->bssid[3], HEX);
    Serial.print(" ");
    Serial.print(stat_info->bssid[4], HEX);
    Serial.print(" ");
    Serial.print(stat_info->bssid[5], HEX);
    Serial.print(" ");

    stat_info = STAILQ_NEXT(stat_info, next);
    i++;
    Serial.println();
  }
}

void setup(void)
{
  Serial.begin(9600);
  Serial.println("");
  WiFi.mode(WIFI_AP); //Only Access point
  WiFi.softAP(ssid);  //To enable password access add password variable
  WiFi.softAPConfig(local_ip, gateway, subnet);

  IPAddress myIP = WiFi.softAPIP(); //Get IP address
  Serial.print("HotSpt IP:");
  Serial.println(myIP);

  initRoutes();

  server.begin(); //Start server
  Serial.println("HTTP server started");

  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_BUTTON, INPUT_PULLUP);
  pinMode(BLACK_BUTTON, INPUT_PULLUP);
}

void loop(void)
{
  server.handleClient(); //Handle client requests

  long now = millis();
  if (capturing)
  {
    if (now - lastCapturingCheck >= 1000)
    {
      timeToCapture = timeToCapture - 1000;
      lastCapturingCheck = now;
      if (timeToCapture <= 0)
      {
        capturing = false;
      }
    }
  }
  else
  {
    if (now - lastCapturingCheck >= 1000)
    {
      lastCapturingCheck = now;
      if (timeToCapture <= 1000 * 60 * 5)
      {
        timeToCapture = timeToCapture + 1000;
      }
    }
  }

  if (now - lastClientsCheck >= 5000)
  {
    Serial.println(timeToCapture);

    lastClientsCheck = now;
    client_status();
  }
}
