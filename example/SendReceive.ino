#include <SoftwareSerial.h>
#include<ugsm.h>

// create serial object and wrapper it around 
// uGsm class do let it do works for you

SoftwareSerial serial_1(10, 11);
// default speed it's 9600
// in case you want to change to speed just add second parameter
// uGsm gsm1(serial_1, 115200);
uGsm gsm1(serial_1);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(13, OUTPUT);
  Serial.println("START PROJECT");
  Serial.println("Starting the GSM900A .....");
  while(!gsm1.isConnected())
    ;
  Serial.println("GSM900A is ready to work :)");
  Serial.println("Let's delete messages from gsm900a");

  
  delay(300);
  gsm1.ATSendCommand(F("AT+CMGD=0,4\r"));
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println("START Reciving Messages");
  if(gsm1.readSMS()){
    gsm1.doCommand(F("PUMP2ON"), [](){
      Serial.println("PUMP2ON is on");
      digitalWrite(13, HIGH);
      gsm1.sendMessage(F("092XXXXXXX"), F("PUMP 2 is turned on"));
    });
    gsm1.doCommand(F("PUMP2OFF"), [](){
      Serial.println("PUMP2ON is OFF");
      digitalWrite(13, LOW);
      gsm1.sendMessage(F("092XXXXXXX"), F("PUMP 2 is turned off"));
    });
  }

}