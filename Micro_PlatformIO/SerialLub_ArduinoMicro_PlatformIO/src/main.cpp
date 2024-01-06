#include <Arduino.h>
#include "SerialLub.h"

// State Machine
cppQueue stateQueue(sizeof(currentState), STATE_QUEUE_LENGTH, FIFO);
stateCodes currentState = WAIT;
stateCodes nextState = WAIT;

// Communications
uint8_t message[MESSAGE_LENGTH] = {
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
	0x00,
};

// put function declarations here:
int myFunction(int, int);

// void canMsgReceived();

void setup()
{
	// Setup PC Comms
	Serial.begin(115200);

	// Setup SPI <-> CAN comms
	SPI.begin();
	SPI.setBitOrder(MSBFIRST);
	SPI.setClockDivider(SPI_CLOCK_DIV2); // 16MHz /2 = 8MHz
	SPI.setDataMode(SPI_MODE0);

	pinMode(CS, OUTPUT);
	digitalWrite(CS, HIGH);

	// Setup hardware
	pinMode(RX_INT_PIN, INPUT);
	pinMode(LED_BUILTIN, OUTPUT);
	attachInterrupt(digitalPinToInterrupt(RX_INT_PIN), canMsgReceived, FALLING);
	digitalWrite(LED_BUILTIN, LOW);

	// Setup state FIFO
	stateQueue.flush();
}

void loop()
{
	// check for new state
	if (!stateQueue.pop(&currentState)) {	// pop returns true if works, false if empty
		// if queue is empty
		currentState = WAIT;
	}

	// State Machine
	switch (currentState)
	{
	case WAIT:
		// Do nothing
		break;
	case MESSAGE_RECEIVED:
		// read message from MCP buffer to local buffer
		readMcpRxBuffer();
		break;
	case PARSE_DATA:

		break;
	case EXECUTE_COMMAND:

		break;
	case SEND_RESPONSE:

		break;
	}
}

// put function definitions here:
int myFunction(int x, int y)
{
	return x + y;
}
