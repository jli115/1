/* Code for the Night Stand Book
This code is for the C&C class 4 project.
Tabitha, JingPo & Frank Ferrao*/

/*
    Base Code supplied by:
   Creation & Computation - Digital Futures, OCAD University
   Kate Hartman / Nick Puckett




 * ***NOTE***
   When Wifi is enabled you can only use these pins as analog inputs
   A2
   A3
   A4
   A9 - 33
   A7 - 32

*/

#include <ArduinoJson.h>
#include "esp_wpa2.h"
#include <WiFi.h>
#define PUBNUB_DEBUG
#include <wifiDetails.h>

WiFiClient client;
String MakerIFTTT_Key ;
;String MakerIFTTT_Event;
char *append_str(char *here, String s) {  int i=0; while (*here++ = s[i]){i++;};return here-1;}
char *append_ul(char *here, unsigned long u) { char buf[20]; return append_str(here, ultoa(u, buf, 10));}
char post_rqst[256];char *p;char *content_length_here;char *json_start;int compi;


unsigned long lastRefresh = 0;    //used to make the timer work
int publishRate = 2000;           //how often to update the value on pubnub in milliseconds


int readPin2 = A3;

int sensorVal2;                       //variables to hold values to send

int counter;

// Remove all the PubNub code and switching to IFTT
// This will send an email votification


String whoAmI = "Frank";

int lightOn = false;
String lightStat = "";


// Enable Wifi on th Feather
void setup()
{
  Serial.begin(9600);

  // Connect to Wifi - You must have wifiDetails.h in your library folder for this to work.
  // Add your wifi location in the () - "0" for OCAD or "1" for home.
  // Note: you will need to update the wifiDetails.h file with your home wifi credentials.
  connectStandardWifi(1);                              

}


// Send Sensor Data from the Fether to PubNub and then send an SMS 
// Stil have to code this

void loop()
{

  

  if (sensorVal2 > 400 && lightOn == false) {
    lightStat = "Light came On";
    notifyIftt();
    lightOn = true;
  }
  else if (lightOn == true && sensorVal2 > 400){
    lightStat = "Light is Still On";
    lightOn = true;
  }
  else if (lightOn == true && sensorVal2 < 300){
    lightOn = false;
    lightStat = "Light went Off";
  }

  // This is where I need to check the value and then send and SMS message
  if (millis() - lastRefresh >= publishRate) //timer used to publish the values at a given rate
  {
    checkSensor();                      //execute the function that sends the values to pubnub
    lastRefresh = millis();                 //save the value so that the timer works

  }
  
}



void checkSensor() {

  sensorVal2 =  map(analogRead(readPin2), 0, 300, 0, 100); // check sensor Value
  
  Serial.print(lightStat);
  Serial.print("\t");
  Serial.print(sensorVal2);           
  Serial.print("\t");
}

void notifyIftt() {
  if (client.connect("maker.ifttt.com",80)) {
    MakerIFTTT_Key ="b3a90qWD7JZ-TyrQuTkOQr"; // new key b3a90qWD7JZ-TyrQuTkOQr
    MakerIFTTT_Event ="email";
    p = post_rqst;
    p = append_str(p, "POST /trigger/");
    p = append_str(p, MakerIFTTT_Event);
    p = append_str(p, "/with/key/");
    p = append_str(p, MakerIFTTT_Key);
    p = append_str(p, " HTTP/1.1\r\n");
    p = append_str(p, "Host: maker.ifttt.com\r\n");
    p = append_str(p, "Content-Type: application/json\r\n");
    p = append_str(p, "Content-Length: ");
    content_length_here = p;
    p = append_str(p, "NN\r\n");
    p = append_str(p, "\r\n");
    json_start = p;
    p = append_str(p, "{\"value1\":\"");
    p = append_str(p, "");
    p = append_str(p, "\",\"value2\":\"");
    p = append_str(p, "light is on!");
    p = append_str(p, "\",\"value3\":\"");
    p = append_str(p, "Huzzah!");
    p = append_str(p, "\"}");

    compi= strlen(json_start);
    content_length_here[0] = '0' + (compi/10);
    content_length_here[1] = '0' + (compi%10);
    client.print(post_rqst);

    Serial.println("Request Was Sent");
  }
}
