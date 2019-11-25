# uGsm
it's just little library to help my projects, as I looked up there's alot of libraries to work with, but some how it doesn't do its work well
with gsm900A.

# What Works Currently 
1. you can make a phone call
2. and send message to specific destination
3. receving messages and execute the recevied commands


# example of how to define uGsm object
```c++
// create serial object and wrapper it around 
// uGsm class do let it do works for you

SoftwareSerial serial_1(10, 11);
// default speed it's 9600
// in case you want to change to speed just add second parameter
// uGsm gsm1(serial_1, 115200);
// uGsm<SoftwareSerial> gsm1(serial_1);

// for ArduinoMega in case SoftwareSerial doesn't work properly use built-in hardware serial communication
// Serial1 ===> RX is 19 and TX is 18
uGsm<HardwareSerial> gsm1(Serial1);
```

other interface is still the same


# snippet of What uGsm can do
```c++
  // readSMS is going to block the process 
  // and wait for incoming messages, if there's message
  // it will execute one of command below if it's occured
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
```


# TODO
0. add event-driven approach to library.
1. add timeout to blocking functions 
2. Create simple examples
3. Add string readSMS() function to allow user read the message
4. Add deleteALLSMS() & deleteSMS(int index) functions