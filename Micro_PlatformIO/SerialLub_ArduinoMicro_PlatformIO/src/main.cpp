#include <Arduino.h>
#include "SerialLub.h"

// State Machine
cppQueue stateQueue(sizeof(currentState), STATE_QUEUE_LENGTH, FIFO);
stateCodes currentState = WAIT;
stateCodes nextState = WAIT;

// Stepper
float speed = 0;
uint8_t position = 0;

// LED
uint8_t brightness = 0;

// Communications
uint8_t message[MESSAGE_LENGTH];

// put function declarations here:
int myFunction(int, int);

// void canMsgReceived();

void setup()
{
	// Setup PC Comms
	Serial.begin(115200);
	Serial.println("Serial Setup");

	// Setup SPI <-> CAN comms
	SPI.begin();
	SPI.setBitOrder(MSBFIRST);
	SPI.setClockDivider(SPI_CLOCK_DIV2); // 16MHz /2 = 8MHz
	SPI.setDataMode(SPI_MODE0);
	for (int i = 0; i < MESSAGE_LENGTH; i++)
	{ // initialize message array
		message[i] = 0x00;
	}
	Serial.println("SPI Setup");

	pinMode(CS, OUTPUT);
	digitalWrite(CS, HIGH);

	// Setup hardware
	pinMode(RX_INT_PIN, INPUT);
	pinMode(LED_BUILTIN, OUTPUT);
	attachInterrupt(digitalPinToInterrupt(RX_INT_PIN), canMsgReceived, FALLING);
	digitalWrite(LED_BUILTIN, LOW);

	// LED
	pinMode(LED_PIN, OUTPUT);
	analogWrite(LED_PIN, brightness);

	// CAN
	resetMcp();
	setMcpMasksAndFilters();
	setBitRate125();
	setInterrupt();
	setNormalMode();

	// Setup state FIFO
	stateQueue.flush();
}

void loop()
{
	Serial.print("Current State: ");
	Serial.println(currentState);
	// State Machine
	switch (currentState)
	{
	case WAIT:
		// check for new state
		if (!stateQueue.pop(&currentState))
		{ // pop returns true if works, false if empty
			// if queue is empty
			currentState = WAIT;
		}
		break;
	case MESSAGE_RECEIVED:
		// read message from MCP buffer to local buffer
		readMcpRxB0();
		break;
	case PARSE_AND_EXECUTE:
		// parse data by command
		parseAndExecute();

		break;
	case SEND_RESPONSE:
		sendStatus();
		break;
	case GOING_HOME:

		break;
	default:

		break;
	}
}

// put function definitions here:
int myFunction(int x, int y)
{
	return x + y;
}
