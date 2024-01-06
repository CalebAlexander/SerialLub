#include "SerialLub.h"

bool setupSerialLub()
{
    return false;
}

void canMsgReceived()
{
    if (digitalRead(RX_INT_PIN) == LOW)
    {
        // Message is ready in RX Buffer of CAN Module
        nextState = MESSAGE_RECEIVED;
        stateQueue.push(&nextState);
    }
}

void readMcpRxBuffer()
{
}

void resetMcp()
{
    digitalWrite(CS, LOW);

    SPI.transfer(RESET);

    digitalWrite(CS, HIGH);
}

uint8_t readRegister(uint8_t address)
{
    uint8_t value = 0b0;

    digitalWrite(CS, LOW);

    SPI.transfer(READ_REGISTER);
    SPI.transfer(address);
    value = SPI.transfer(0x00);

    digitalWrite(CS, HIGH);

    return value;
}

void writeRegister(uint8_t address, uint8_t value)
{

    digitalWrite(CS, LOW);

    SPI.transfer(WRITE_REGISTER);
    SPI.transfer(address);
    SPI.transfer(value);

    digitalWrite(CS, HIGH);
}