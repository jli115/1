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
#define PubNub_BASE_CLIENT WiFiClient
#include <PubNub.h>

WiFiClient client;
String MakerIFTTT_Key ;
;String MakerIFTTT_Event;
char *append_str(char *here, String s) {  int i=0; while (*here++ = s[i]){i++;};return here-1;}
char *append_ul(char *here, unsigned long u) { char buf[20]; return append_str(here, ultoa(u, buf, 10));}
char post_rqst[256];char *p;char *content_length_here;char *json_start;int compi;

const static char pubkey[] = "pub-c-7a5852bf-1b25-4db2-a0ca-f7a370ea70d2";  //get this from your PUbNub account
const static char subkey[] = "sub-c-f36ef67a-e6a6-11e8-b820-164499a0198c";  //get this from your PubNub account

const static char pubChannel[] = "sensorValues"; //choose a name for the channel to publish messages to



unsigned long lastRefresh = 0;    //used to make the timer work
int publishRate = 2000;           //how often to update the value on pubnub in milliseconds


int readPin2 = A3;

int sensorVal2;                       //variables to hold values to send

int avgSensor2;

int counter;

// Remove all the PubNub code and switching to IFTT
// This will send an email votification


String whoAmI = "Jingpo";

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
    PubNub.begin(pubkey, subkey);                      //connect to the PubNub Servers
  Serial.println("PubNub Connected");                            

}


// Send Sensor Data from the Fether to PubNub and then send an SMS
// Stil have to code this

void loop()
{

sensorVal2 =  map(analogRead(readPin2), 0, 300, 0, 100);  

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

 counter++;
  // This is where I need to check the value and then send and SMS message
  if (millis() - lastRefresh >= publishRate) //timer used to publish the values at a given rate
  {
    checkSensor();                      //execute the function that sends the values to pubnub
    lastRefresh = millis();                 //save the value so that the timer works
avgSensor2 = avgSensor2 / counter;
  

    publishToPubNub();                      //execute the function that sends the values to pubnub
    lastRefresh = millis();                 //save the value so that the timer works

    avgSensor2 = 0;                         //this resets the average, can be removed to calculate the average over the entire time

    counter = 0;
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
    MakerIFTTT_Key ="b3a90qWD7JZ-TyrQuTkOQr";
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
    p = append_str(p, "esmcnulty@gmail.com");
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

void publishToPubNub()
{
  WiFiClient *client;
  DynamicJsonBuffer messageBuffer(600);                    //create a memory buffer to hold a JSON Object
  JsonObject& pMessage = messageBuffer.createObject();    //create a new JSON object in that buffer

  ///the imporant bit where you feed in values
  pMessage["who"] = whoAmI;
  pMessage["sensorVal2"] = sensorVal2;                      //add a new property and give it a value
                    //add a new property and give it a value
  pMessage["avgSensorVal2"] = avgSensor2;

  //pMessage.prettyPrintTo(Serial);   //uncomment this to see the messages in the serial monitor


  int mSize = pMessage.measureLength() + 1;                   //determine the size of the JSON Message
  char msg[mSize];                                            //create a char array to hold the message
  pMessage.printTo(msg, mSize);                              //convert the JSON object into simple text (needed for the PN Arduino client)

  client = PubNub.publish(pubChannel, msg);                      //publish the message to PubNub

  if (!client)                                                //error check the connection
  {
    Serial.println("client error");
    delay(1000);
    return;
  }

  if (PubNub.get_last_http_status_code_class() != PubNub::http_scc_success)  //check that it worked
  {
    Serial.print("Got HTTP status code error from PubNub, class: ");
    Serial.print(PubNub.get_last_http_status_code_class(), DEC);
  }

  while (client->available())                                 //get feedback from PubNub
  {
    Serial.write(client->read());
  }
  client->stop();                                             //stop the connection
  Serial.print("Successful Publish");
  Serial.print("\t");    // prints a tab
  Serial.print(whoAmI);
  Serial.print("\t");    // prints a tab
  Serial.print(sensorVal2);            
         
  Serial.print("\t");    // prints a tab         
  Serial.print(avgSensor2);
  Serial.print("\t");    // prints a tab

}
