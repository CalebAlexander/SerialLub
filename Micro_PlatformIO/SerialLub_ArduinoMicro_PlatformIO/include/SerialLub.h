/*
    SerialLub
    This is Lubby's interface control library.
    It is a protocol layer that sits on top of other serial communications and includes data packaging and handling.
    Compatible with any microcontroller after minor modifications.
    Currently configured for SPI <-> MCP2515 <-> CAN

*/
#ifndef SERIALLUB_H
#define SERIALLUB_H

#include <SPI.h>
#include <cppQueue.h>

#define RX_INT_PIN 1
#define STATE_QUEUE_LENGTH 10
#define CS 17

#define MESSAGE_LENGTH 8

// MCP2515 CAN Registers and Commands
const uint8_t RX_BUF_0_H_READ_ADR = 0b00; // nm -> 0x61
const uint8_t RX_BUF_0_L_READ_ADR = 0b01; // nm -> 0x66
const uint8_t RX_BUF_1_H_READ_ADR = 0b10; // nm -> 0x71
const uint8_t RX_BUF_1_L_READ_ADR = 0b11; // nm -> 0x76

const uint8_t TX_BUF_0_H_LOAD_ADR = 0b000; // abc -> 0x31
const uint8_t TX_BUF_0_L_LOAD_ADR = 0b001; // abc -> 0x36
const uint8_t TX_BUF_1_H_LOAD_ADR = 0b010; // abc -> 0x41
const uint8_t TX_BUF_1_L_LOAD_ADR = 0b011; // abc -> 0x46
const uint8_t TX_BUF_2_H_LOAD_ADR = 0b100; // abc -> 0x51
const uint8_t TX_BUF_2_L_LOAD_ADR = 0b101; // abc -> 0x56

const uint8_t RESET = 0b11000000;           // 1100 0000
const uint8_t READ_REGISTER = 0b00000011;   // 0000 0011
const uint8_t READ_RX_BUF = 0b10010000;     // 1001 0nm0
const uint8_t WRITE_REGISTER = 0b00000010;  // 0000 0010
const uint8_t LOAD_TX_BUF = 0b01000000;     // 0100 0abc
const uint8_t REQUEST_TO_SEND = 0b01000000; // 1000 0T2T1T0
const uint8_t BIT_MODIFY = 0b00000101;      // 0000 0101
const uint8_t READ_STATUS = 0b01000000;     // 1011 0000

const uint8_t CANSTAT_REG_ADR = 0x0E;
const uint8_t CANCTRL_REG_ADR = 0x0F;

extern uint8_t opmod;



enum stateCodes
{
    WAIT,
    MESSAGE_RECEIVED,
    PARSE_DATA,
    EXECUTE_COMMAND,
    SEND_RESPONSE
};

extern stateCodes currentState;
extern stateCodes nextState;
extern uint8_t message[MESSAGE_LENGTH];

extern cppQueue stateQueue;

bool setupSerialLub(void);
// State Machine Functions
void canMsgReceived(void);
void readMcpRxBuffer(void);


// MCP2515 CAN Functions
void resetMcp(void);
uint8_t readRegister(uint8_t);
void writeRegister(uint8_t, uint8_t);

#endif