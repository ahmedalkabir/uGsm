# uGsm
it's just little library to help my projects, as I looked up there's alot of libraries to work with, but some how it doesn't do its work well
with gsm900A.

# What Works Currently 
1. you can make a phone call
2. and send message to specific destination
3. receving messages and execute the recevied commands

# Note Before Using Library
well, there's something to consider when we receive the message the problem is with SoftwareSerial it has limited buffer of received data
from the sim900a it's about 64 bytes which is not sufficient for receiving message so we need to modify the value by chaning it to 128 bytes 

# How you change the size of received Buffer 
it's simple just go to C:\Program Files (x86)\Arduino\hardware\arduino\avr\libraries\SoftwareSerial\src
and open up SoftwareSerial.h and look for _SS_MAX_RX_BUFF 64 
and replace 64 by 128 you can replace it with any value as long it won't exceed the ram size

# Notes for Arduino Mega 
I prefer to use hardware Serial in Arduino Mega as it has three hardware serials so it's efficient to use them better than SoftwareSerial, as a SoftwareSerial it has limite size of buffer so you have to change it by heading to C:\Program Files (x86)\Arduino\hardware\arduino\avr\cores\arduino, open up HardwareSerial.h and look up for SERIAL_RX_BUFFER_SIZE 64, replace 64 by 128 
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
1. a lot of things to do