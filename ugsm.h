#ifndef UGSM_H_
#define UGSM_H_

// mini library to work with gsm900A specially
// created by Ahmed Alkabir
// well, I've to consider to echo property to library for better stability
#include <Arduino.h>
#include <string.h>
#include <stdint.h>
#include <SoftwareSerial.h>

template <typename T>
class uGsm
{
public:
  uGsm(T &serial, size_t speed = 9600)
  {
    _serialS = &serial;
    _serialS->begin(speed);
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
  bool sendMessage(const __FlashStringHelper *destination, const __FlashStringHelper *message);
  // readSMS:
  // dummy read function for now
  // still underwork
  // for now can do basic job
  // if there's message coming read it
  bool readSMS(void);

  // doCommand: it will execute callback function when specific command occured
  // you should use it when receiving messages like this
  //  if(gsm.readSMS()){
  //    gsm.doCommand(F("LIGHT1ON"), [](){
  //      digitalWrite(13, HIGH);
  //    });
  //  }
  bool doCommand(const char *command, void (*func)());
  bool doCommand(const __FlashStringHelper *command, void (*func)());

private:
  T *_serialS;
  char response[203];

  // private methods
  int available();
  // it's only for SoftwareSerial
  bool isListening();
  bool listen();

  void flush();
  size_t write(const char c);
  size_t write(const char *str);
  size_t write(const __FlashStringHelper *str);
  int read();
  size_t readBytes(char buffer[], size_t lenght);
  size_t print(const char *str);
  void clearBufferFromSerial(void);
};

// the implementation
template <typename T>
bool uGsm<T>::isConnected()
{
  if (AT())
    return true;
  else
    return false;
}

template <typename T>
int uGsm<T>::available(void)
{
  // if (_serialH)
  //     return _seriaH->available();

  if (_serialS)
    return _serialS->available();
  return -1;
}

template <typename T>
bool uGsm<T>::isListening(void)
{
  // if (_serialH)
  //     return _serialH->isListening();

  if (_serialS)
    return _serialS->isListening();
  return false;
}

template <typename T>
bool uGsm<T>::listen(void)
{
  if (_serialS)
    return _serialS->listen();
  return false;
}

// Specialization methods for HardwareSerial
template <>
bool uGsm<HardwareSerial>::isListening(void)
{
  return true;
}

template <>
bool uGsm<HardwareSerial>::listen(void)
{
  return true;
}

template <typename T>
void uGsm<T>::flush(void)
{
  // if (_serialH)
  //     _serialH->flush();

  if (_serialS)
    _serialS->flush();
}

template <typename T>
size_t uGsm<T>::write(const char c)
{
  // if (_serialH)
  //     return _serialH->write(c);

  if (_serialS)
    return _serialS->write(c);
  return 0;
}

template <typename T>
size_t uGsm<T>::write(const char str[])
{
  // if (_serialH)
  //     _serialH->write(str);

  if (_serialS)
    _serialS->write(str);
  return 0;
}

template <typename T>
size_t uGsm<T>::write(const __FlashStringHelper *str)
{
  char buffer[strlen_P((const char *)str) + 1];
  strcpy_P(buffer, (const char *)str);
  return write(buffer);
}

template <typename T>
size_t uGsm<T>::readBytes(char buffer[], size_t length)
{
  // if (_serialH)
  //     return _serialH->readBytes(buffer, length);

  if (_serialS)
    return _serialS->readBytes(buffer, length);
  return 0;
}

// TODO: Complete this function
// main purpose to clear buffer from the serial instance
template <typename T>
void uGsm<T>::clearBufferFromSerial(void)
{
  if (!isListening())
    listen();

  auto t = millis();
  // if (_serialH)
  // {
  //     while (_serialH->available() == 0 && millis() - t < 50)
  //         ;
  //     if (_serialH->available() == 0)
  //         return;
  // }

  if (_serialS)
  {
    while (_serialS->available() == 0 && millis() - t < 50)
      ;
    if (_serialS->available() == 0)
      return;
  }
}

// a wrapper around print method of SoftwareSerial & HardWareSerial objects
template <typename T>
size_t uGsm<T>::print(const char *str)
{
  // if (_serialH)
  // {
  //     return _serialH->print(str);
  // }
  if (_serialS)
  {
    return _serialS->print(str);
  }
  return -1;
}

// a wrapper around read method of SoftwareSerial & HardWareSerial objects
template <typename T>
int uGsm<T>::read()
{
  return _serialS->read();
}
// is going to send at command to gsm900a
// and as I stated it won't turn back echo for now
template <typename T>
void uGsm<T>::ATSendCommand(const char *at)
{
  if (!isListening())
    listen();

  print(at);
  // as echo is enabled as default mode
  // so I guess, I have to deal with it
}

template <typename T>
void uGsm<T>::ATSendCommand(const __FlashStringHelper *at)
{
  char buffer[strlen_PF((const char *)at) + 1];
  strcpy_P(buffer, (const char *)at);
  return ATSendCommand(buffer);
}

// Get AT Response, this function is block call until timeout.
template <typename T>
bool uGsm<T>::atResponse(char *buffer, size_t bufferSize, unsigned long timeout)
{
  // let's make sure, there's data to parse
  // TODO: Add timout feature
  while (true)
  {
    // if (_serialS->available() > 0)
    //     break;
    if (available() > 0)
      break;
  }

  uint8_t i = 0;
  while (true)
  {
    while (available() > 0)
    {
      buffer[i++] = read();
      if (i >= 2)
      {
        if (buffer[i - 2] == '\r' && buffer[i - 1] == '\n')
          break;
      }
    }
    if (i >= 2)
    {
      if (buffer[i - 2] == '\r' && buffer[i - 1] == '\n')
      {
        buffer[i - 2] = '\0';
        if (strlen(buffer) > 0)
          break;
        else
          i = 0;
      }
    }
  }
  // wait before receiving the respond from
  // the gsm900a
  delay(5);
  return true;
}

template <typename T>
bool uGsm<T>::atResponse(unsigned long timeout = 50)
{
  return atResponse(response, sizeof(response), timeout);
}

template <typename T>
bool uGsm<T>::isResponse(const char *expected)
{
  if (strcmp(response, expected) != 0)
    return false;
  return true;
}

template <typename T>
bool uGsm<T>::isResponse(const __FlashStringHelper *expected)
{
  char buffer[strlen_PF((const char *)expected) + 1];
  strcpy_P(buffer, (const char *)expected);
  return isResponse(buffer);
}

template <typename T>
bool uGsm<T>::isContain(const char *expected)
{
  if (strstr(response, expected) == NULL)
    return false;
  return true;
}

template <typename T>
bool uGsm<T>::isContain(const __FlashStringHelper *expected)
{
  char buffer[strlen_PF((const char *)expected) + 1];
  strcpy_P(buffer, (const char *)expected);
  return isContain(buffer);
}

template <typename T>
bool uGsm<T>::isOK(void)
{
  if (!(isResponse(F("OK")) || isResponse(F("OK\r"))))
    return false;
  if (available())
    clearBufferFromSerial();
  return true;
}

template <typename T>
bool uGsm<T>::AT(void)
{
  const __FlashStringHelper *command = F("AT\r");

  // send command to gsm
  // TODO: do function for it -----> DONE
  ATSendCommand(command);
  while (true)
  {
    if (!atResponse())
    {
      break;
    }
    if (isOK())
      return true;
    else
      return false;
  }
  return false;
}

template <typename T>
void uGsm<T>::callNumber(const char *destination)
{
  const __FlashStringHelper *command = F("ATD%s;\r");
  char buffer[8 + strlen(destination)];
  sprintf_P(buffer, (const char *)command, destination);
  ATSendCommand(buffer);
}

template <typename T>
bool uGsm<T>::sendMessage(const char *destination, const char *message)
{
  // wait 200ms to send message
  delay(200);
  const __FlashStringHelper *_command = F("AT+CMGS=\"%s\"\r");
  const __FlashStringHelper *_response = F("+CMGS: ");
  uint8_t length = 0;
  char buffer[12 + strlen(destination)];

  sprintf_P(buffer, (const char *)_command, destination);
  // let's clear buffer first
  clearBufferFromSerial();
  ATSendCommand(buffer);
  delay(500);
  for (size_t i = 0; i < strlen(message) && length < 160; i++)
  {
    if (message[i] != '\r')
    {
      write(message[i]);
      length++;
    }
  }
  // let's sent message to destination
  delay(100);
  write(0x1A);
  write(0x0D);
  write(0x0A);
  while (!isContain(_response))
  {
    atResponse();
  }
  if (isContain(_response))
    return true;
  else
    return false;
}

template <typename T>
bool uGsm<T>::sendMessage(const __FlashStringHelper *destination, const __FlashStringHelper *message)
{
  char buf_dest[strlen_PF((const char *)destination) + 1];
  char buf_messages[strlen_PF((const char *)message) + 1];

  strcpy_P(buf_dest, (const char *)destination);
  strcpy_P(buf_messages, (const char *)message);
  return sendMessage(buf_dest, buf_messages);
}

// dummy read function for now
// still underwork
// for now can do basic job
// if there's message coming read it
template <typename T>
bool uGsm<T>::readSMS(void)
{
  const __FlashStringHelper *_command = F("AT+CMGR=%s\r");
  char message_number[3];
  // form of recivied sms notification
  // is
  // +CMTI: "SM",00
  while (atResponse())
  {
    if (isContain("+CMTI:"))
      break;
  }
  message_number[0] = response[12];
  message_number[1] = response[13];
  message_number[2] = '\0';
  char buffer[13];
  sprintf_P(buffer, (const char *)_command, message_number);
  ATSendCommand(buffer);
  while (atResponse())
  {
    if (isContain("+CMGR:"))
      break;
  }
  atResponse();
  // Serial.println(response);
  return true;
}

template <typename T>
bool uGsm<T>::doCommand(const char *command, void (*func)())
{
  if (isContain(command))
  {
    func();
    return true;
  }
  return false;
}

template <typename T>
bool uGsm<T>::doCommand(const __FlashStringHelper *command, void (*func)())
{
  if (isContain(command))
  {
    func();
    return true;
  }
  return false;
}

#endif