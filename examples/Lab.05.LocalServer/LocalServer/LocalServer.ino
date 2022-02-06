/*
   Local Server
   Modified from ESP8266 AdvancedWebServer Example

   Copyright (c) 2015, Majenko Technologies
   All rights reserved.

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

 * * Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.

 * * Redistributions in binary form must reproduce the above copyright notice, this
     list of conditions and the following disclaimer in the documentation and/or
     other materials provided with the distribution.

 * * Neither the name of Majenko Technologies nor the names of its
     contributors may be used to endorse or promote products derived from
     this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
   ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <SLAS2022.h>

//Adafruit FeatherWing display i2c address: 0x3C
U8G2_SH1107_64X128_F_HW_I2C u8g2(U8G2_R1, /* reset=*/ U8X8_PIN_NONE, /* clock=*/ SCL, /* data=*/ SDA);   // Adafruit Feather ESP8266/32u4 Boards + FeatherWing OLED

const char *ssid = "YourSSIDHere";
const char *password = "YourPSKHere";

ESP8266WebServer server(80);

const int led = 13;

void handleRoot() {
  digitalWrite(led, 1);
  char temp[400];
  int sec = millis() / 1000;
  int min = sec / 60;
  int hr = min / 60;

  snprintf(temp, 400,

           "<html>\
  <head>\
    <meta http-equiv='refresh' content='5'/>\
    <title>ESP8266 Demo</title>\
    <style>\
      body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
    </style>\
  </head>\
  <body>\
    <h1>Hello from ESP8266!</h1>\
    <p>Uptime: %02d:%02d:%02d</p>\
    <img src=\"/test.svg\" />\
  </body>\
</html>",

           hr, min % 60, sec % 60
          );
  server.send(200, "text/html", temp);
  digitalWrite(led, 0);
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void) {
  u8g2.begin();
  u8g2_prepare();
  
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.print("Connecting to WiFi..");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  u8g2.clearBuffer();
  u8g2.setCursor(0, 0);
  u8g2.print("SSID: ");
  u8g2.println(ssid);
  u8g2.setCursor(0, 16);
  u8g2.print("IP: ");
  u8g2.println(WiFi.localIP());
  u8g2.sendBuffer();
  

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.on("/test.svg", drawGraph);
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

const int N_POINTS = 380;
double dataset[N_POINTS];
int idx = 0;

void u8g2_prepare(void) {
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.setFontRefHeightExtendedText();
  u8g2.setDrawColor(1);
  u8g2.setFontPosTop();
  u8g2.setFontDirection(0);
}

void pollPhotoCell() {
  int sensorValue = analogRead(A0); // read the input on analog pin 0:
  double intensity = map(sensorValue, 11, 926, 1, 400); // scale the value to lux (1 - 400)
  dataset[idx] = intensity;
  idx++;
  if(idx>379) { idx = 0; }
}

uint32_t last_time_1 = 0;
uint32_t task_1_interval = 10;



void loop(void) {
  uint32_t now = millis();
  server.handleClient();
  if ( (now - last_time_1) > task_1_interval) {
    pollPhotoCell();
    last_time_1 = now;
  }
}

void drawGraph() {
  Serial.println("graphing");

  char num_buf [16];
  String out = "";
  char temp[100];
  out += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\" width=\"";
  
  static int graphWidth = 600;
  static int graphHeight = 200;
  static int padding = 10;

  float x_min = 0;
  float x_max = N_POINTS;
  float y_min = 1;
  float y_max = 400;

  sprintf (num_buf, "%03i", graphWidth);
  out += num_buf;
  out += "\" height=\"";
  sprintf (num_buf, "%03i", graphHeight);
  out += num_buf;
  out += "\">\n";
  out += "<rect width=\"";
  sprintf (num_buf, "%03i", graphWidth);
  out += num_buf;
  out += "\" height=\"";
  sprintf (num_buf, "%03i", graphHeight);
  out += num_buf;
  out += "\" fill=\"rgb(220, 220, 220)\" stroke-width=\"1\" stroke=\"rgb(0, 0, 0)\" />\n";
  out += "<text fill=\"#000\" font-size=\"12\" font-family=\"Verdana\" x=\"10\" y=\"190\">";
  sprintf (num_buf, "%.2f", x_min);
  out += num_buf;
  out += "</text>";
  out += "<text fill=\"#000\" font-size=\"12\" font-family=\"Verdana\" x=\"550\" y=\"190\">";
  sprintf (num_buf, "%.2f", x_max);
  out += num_buf;
  out += "</text>";
  out += "<text fill=\"#000\" font-size=\"12\" font-family=\"Verdana\" x=\"5\" y=\"150\">";
  sprintf (num_buf, "%.2f", y_min);
  out += num_buf;
  out += "</text>";
  out += "<text fill=\"#000\" font-size=\"12\" font-family=\"Verdana\" x=\"5\" y=\"15\">";
  sprintf (num_buf, "%.2f", y_max);
  out += num_buf;
  out += "</text>";
  
  out += "<g stroke=\"black\">\n";
  for (int x = 0; x < N_POINTS - 2; x++) {

    int mx1 = safeMap<float>(x,  x_min,  x_max, padding, (graphWidth-padding) );
    int mx2 = safeMap<float>(x+1,  x_min,  x_max, padding, (graphWidth-padding) );
    int my1 = safeMap<float>(dataset[x],  y_min,  y_max, (graphHeight-padding), padding );
    int my2 = safeMap<float>(dataset[x+1],  y_min,  y_max, (graphHeight-padding), padding );

    sprintf(temp, "<line x1=\"%d\" y1=\"%d\" x2=\"%d\" y2=\"%d\" stroke-width=\"1\" />\n", mx1, my1, mx2, my2);
    out += temp;
  }
  out += "</g>\n</svg>\n";

  server.send(200, "image/svg+xml", out);
}
