
#include "commsInterface.h"


commsInterface::commsInterface()
{
	addressPins = nullptr;
	curAddress = -1;
}

commsInterface::~commsInterface()
{

	if(numPins != 0) // debug
	{
		for(int i = 0; i < numPins; i++)
		{
			pinClose(addressPins[i]);
		}
	}

	if(addressPins != nullptr) // dealocate memory
	{
		delete addressPins;
	}
}

int commsInterface::initialize(const string devAddr, const int numberOfPins,
				const int *addressPinArray,
				const unsigned char bitsPerWord,
				unsigned int speed)
{

	// initialize spi bus
	int res = spiHandle::initialize(devAddr, bitsPerWord, speed);
	if(res != 0)
	{
		return(res);
	}

	// initialize pins
	if(numberOfPins > 0)
	{
		int *addrArray = new int[numberOfPins];

		for(int i = 0; i < numberOfPins; i++)
		{
			res = initializePin(addressPinArray[i], PINMODE_OUT); // open pins in output mode
			if(res != 0)
			{
				delete addrArray;
				return(7);
			}
			addrArray[i] = addressPinArray[i]; // make sure to copy pin values
		}

		addressPins = addrArray;
		numPins = numberOfPins;
	}

	return(0);
}

int commsInterface::setChipSelectAddr(int value)
{
	if(curAddress == value) // change pinouts only if address is different
		return(0);

	for(int i = 0; i < numPins; i++)
	{
		if(value & (1 << i)) // set pin high if the ith position of value is a 1
		{
			if(setPinVal(addressPins[i], true) != 0)
				return(1);
		}
		else // else keep pin low
		{
			if(setPinVal(addressPins[i], false) != 0)
				return(1);
		}
	}
	curAddress = value;
	return(0);
}
int commsInterface::readWriteAddr(const unsigned char *transBuf, const int transLen,
				unsigned char *recBuf, const int recLen, const int address)
{
	if(setChipSelectAddr(address) != 0)
		return(7);

	int res = readWrite(transBuf, transLen, recBuf, recLen);

	if(res != 0)
		return(res);

	return(0);
}
