/*
    SerialLub
    This is Lubby's interface control library.
    It is a protocol layer that sits on top of other serial communications and includes data packaging and handling.
    Compatible with any microcontroller after minor modifications.
    Currently configured for SPI <-> MCP2515 <-> CAN

*/
#ifndef SERIALLUB_H
#define SERIALLUB_

#include <SPI.h>
#include <cppQueue.h>
#include <AccelStepper.h>

#define RX_INT_PIN 1
#define STATE_QUEUE_LENGTH 10
#define CS 17

#define LED_PIN 0

#define MESSAGE_LENGTH 2

// MCP Masks and Filters
#define ALL_NODES_SID 0b11111111111
#define THIS_NODE 0b00000000001

#define RX_0_MASK_ADR_H 0x20             // mask address H
#define RX_0_MASK_ADR_L 0x21             // mask address L
#define RX_0_ALL_NODES_FILTER_ADR_H 0x00 // first Filter address H
#define RX_0_ALL_NODES_FILTER_ADR_L 0x01 // first Filter address L
#define RX_0_THIS_NODE_FILTER_ADR_H 0x04 // second Filter address H
#define RX_0_THIS_NODE_FILTER_ADR_L 0x05 // second Filter address L

// Mask = 0x7FF = 11 1's
// All Nodes SID = 0x7FF = 11 1's
// This Node SID = 0x01
#define RX_0_MASK_H 0b11111111             // mask value H
#define RX_0_MASK_L 0b11100000             // mask value L
#define RX_0_ALL_NODES_FILTER_H 0b11111111 // first Filter value H
#define RX_0_ALL_NODES_FILTER_L 0b11100000 // first Filter value L
#define RX_0_THIS_NODE_FILTER_H 0b00000000 // second Filter value H
#define RX_0_THIS_NODE_FILTER_L 0b00100000 // second Filter value L

// MCP Bit Rate
#define CNF1 0x2A
#define CNF2 0x29
#define CNF3 0x28

#define MCP_8MHz_125kBPS_CFG1 0x01
#define MCP_8MHz_125kBPS_CFG2 0xB1
#define MCP_8MHz_125kBPS_CFG3 0x85

// MCP interrupt
#define INT_ENABLE_ADR 0x2B
#define INT_ENABLE_VALUE 0x01 // interrupt when message received in RXB0

// MCP Modes
#define CANSTAT_REG_ADR 0x0E
#define CANCTRL_REG_ADR 0x0F
#define MCP_NORMAL_MODE 0x00

// MCP2515 CAN Registers and Commands
#define RX_BUF_0_H_READ_SPI_CMD 0b00 // nm -> 0x61
#define RX_BUF_0_L_READ_SPI_CMD 0b01 // nm -> 0x66
#define RX_BUF_1_H_READ_SPI_CMD 0b10 // nm -> 0x71
#define RX_BUF_1_L_READ_SPI_CMD 0b11 // nm -> 0x76
#define TX_BUF_0_H_LOAD_ADR 0b000    // abc -> 0x31
#define TX_BUF_0_L_LOAD_ADR 0b001    // abc -> 0x36
#define TX_BUF_1_H_LOAD_ADR 0b010    // abc -> 0x41
#define TX_BUF_1_L_LOAD_ADR 0b011    // abc -> 0x46
#define TX_BUF_2_H_LOAD_ADR 0b100    // abc -> 0x51
#define TX_BUF_2_L_LOAD_ADR 0b101    // abc -> 0x56
#define RESET 0b11000000             // 1100 0000
#define READ_REGISTER 0b00000011     // 0000 0011
#define READ_RX_BUF 0b10010000       // 1001 0nm0
#define READ_RX_BUF_0 0b10010010     // 1001 0010 start at RXB0D0
#define READ_RX_BUF_1 0b10010110     // 1001 0010 start at RXB0D0
#define WRITE_REGISTER 0b00000010    // 0000 0010
#define LOAD_TX_BUF 0b01000000       // 0100 0abc
#define REQUEST_TO_SEND 0b01000000   // 1000 0T2T1T0
#define BIT_MODIFY 0b00000101        // 0000 0101
#define READ_STATUS 0b01000000       // 1011 0000

extern uint8_t opmod;

// Stepper
extern float speed;
extern uint8_t position;

// LED
extern uint8_t brightness;

// Pattern

enum stateCodes
{
    WAIT,
    MESSAGE_RECEIVED,
    PARSE_AND_EXECUTE,
    SEND_RESPONSE,
    GOING_HOME
};
enum messageCommands
{
    STATUS_REPORT = 0b1001000,
    GO_HOME = 0b1001001,
    SET_SPEED = 0b1001011,
    SET_BRIGHTNESS = 0b1001100,
    GO_TO_POSITION = 0b1001101,
    SET_PATTERN = 0b1001110,
};

extern stateCodes currentState;
extern stateCodes nextState;
extern uint8_t message[MESSAGE_LENGTH];
extern uint16_t SID;

extern cppQueue stateQueue;

bool setupSerialLub(void);
// State Machine Functions
void canMsgReceived(void);
void readMcpRxB0(void);
void readMcpRxB1(void);
void parseAndExecute(void);

// MCP2515 CAN Functions
void resetMcp(void);
void setMcpMasksAndFilters(void);
void setBitRate125(void);
void setInterrupt(void);
void setNormalMode(void);
uint8_t getMcpStatus(void);

uint8_t readRegister(uint8_t);
void writeRegister(uint8_t, uint8_t);

// Stepper Motor Functions
void setStepperSpeed(uint8_t);
void setBrightness(uint8_t);
void goHome(void);
void goToPosition(uint8_t);
void setPattern(uint8_t);

void sendStatus(void);

#endif