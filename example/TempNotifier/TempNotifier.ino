#include <SoftwareSerial.h>
#include <ugsm.h>
#include <Wire.h>
#include "Adafruit_HTU21DF.h"

Adafruit_HTU21DF htu = Adafruit_HTU21DF();

// here we'r going to define our software uart streamer to 
// use in uGsm library
SoftwareSerial serialGSM(10, 11);
// declare a uGsm Object
uGsm gsmClient;


void setup()
{
  // put your setup code here, to run once:
  serialGSM.begin(9600);
  Serial.begin(9600);

  pinMode(13, OUTPUT);

  gsmClient.begin(&serialGSM);

  if (!htu.begin()) {
    Serial.println("Couldn't find sensor!");
    while (1);
  }

  Serial.println(F("START PROJECT"));
  Serial.println(F("Starting the GSM900A ....."));

  // check sim900a if it's powered up
  while (!gsmClient.isPoweredUp(3000))
  {
    Serial.println(F("check your module is connected or powered up"));
    return;
  }

  // check if it's registered
  while (!gsmClient.isRegistered(3000))
  {
    Serial.println(F("YOUR SIM is not registered to the network"));
    return;
  }

  Serial.println(F("START RECEIVE COMMANDS"));
}

void loop()
{
  float temp = htu.readTemperature();
  float rel_hum = htu.readHumidity();
  if(temp > 30.0){
    gsmClient.sendSMS(F("0923305946"), F("Temerature is Hotter than threshold value"))
  }
}
