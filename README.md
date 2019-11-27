# uGsm
it's just little library to help my projects, as I looked up there's alot of libraries to work with, but some how it doesn't do its work well
with sim900a.

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
#include <ugsm.h>
SoftwareSerial serialGSM(10, 11);
uGsm gsmClient;
char phone[13];
char *message;

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
  gsmClient.deleteAllSMS();
}

void loop()
{
  if (gsmClient.messageToRead())
  {
    gsmClient.readLastSMS(phone, &message);
    if (strcmp_P(message, PSTR("TURN1ON")) == 0)
    {
      digitalWrite(13, HIGH);
      gsmClient.sendSMS(F("092XXXXXXX"), F("LIGHT 1 HAS TURNED ON"));
      gsmClient.deleteAllSMS();
    }
    else if (strcmp_P(message, PSTR("TURN1OFF")) == 0)
    {
      digitalWrite(13, LOW);
      gsmClient.sendSMS(F("092XXXXXXX"), F("LIGHT 1 HAS TURNED OFF"));
      gsmClient.deleteAllSMS();
    }
  }
}
```


# TODO
1. a lot of things to do
