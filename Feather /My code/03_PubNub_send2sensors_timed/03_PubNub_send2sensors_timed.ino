/*
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
#define PubNub_BASE_CLIENT WiFiClient
#define PUBNUB_DEBUG
#include <PubNub.h>
#include <wifiDetails.h>


// Kate's PubNub keys
const static char pubkey[] = "pub-c-7a5852bf-1b25-4db2-a0ca-f7a370ea70d2";  //get this from your PUbNub account
const static char subkey[] = "sub-c-f36ef67a-e6a6-11e8-b820-164499a0198c";  //get this from your PubNub account

const static char pubChannel[] = "sensorValues"; //choose a name for the channel to publish messages to


unsigned long lastRefresh = 0;    //used to make the timer work




int publishRate = 50;           //how often to update the value on pubnub in milliseconds



int readPin1 = A3;


int sensorVal1;                       //variables to hold values to send


//average sensor values
int avgSensor1;


int counter;


// change to your name
String whoAmI = "Jingpo";

int lightOn = false;
String lightStat = "";


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

void loop()
{

  //sensorVal1 = analogRead(readPin1);      //read the sensor value and map it back to 0-1023 range
 sensorVal1 =  map(analogRead(readPin1), 0, 300, 0, 100);

  if (sensorVal1 > 400 && lightOn == false) {
    lightStat = "Light came On";
    //lightOn = true;
  }
  else {
    lightStat = "Light is Off";
    //lightOn = false;
  }
  counter++;
  if (millis() - lastRefresh >= publishRate) //timer used to publish the values at a given rate
  {
    avgSensor1 = avgSensor1 / counter;
  

    publishToPubNub();                      //execute the function that sends the values to pubnub
    lastRefresh = millis();                 //save the value so that the timer works

    avgSensor1 = 0;                         //this resets the average, can be removed to calculate the average over the entire time

    counter = 0;
  }

  

}



void publishToPubNub()
{
  WiFiClient *client;
  DynamicJsonBuffer messageBuffer(600);                    //create a memory buffer to hold a JSON Object
  JsonObject& pMessage = messageBuffer.createObject();    //create a new JSON object in that buffer

  ///the imporant bit where you feed in values
  pMessage["who"] = whoAmI;
  pMessage["sensorVal1"] = sensorVal1;                      //add a new property and give it a value
                    //add a new property and give it a value
  pMessage["avgSensorVal1"] = avgSensor1;

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
  Serial.print(sensorVal1);            
         
  Serial.print("\t");    // prints a tab         
  Serial.print(avgSensor1);
  Serial.print("\t");    // prints a tab

}
