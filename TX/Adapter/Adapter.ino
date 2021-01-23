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

//uint8_t Plr, Col, Typ, Wp;
uint8_t Plr = 0x01;       //Player1
uint8_t Col = 0xB1;       //Colour - Red
uint8_t Typ = 0xD1;       //Type - Quad
uint8_t Wp  = 0xE1;       //Weapon - No
float int_arr[2];         //Lat+Lon

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
  decoder.begin(FrSkySportSingleWireSerial::SOFT_SERIAL_PIN_D2, &gps);

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
  decodeResult = decoder.decode();

  // Display data once a second to not interfeere with data decoding
  currentTime = millis();
  if(currentTime > displayTime)
  {
    displayTime = currentTime + 1000;

    Serial.println("");

    int_arr[0] = gps.getLat();
    int_arr[1] = gps.getLon();

    uint8_t i,j;  
    uint8_t to_send_arr[8]; // 2 x (32/8)
    float *p;
    uint8_t *p2;

    p2=(uint8_t *)int_arr;

    for (i=0;i<8;i++)
    {
      to_send_arr[i] = p2[i];
    }

    Udp.beginPacket(udpAddress,udpPort);

    for (j=0;j<8;j++)
    {
    Udp.write(to_send_arr[j]);
    }
    Udp.write(Plr);    Udp.write(Col);    Udp.write(Typ);    Udp.write(Wp);
    Udp.endPacket();
    Serial.println("");
  }
}