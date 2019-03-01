#ifndef UGSM_H_
#define UGSM_H_

// mini library to work with gsm900A specially
// created by Ahmed Alkabir
// well, I've to consider to echo property to library for better stability
#include <Arduino.h>
#include <string.h>
#include <stdint.h>
#include <SoftwareSerial.h>

class uGsm
{
public:
  uGsm(SoftwareSerial &serial, size_t speed = 9600)
  {
    _serial = &serial;
    _serial->begin(speed);
    response[0] = '\0';
  }

  //AT: send AT Command to GSM900A
  // if it's every thing ok it should return true, otherwise false
  bool AT(void);

  // ATSendCommand: send at command to gsm900a
  // don't forget to add \r at the end of command
  void ATSendCommand(const char *at);
  void ATSendCommand(const __FlashStringHelper *at);
  bool isConnected();

  // atResponse: Get AT Response, this function is block call until timeout.
  // note: timeout feature doesn't work for now, <<<< ADD IT >>>>
  bool atResponse(char *buffer, size_t bufferSize, unsigned long timeout);
  bool atResponse(unsigned long timeout = 50);

  // isResponse: it return true if expexted string occured 
  bool isResponse(const char *expected);
  bool isResponse(const __FlashStringHelper *expected);

  // isContain: if expected string existed in response from gsm900a it should return true, otherwise false
  bool isContain(const char *expected);
  bool isContain(const __FlashStringHelper *expected);

  // isOK: it's just a wrapper around isResponse(F("OK"))
  bool isOK(void);


  // callNumber: 
  void callNumber(const char *destination);

  // sendMessage: is going to send message to specific destination
  // message it must not exceed 160 characters
  // it's going return true in case message sent
  bool sendMessage(const char *destination, const char *message);

  // readSMS:
  // dummy read function for now
  // still underwork
  // for now can do basic job
  // if there's message coming read it
  bool readSMS(void);


  bool doCommand(const char *command, void (*func)());

private:
  SoftwareSerial *_serial;
  char response[203];

  // private methods
  int available();
  bool isListening();
  bool listen();
  void flush();
  size_t write(const char c);
  size_t write(const char *str);
  size_t write(const __FlashStringHelper *str);
  size_t readBytes(char buffer[], size_t lenght);
  void clearBufferFromSerial(void);
};
#endif