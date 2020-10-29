#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

#define RED_LED 0
#define GREEN_LED 2
#define BLUE_BUTTON 4
#define BLACK_BUTTON 5

const char *ssid = "BEREG-WIFI";
int counter = 0;

ESP8266WebServer server(80); //Server on port 80

void defaultRoute()
{
  String web = "";
  web += "<h1>Counter is ";
  web += String(counter);
  web += "</h1>";
  server.send(200, "text/html", web);
}

void incrementRoute()
{
  counter++;
  server.send(200, "text/html", "<h1>Counter incremented</h1>");
}

void initRoutes()
{
  server.on("/", defaultRoute);
  server.on("/add", incrementRoute);
}

void buttonsLed(int first, int second)
{
  if (first == LOW)
  {
    digitalWrite(GREEN_LED, HIGH);
  }
  else
  {
    digitalWrite(GREEN_LED, LOW);
  }
  if (second == LOW)
  {
    digitalWrite(RED_LED, HIGH);
  }
  else
  {
    digitalWrite(RED_LED, LOW);
  }
}

void setup(void)
{
  Serial.begin(9600);
  Serial.println("");
  WiFi.mode(WIFI_AP); //Only Access point
  WiFi.softAP(ssid);

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

  int blueBtn = digitalRead(BLUE_BUTTON);
  int blackBtn = digitalRead(BLACK_BUTTON);
  buttonsLed(blueBtn, blackBtn);
}
