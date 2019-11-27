#include <SoftwareSerial.h>
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
