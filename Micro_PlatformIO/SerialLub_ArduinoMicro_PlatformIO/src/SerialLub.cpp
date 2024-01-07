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

void readMcpRxB0()
{
    digitalWrite(CS, LOW); // start transfer

    SPI.transfer(READ_RX_BUF_0); // send READ RX BUFFER instruction
    for (int i = 0; i < MESSAGE_LENGTH; i++)
    {
        message[i] = SPI.transfer(0x00); // read data bytes
    }

    digitalWrite(CS, HIGH); // end transfer

    Serial.print("Message Read:");
    for (int i = 0; i < MESSAGE_LENGTH; i++)
    {
        Serial.print("\t");
        Serial.print(message[i], HEX);
    }
    Serial.println("");

    // nextState = PARSE_AND_EXECUTE; // set next state into queue
    nextState = WAIT; // set next state into queue
    stateQueue.push(&nextState);
}

void readMcpRxB1()
{
    digitalWrite(CS, LOW); // start transfer

    SPI.transfer(READ_RX_BUF_1); // send READ RX BUFFER instruction
    for (int i = 0; i < MESSAGE_LENGTH; i++)
    {
        message[i] = SPI.transfer(0x00); // read data bytes
    }

    digitalWrite(CS, HIGH); // end transfer and clear interrupt

    nextState = PARSE_AND_EXECUTE; // set next state into queue
    stateQueue.push(&nextState);
}

void parseAndExecute()
{
    uint8_t command = message[0];
    Serial.print("Parse and Execute Command:\t");
    Serial.println(command);
    switch (command)
    {
    case (STATUS_REPORT):
        // reply with status
        nextState = SEND_RESPONSE;
        stateQueue = stateQueue.push(&nextState);
        break;
    case (GO_HOME):
        // start going GO_HOME
        goHome();
        // reply when home
        nextState = SEND_RESPONSE;
        stateQueue = stateQueue.push(&nextState);
        break;
    case (SET_SPEED):
        setStepperSpeed(message[1]);
        break;
    case (SET_BRIGHTNESS):
        setStepperSpeed(message[1]);
        break;
    case (GO_TO_POSITION):
        goToPosition(message[1]);
        nextState = SEND_RESPONSE;
        stateQueue = stateQueue.push(&nextState);
        break;
    case (SET_PATTERN):
        setPattern(message[1]);
        break;
    default:
        // command not recognized
        break;
    }
}

void setStepperSpeed(uint8_t msgSpeedRPH)
{
    Serial.print("Setting Stepper Speed:\t");
    Serial.print(msgSpeedRPH);
    Serial.println(" revs per hour");
}

void setBrightness(uint8_t msgBrightness)
{
    Serial.print("Setting Brightness:\t");
    Serial.println(msgBrightness);

    if (msgBrightness > 255)
    {
        msgBrightness = 255;
    }
    brightness = msgBrightness;
    analogWrite(LED_PIN, brightness); // change later with patters
}

void goHome()
{
    Serial.println("Going Home");
}

void goToPosition(uint8_t msgPosition)
{
    Serial.print("Going to Position:\t");
    Serial.println(msgPosition);
}

void setPattern(uint8_t msgPattern)
{
    Serial.print("Setting Pattern:\t");
    Serial.println(msgPattern);
}

void sendStatus()
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
    Serial.print("Reading Register:\t");
    Serial.print(address, HEX);
    Serial.print("\t");
    Serial.println(value, BIN);

    return value;
}

void writeRegister(uint8_t address, uint8_t value)
{
    Serial.print("Writing Register:\t");
    Serial.print(address, HEX);
    Serial.print("\t");
    Serial.println(value, BIN);

    digitalWrite(CS, LOW);

    SPI.transfer(WRITE_REGISTER);
    SPI.transfer(address);
    SPI.transfer(value);

    digitalWrite(CS, HIGH);
}
void setMcpMasksAndFilters()
{
    Serial.println("Setting up Filters and Masks");

    // Set Mask
    writeRegister(RX_0_MASK_ADR_H, RX_0_MASK_H);
    writeRegister(RX_0_MASK_ADR_L, RX_0_MASK_L);

    // Set first Filter - All Nodes SID
    writeRegister(RX_0_ALL_NODES_FILTER_ADR_H, RX_0_ALL_NODES_FILTER_H);
    writeRegister(RX_0_ALL_NODES_FILTER_ADR_L, RX_0_ALL_NODES_FILTER_L);

    // Set second Filter - This Node 0x01
    writeRegister(RX_0_THIS_NODE_FILTER_ADR_H, RX_0_THIS_NODE_FILTER_H);
    writeRegister(RX_0_THIS_NODE_FILTER_ADR_L, RX_0_THIS_NODE_FILTER_L);
}

void setBitRate125()
{
    Serial.println("Setting up Bitrate 125kbps");

    // Set CNF1
    writeRegister(CNF1, MCP_8MHz_125kBPS_CFG1);

    // Set CNF2
    writeRegister(CNF2, MCP_8MHz_125kBPS_CFG2);

    // Set CNF3
    writeRegister(CNF3, MCP_8MHz_125kBPS_CFG3);
}

void setInterrupt()
{
    Serial.println("Setting up RX interrupt");

    writeRegister(INT_ENABLE_ADR, INT_ENABLE_VALUE);
}

void setNormalMode()
{
    Serial.println("Setting Normal Mode");

    writeRegister(CANCTRL_REG_ADR, MCP_NORMAL_MODE);
}
uint8_t getMcpStatus()
{
    Serial.println("Getting MCP Status");

    return readRegister(CANSTAT_REG_ADR);
}