# uGsm
it's just little library to help my projects, as I looked up there's alot of libraries to work with, but some how it doesn't do its work well
with gsm900A.

# What Works Currently 
1. you can make a phone call
2. and send message to specific destination
3. receving messages and execute the recevied commands


# snippet of uGsm example
```c++
#include <SoftwareSerial.h>
#include <ugsm.h>

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

  // we wait for message and execute it based on the 
  // received message
  if (gsmClient.messageToRead())
  {
    gsmClient.doCommand(F("TURN1ON"), []() -> void {
      Serial.println("ON1");
    });
    gsmClient.doCommand(F("TURN1OFF"), []() -> void {
      Serial.println("OFF1");
    });
    gsmClient.doCommand(F("TURN2ON"), []() -> void {
      Serial.println("ON2");
    });
    gsmClient.doCommand(F("TURN2OFF"), []() -> void {
      Serial.println("OFF2");
    });
  }
}
```


# TODO
1. a lot of things to do, but currently can do basic things