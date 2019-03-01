#include "ugsm.h"

bool uGsm::isConnected()
{
    if (AT())
        return true;
    else
        return false;
}

int uGsm::available(void)
{
    if (_serial)
        return _serial->available();
    return -1;
}

bool uGsm::isListening(void)
{
    if (_serial)
        return _serial->isListening();
    return false;
}

bool uGsm::listen(void)
{
    if (_serial)
        return _serial->listen();
    return false;
}

void uGsm::flush(void)
{
    if (_serial)
        _serial->flush();
}

size_t uGsm::write(const char c)
{
    if (_serial)
        return _serial->write(c);
    return 0;
}

size_t uGsm::write(const char str[])
{
    if (_serial)
        _serial->write(str);
    return 0;
}

size_t uGsm::write(const __FlashStringHelper *str)
{
    char buffer[strlen_P((const char *)str) + 1];
    strcpy_P(buffer, (const char *)str);
    return write(buffer);
}

size_t uGsm::readBytes(char buffer[], size_t length)
{
    if (_serial)
        return _serial->readBytes(buffer, length);
    return 0;
}

// TODO: Complete this function
// main purpose to clear buffer from the serial instance
void uGsm::clearBufferFromSerial(void)
{
    if (!isListening())
        listen();

    auto t = millis();
    if (_serial)
    {
        while (_serial->available() == 0 && millis() - t < 50)
            ;
        if (_serial->available() == 0)
            return;
    }
}

// is going to send at command to gsm900a
// and as I stated it won't turn back echo for now
void uGsm::ATSendCommand(const char *at)
{
    if (!isListening())
        listen();
    if (_serial)
        _serial->print(at);
    // as echo is enabled as default mode
    // so I guess, I have to deal with it
}

void uGsm::ATSendCommand(const __FlashStringHelper *at)
{
    char buffer[strlen_PF((const char *)at) + 1];
    strcpy_P(buffer, (const char *)at);
    return ATSendCommand(buffer);
}

// Get AT Response, this function is block call until timeout.
bool uGsm::atResponse(char *buffer, size_t bufferSize, unsigned long timeout)
{
    // let's make sure, there's data to parse
    // TODO: Add timout feature
    while (true)
    {
        if (_serial->available() > 0)
            break;
    }

    uint8_t i = 0;
    while (true)
    {
        while (_serial->available() > 0)
        {
            buffer[i++] = _serial->read();
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

bool uGsm::atResponse(unsigned long timeout = 50)
{
    return atResponse(response, sizeof(response), timeout);
}

bool uGsm::isResponse(const char *expected)
{
    if (strcmp(response, expected) != 0)
        return false;
    return true;
}

bool uGsm::isResponse(const __FlashStringHelper *expected)
{
    char buffer[strlen_PF((const char *)expected) + 1];
    strcpy_P(buffer, (const char *)expected);
    return isResponse(buffer);
}

bool uGsm::isContain(const char *expected)
{
    if (strstr(response, expected) == NULL)
        return false;
    return true;
}

bool uGsm::isContain(const __FlashStringHelper *expected)
{
    char buffer[strlen_PF((const char *)expected) + 1];
    strcpy_P(buffer, (const char *)expected);
    return isContain(buffer);
}

bool uGsm::isOK(void)
{
    if (!(isResponse(F("OK")) || isResponse(F("OK\r"))))
        return false;
    if (available())
        clearBufferFromSerial();
    return true;
}

bool uGsm::AT(void)
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

void uGsm::callNumber(const char *destination)
{
    const __FlashStringHelper *command = F("ATD%s;\r");
    char buffer[8 + strlen(destination)];
    sprintf_P(buffer, (const char *)command, destination);
    ATSendCommand(buffer);
}

bool uGsm::sendMessage(const char *destination, const char *message)
{
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

bool uGsm::sendMessage(const __FlashStringHelper *destination, const __FlashStringHelper *message){
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
bool uGsm::readSMS(void)
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

bool uGsm::doCommand(const char *command, void (*func)())
{
    if (isContain(command))
    {
        func();
        return true;
    }
    return false;
}

bool uGsm::doCommand(const __FlashStringHelper *command, void (*func)()){
    if(isContain(command)){
        func();
        return true;
    }
    return false;
}
