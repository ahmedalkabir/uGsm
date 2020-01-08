#ifndef UGSM_H_
#define UGSM_H_

// mini library to work with sim900a specially
// created by Ahmed Alkabir
#include <Arduino.h>
#include <utility_gsm.hpp>
#include <string.h>
#include <stdint.h>

// TODO: add feature to check of version of module

#define BUFFER_SIZE 203

template<typename T>
class uGsm
{
public:
  uGsm() {}
  // to start a project by assign a serial object
  void begin(Stream *_serial);

  // to check if the arduino is connected to module
  bool isPoweredUp(uint16_t time_out);

  // to check if the module is registered to network
  bool isRegistered(uint16_t time_out);

  // to send message to specifc destenation
  bool sendSMS(const char *dst, const char *msg);
  bool sendSMS(const __FlashStringHelper *dst, const __FlashStringHelper *msg);

  // return true if there's income message to read
  // implementation of sim900a
  template<typename F = T>
  typename enable_if<is_sim900a<F>::value, bool>::type messageToRead();

  // implementation of sim900
  template<typename F = T>
  typename enable_if<is_sim900<F>::value, bool>::type messageToRead();

  // implementation of sim900
  template<typename F = T>
  typename enable_if<is_sim800l<F>::value, bool>::type messageToRead();

  // implementation of sim800L
  // return 1 if readSMS read message successfully otherwise return 0, and return -1
  // for invalid input
  template<typename F = T>
  typename enable_if<is_sim900a<F>::value, int>::type 
  readLastSMS(char *phone_number, char **received_message);

  template<typename F = T>
  typename enable_if<is_sim900<F>::value, int>::type 
  readLastSMS(char *phone_number, char **received_message);

  template<typename F = T>
  typename enable_if<is_sim800l<F>::value, int>::type
  readLastSMS(char *phone_number, char **received_message);

  // TODO : prevent from entering unreasonable number
  // return 1 if readSMS read message successfully otherwise return 0, and return -1
  // for invalid input
  int readSMS(uint8_t index_m, char *phone_number, char **received_message);

  // delteSMS : delete the message by specific index
  bool deleteSMS(uint8_t index_m);

  // is used to delete all messages stored in sms
  bool deleteAllSMS();

  // it's unuseful function and might move it to private
  // to disable Echo
  void disableEcho();
  // to enable Echo
  void enableEcho();

private:
  Stream *_serialGSM = nullptr;
  char buffer[BUFFER_SIZE];
  char last_message_index[3];
  // private methods
  inline void write_at_command(const char *cmd);
  inline void write_at_command(const __FlashStringHelper *cmd);
  void flush_the_serial_and_buffer();
  bool wait_for_response(const char *rsp, uint16_t time_out);
  bool wait_for_response(const __FlashStringHelper *rsp, uint16_t time_out);
  char *read_buffer();
  inline bool is_contain_response(const char *rsp);
  inline bool is_contain_response(const __FlashStringHelper *rsp);
};

// implementation of private methods
template<typename T>
char *uGsm<T>::read_buffer()
{
  if (_serialGSM->available() > 0)
  {
    char ch;
    char *ptr = buffer;
    while (_serialGSM->available() > 0)
    {
      ch = _serialGSM->read();
      *ptr++ = ch;
    }
    *ptr = '\0';
  }
  // let's make a few delay between command
  // Serial.println(buffer);
  delay(20);
  return buffer;
}

template<typename T>
void uGsm<T>::write_at_command(const char *cmd)
{
  flush_the_serial_and_buffer();
  uint8_t cmd_len = strlen(cmd);
  for (uint8_t i = 0; i < cmd_len; i++)
  {
    _serialGSM->write(cmd[i]);
  }
  delay(500);
}

template<typename T>
void uGsm<T>::write_at_command(const __FlashStringHelper *cmd)
{
  char cmdR[strlen_P(reinterpret_cast<const char *>(cmd)) + 1];
  strcpy_P(cmdR, reinterpret_cast<const char *>(cmd));
  write_at_command(cmdR);
}

template<typename T>
bool uGsm<T>::wait_for_response(const char *response, uint16_t time_out = 1000)
{
  uint16_t last_time = millis();
  while (1)
  {
    const char *pBuffer = read_buffer();
    if (is_contain_response(response))
    {
      flush_the_serial_and_buffer();
      return true;
    }

    if (millis() - last_time > time_out)
    {
      flush_the_serial_and_buffer();
      return false;
    }
  }
  return false;
}

template<typename T>
bool uGsm<T>::wait_for_response(const __FlashStringHelper *response, uint16_t time_out)
{
  char responseR[strlen_P(reinterpret_cast<const char *>(response)) + 1];
  strcpy_P(responseR, reinterpret_cast<const char *>(response));
  return wait_for_response(responseR, time_out);
}

template<typename T>
void uGsm<T>::flush_the_serial_and_buffer()
{
  while (_serialGSM->available() > 0)
    _serialGSM->read();
  memset(buffer, '\0', 203);
}

template<typename T>
bool uGsm<T>::is_contain_response(const char *rsp)
{
  if (strstr(buffer, rsp) == NULL)
    return false;
  return true;
}

template<typename T>
bool uGsm<T>::is_contain_response(const __FlashStringHelper *rsp)
{
  char rsp_r[strlen_P(reinterpret_cast<const char *>(rsp)) + 1];
  strcpy_P(rsp_r, reinterpret_cast<const char *>(rsp));
  return is_contain_response(rsp_r);
}

// implementation of public methods
template<typename T>
void uGsm<T>::begin(Stream *_serial)
{
  _serialGSM = _serial;
  enableEcho();
  // set text mode for sms
  write_at_command(F("AT+CMGF=1\r"));
}

template<typename T>
void uGsm<T>::disableEcho()
{
  write_at_command(F("ATE0\r"));
  flush_the_serial_and_buffer();
}

template<typename T>
void uGsm<T>::enableEcho()
{
  write_at_command(F("ATE1\r"));
  flush_the_serial_and_buffer();
}

template<typename T>
bool uGsm<T>::isPoweredUp(uint16_t time_out)
{
  write_at_command(F("AT\r"));
  return wait_for_response(F("OK"), time_out);
}

template<typename T>
bool uGsm<T>::isRegistered(uint16_t time_out)
{
  write_at_command(F("AT+COPS?\r"));
  return wait_for_response(F("+COPS: 0,0"), time_out);
}

template<typename T>
bool uGsm<T>::sendSMS(const char *dst, const char *msg)
{
  // let's set text mode for sending message
  flush_the_serial_and_buffer();
  write_at_command(F("AT+CMGF=1\r"));
  if (!wait_for_response(F("OK"), 3000))
    return false;
  delay(20);
  // let's send message
  char sms_dst[20];
  sprintf_P(sms_dst, PSTR("AT+CMGS=\"%s\"\r"), dst);
  write_at_command(sms_dst);
  delay(20);
  if (wait_for_response(F(">"), 3000))
  {
    // DEBUG
    // Serial.println("DEBUG -- SEND MESSAGE");
    // write message
    const char *pChar = msg;
    while (*pChar != '\0')
    {
      _serialGSM->write(*pChar++);
    }

    // after that write end marker
    _serialGSM->write((char)26);
    delay(200);
    flush_the_serial_and_buffer();
    while (_serialGSM->available() == 0)
      ;
    return wait_for_response("+CMG", 3000);
  }
  else
  {
    return false;
  }
}

template<typename T>
bool uGsm<T>::sendSMS(const __FlashStringHelper *dst, const __FlashStringHelper *msg)
{
  char dst_r[strlen_P(reinterpret_cast<const char *>(dst)) + 1];
  char msg_r[strlen_P(reinterpret_cast<const char *>(msg)) + 1];

  strcpy_P(dst_r, reinterpret_cast<const char *>(dst));
  strcpy_P(msg_r, reinterpret_cast<const char *>(msg));

  return sendSMS(dst_r, msg_r);
}

// implementation of sim900a
template<typename T>
template<typename F>
typename enable_if<is_sim900a<F>::value, bool>::type uGsm<T>::messageToRead()
{
  if (_serialGSM->available() > 0)
  {
    char *pBuffer = read_buffer();
    const char *expct_rsp = "+CMTI: \"SM\",";
    if (strstr(pBuffer, expct_rsp) != NULL)
    {
      // new implementation for messageToRead - 24-12-2019 by Ahmed Alkabir
      char *found;
      for(uint8_t i=0; (found = strsep(&pBuffer, ",")) != NULL; i++){
        if(i==1){
          strncpy(last_message_index, found, strlen(found)-2);
          return true;
        }
      }
      // look for my funcking dump implementation
      // uint8_t len_rsp = strlen(expct_rsp);
      // uint8_t incrChar = 0;
      // char *pLastIndex = last_message_index;
      // while (*pBuffer != '\0')
      // {
      //   (*pBuffer++ == expct_rsp[incrChar]) ? incrChar++ : 0;
      //   if (incrChar == len_rsp)
      //   {
      //     // here i'm going to read the index of received message
      //     for (uint8_t i = 0; i < strlen(pBuffer); i++)
      //     {
      //       *pLastIndex++ = *pBuffer++;
      //     }
      //     *pLastIndex = '\0';
      //     // Serial.println(last_message_index);
      //     flush_the_serial_and_buffer();
      //     return true;
      //   }
      // }
    }
  }
  return false;
}

template<typename T>
template<typename F>
typename enable_if<is_sim900<F>::value, bool>::type uGsm<T>::messageToRead()
{
  // well, so far I know sim900 show detail of message as soon as received it
  // so we gonna save the detail of message to buffer
  if (_serialGSM->available())
  {
    // wait until we recieve all the buffer
    // I might change the value
    delay(100);
    // so we saved the message to buffer
    // how someone can read the disred message
    // just by calling readLastSMS method of SIM900
    char *p = buffer;
    while (_serialGSM->available())
    {
      *p++ = _serialGSM->read();
    }
    *p = '\0';
    return true;
  }
  return false;
}

template<typename T>
template<typename F>
typename enable_if<is_sim800l<F>::value, bool>::type uGsm<T>::messageToRead()
{
  return false;
}

template<typename T>
template<typename F>
typename enable_if<is_sim900a<F>::value, int>::type uGsm<T>::readLastSMS(char *phone_number, char **received_message)
{
  return readSMS(atoi(last_message_index), phone_number, received_message);
}

template<typename T>
template<typename F>
typename enable_if<is_sim900<F>::value, int>::type uGsm<T>::readLastSMS(char *phone_number, char **received_message)
{
  // so here we are going to parse the sms message
  char *pBuffer = buffer;
  char *found;
  for(uint8_t i=0; (found = strsep(&pBuffer, "\n")) != NULL; i++){
    // if(i==1){
    //   strncpy(last_message_index, found, strlen(found)-2);
    //   return true;
    // }
    if(i==2){
      *received_message = found;
      return 1;
    }
  }
}

template<typename T>
template<typename F>
typename enable_if<is_sim800l<F>::value, int>::type uGsm<T>::readLastSMS(char *phone_number, char **received_message)
{

}

// I need to reconsider the implementation
template<typename T>
int uGsm<T>::readSMS(uint8_t index_m, char *phone_number, char **received_message)
{
  char c_index_m[3];
  char cmd[12];
  itoa(index_m, c_index_m, 10);
  sprintf_P(cmd, PSTR("AT+CMGR=%s\r"), c_index_m);
  flush_the_serial_and_buffer();
  write_at_command(cmd);
  char *pBuffer = read_buffer();
  char *pPhone_number = phone_number;
  char *pReceived_message;
  bool phoneFetchedDone = false;
  while (*pBuffer != '\0')
  {
    // char ch = *pBuffer++;
    // start reading phone number
    if (*pBuffer++ == ',' && !phoneFetchedDone)
    {
      // ignore "
      *pBuffer++;
      while (*pBuffer != '"')
      {
        *pPhone_number++ = *pBuffer++;
      }
      *pPhone_number = '\0';
      phoneFetchedDone = true;
    }
    // let's wait for <CR>
    if (*pBuffer == '\r' && phoneFetchedDone)
    {
      // ignore <LF>
      *pBuffer++;
      *pBuffer++;
      pReceived_message = pBuffer;
      while (*pBuffer != '\0')
      {
        // break current loop
        if (*pBuffer++ == '\r')
          break;
      }
      // add the null character to state it as string literals
      *--pBuffer = '\0';
      *received_message = pReceived_message;
      // Serial.println(*received_message);
      // after we got the contents of message we stop the loop
      return 1;
    }
  }
  return 0;
}

template<typename T>
bool uGsm<T>::deleteSMS(uint8_t index_m)
{
  char cmd[10];
  sprintf_P(cmd, PSTR("AT+CMGD=%d\r"), index_m);
  write_at_command(cmd);
  return wait_for_response(F("OK"), 300);
}

template<typename T>
bool uGsm<T>::deleteAllSMS()
{
  write_at_command(F("AT+CMGD=0,4\r"));
  return wait_for_response(F("OK"), 3000);
}

#endif