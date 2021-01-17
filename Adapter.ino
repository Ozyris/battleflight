#include "FrSkySportSensor.h"
#include "FrSkySportSensorGps.h"
#include "FrSkySportSingleWireSerial.h"
#include "FrSkySportDecoder.h"
#include "SoftwareSerial.h"


#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#ifndef STASSID
#define STASSID "BFServer"
#define STAPSK  "123456789"
#endif

String playerChar = "P1RedQuad0";
const char * udpAddress = "192.168.4.1";
const int udpPort = 4210;
WiFiUDP Udp;

FrSkySportSensorGps gps(FrSkySportSensor::ID_IGNORE);  // Create GPS sensor with default ID
FrSkySportDecoder decoder;                           // Create decoder object without polling

uint32_t currentTime, displayTime;
uint16_t decodeResult;

void setup()
{
  // Configure the decoder serial port and sensors (remember to use & to specify a pointer to sensor)
  decoder.begin(FrSkySportSingleWireSerial::SOFT_SERIAL_PIN_D1, &gps);

  Serial.begin(115200);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(STASSID, STAPSK);

  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
  Serial.printf("UDP server on port %d\n", udpPort);
  Udp.begin(udpPort);
}

void loop()
{
  decodeResult = decoder.decode(); //SPort decode

  // Display data once a second to not interfeere with data decoding
  currentTime = millis();
  if(currentTime > displayTime)
  {
    displayTime = currentTime + 1000;
    
    // Get and send GPS sensor data
    String msg = playerChar + "$n"+String(gps.getLat(), 6) + "$e"+String(gps.getLon(), 6)+"$s";
    Udp.beginPacket(udpAddress,udpPort);
    Udp.write(msg.c_str()); 
    Udp.endPacket();
  }
}
