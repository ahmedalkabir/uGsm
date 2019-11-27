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
      digitalWrite(13, HIGH);
    });
    gsmClient.doCommand(F("TURN1OFF"), []() -> void {
      digitalWrite(13, LOW);
    });
  }
}
