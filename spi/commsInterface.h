#ifndef COMMS_INTERFACE
#define COMMS_INTERFACE

#include <string>

#include "spiHandle.h"
#include "gpioHandle.h"

class commsInterface: private gpioHandle, private spiHandle
{
public:

	commsInterface();
	~commsInterface();

	int initialize(const std::string devAddr,
			const int numberOfPins = 0,
			const int *addressPinArray = nullptr,
			const unsigned char bitsPerWord = 8,
			unsigned int speed = 10000000);

	int setChipSelectAddr(int value);

	int readWriteAddr(const unsigned char *transBuf, const int transLen,
			unsigned char *recBuf, const int recLen, const int address);

private:

	int numPins;
	int *addressPins;
	int curAddress;
};

#endif
