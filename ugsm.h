#ifndef UGSM_H_
#define UGSM_H_

// mini library to work with sim900a specially
// created by Ahmed Alkabir
// well, I've to consider to echo property to library for better stability
// amm, let's disable echo
#include <Arduino.h>
#include <string.h>
#include <stdint.h>

class uGsm
{
public:
  uGsm(){}
  void begin(Stream *_serial);

  // to check if the arduino is connected to module
  bool isPoweredUp(uint16_t time_out);

  // to check if the module is registered to network
  bool isRegistered(uint16_t time_out);

  // to send message to specifc destenation
  bool sendSMS(const char *destenation, const char *message);
  bool sendSMS(const __FlashStringHelper *destenation, const __FlashStringHelper *message);
  // return true if there's income message to read
  bool messageToRead();
  void readLastSMS(char *phone_number, char *received_message);
  // void readSMS(char)
  bool deleteSMS(uint8_t index);
  bool deleteAllSMS();
  void disableEcho();
  void enableEcho();

  void test_responed_function();

private:
  Stream *_serialGSM = nullptr;
  char buffer[203];
  char last_message_index[3];
  // private methods
  void write_at_command(const char *cmd);
  void write_at_command(const __FlashStringHelper *cmd);
  void flush_the_serial_and_buffer();
  bool wait_for_response(char *response, uint16_t time_out);
  bool wait_for_response(const __FlashStringHelper *response, uint16_t time_out);
  char* read_buffer();
  bool is_contain_response(const char *response);
  bool is_contain_response(const __FlashStringHelper *response);
};

// implementation of private methods
char* uGsm::read_buffer(){
  if(_serialGSM->available() > 0){
    char ch;
    char *ptr = buffer;
    while(_serialGSM->available() > 0){
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

void uGsm::write_at_command(const char *cmd){
  flush_the_serial_and_buffer();
  uint8_t cmd_len = strlen(cmd);
  for(uint8_t i = 0; i < cmd_len; i++){
    _serialGSM->write(cmd[i]);
  }
  delay(500);
}

void uGsm::write_at_command(const __FlashStringHelper *cmd){
  char cmdR[strlen_P(reinterpret_cast<const char *>(cmd)) + 1];
  strcpy_P(cmdR, reinterpret_cast<const char *>(cmd));
  write_at_command(cmdR);
}

bool uGsm::wait_for_response(char *response, uint16_t time_out = 1000){
  uint16_t last_time = millis();
  while(1){
    const char *pBuffer = read_buffer();
    // uint8_t lenResponse = strlen(response);
    // uint8_t incrementalChar = 0;
    // while(*pBuffer != '\0'){
    //   (*pBuffer++ == response[incrementalChar]) ? incrementalChar++ : 0;
    //   if(incrementalChar == lenResponse){
    //     flush_the_serial_and_buffer();
    //     return true;
    //   }
    // }

    if(is_contain_response(response)){
      flush_the_serial_and_buffer();
      return true;
    }

    if(millis() - last_time > time_out){
      flush_the_serial_and_buffer();
      return false;
    }
  }
  return false;
}

bool uGsm::wait_for_response(const __FlashStringHelper *response, uint16_t time_out){
  char responseR[strlen_P(reinterpret_cast<const char *>(response)) + 1];
  strcpy_P(responseR, reinterpret_cast<const char *>(response));
  return wait_for_response(responseR, time_out);
}

void uGsm::flush_the_serial_and_buffer(){
  while(_serialGSM->available() > 0)
    _serialGSM->read();
  memset(buffer, '\0', 203);
}

bool uGsm::is_contain_response(const char *response){
  if(strstr(buffer, response) == NULL)
    return false;
  return true;
}

bool uGsm::is_contain_response(const __FlashStringHelper *response){
  char responseR[strlen_P(reinterpret_cast<const char *>(response)) + 1];
  strcpy_P(responseR, reinterpret_cast<const char *>(response));
  return is_contain_response(responseR);
}

// implementation of public methods
void uGsm::begin(Stream *_serial){
  _serialGSM = _serial;
  enableEcho();
}

void uGsm::disableEcho(){
  write_at_command(F("ATE0\r"));
  flush_the_serial_and_buffer();
}

void uGsm::enableEcho(){
  write_at_command(F("ATE1\r"));
  flush_the_serial_and_buffer();
}

bool uGsm::isPoweredUp(uint16_t time_out){
  write_at_command(F("AT\r"));
  return wait_for_response(F("OK"), time_out);
}

bool uGsm::isRegistered(uint16_t time_out){
  write_at_command(F("AT+COPS?\r"));
  return wait_for_response(F("+COPS: 0,0"), time_out);
}

bool uGsm::sendSMS(const char *destenation, const char *message){
  // let's set text mode for sending message
  flush_the_serial_and_buffer();
  write_at_command(F("AT+CMGF=1\r"));
  if(!wait_for_response(F("OK"), 3000))
    return false;
  delay(20);
  // let's send message
  char sms_destenation[20];
  sprintf(sms_destenation, "AT+CMGS=\"%s\"\r", destenation);
  write_at_command(sms_destenation);
  delay(20);
  if(wait_for_response(F(">"), 3000)){
    // DEBUG
    // Serial.println("DEBUG -- SEND MESSAGE");
    // write message 
    const char *pChar = message;
    while(*pChar != '\0'){
      _serialGSM->write(*pChar++);
    }

    // after that write end marker
    _serialGSM->write((char)26);
    delay(200);
    flush_the_serial_and_buffer();
    while(_serialGSM->available() == 0);
    return wait_for_response(F("+CMG"), 5000);
  }else{
    return false;
  }
}

bool uGsm::sendSMS(const __FlashStringHelper *destenation, const __FlashStringHelper *message){
  char destenationR[strlen_P(reinterpret_cast<const char *>(destenation)) + 1];
  char messageR[strlen_P(reinterpret_cast<const char *>(message)) + 1];

  strcpy_P(destenationR, reinterpret_cast<const char *>(destenation));
  strcpy_P(messageR, reinterpret_cast<const char *>(message));

  return sendSMS(destenationR, messageR);
}

bool uGsm::messageToRead(){
  if(_serialGSM->available() > 0){
    const char *expected_repsonse = "+CMTI: \"SM\",";
    const char *pBuffer = read_buffer();
    uint8_t lenResponse = strlen(expected_repsonse);
    uint8_t incrementalChar = 0;

    char *pLastIndex = last_message_index;
    while(*pBuffer != '\0'){
      (*pBuffer++ == expected_repsonse[incrementalChar]) ? incrementalChar++ : 0;
      if(incrementalChar == lenResponse){
        // here i'm going to read the index of received message
        for(uint8_t i=0; i < strlen(pBuffer); i++){
          *pLastIndex++ = *pBuffer++;
        }
        *pLastIndex = '\0';
        return true;
      }
    }
  }
  return false;
}

void uGsm::readLastSMS(char *phone_number, char *received_message){
  char command[12];
  // uint16_t last_time = millis();
  sprintf(command,"AT+CMGR=2\r", last_message_index);
  flush_the_serial_and_buffer();
  write_at_command(command);
  // char *pBuffer = read_buffer();
  // flush_the_serial_and_buffer();
  if(_serialGSM->available() > 0){
    char c;
    while (_serialGSM->available() > 0)
    {
      /* code */
      c = _serialGSM->read();
      Serial.write(c);
    }
    
  }
    if(_serialGSM->available() > 0){
    char c;
    while (_serialGSM->available() > 0)
    {
      /* code */
      c = _serialGSM->read();
      Serial.write(c);
    }
    
  }
  // Serial.println(pBuffer);
  char *pPhone_number = phone_number;
  char *pReceived_message = received_message;
  bool phoneFetchedDone = false;
  // while(*pBuffer != '\0'){
  //   // char ch = *pBuffer++;
  //   // start reading phone number
  //   if(*pBuffer++ == ',' && !phoneFetchedDone){
  //     // ignore "
  //     *pBuffer++;
  //     while(*pBuffer != '"'){
  //       *pPhone_number++ = *pBuffer++;
  //     }
  //     *pPhone_number = '\0';
  //     phoneFetchedDone = true;
  //   }
  //   // let's wait for <CR>
  //   // if(*pBuffer == '\r'){
  //   //   // ignore <LF>
  //   //   *pBuffer++;
  //   //   while(*pBuffer != '\r' || *pBuffer != '\n')
  //   //     *pReceived_message++ = *pBuffer++;
  //   //   *pReceived_message = '\0';
  //   //   // stop the loop
  //   //   break;
  //   // }
  // }
}

void uGsm::test_responed_function(){
  write_at_command("AT+COPS?\r");
  Serial.println(read_buffer());
}
#endif