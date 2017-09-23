#include "gpioHandle.h"


gpioHandle::gpioHandle()
{
}

gpioHandle::~gpioHandle()
{
}

//exports pin and sets mode
int gpioHandle::initializePin(const int pin, const int mode, const bool initialValue)
{
	return(0);
}

// changes pin mode
int gpioHandle::setPinMode(const int pin, const int mode, const bool initialValue)
{
	return(0);
}

// retuns -1 when pin is unusable, else returns 0
int gpioHandle::setPinVal(const int pin, const bool val)
{
	return(0);
}

// returns -1 when pin is unusable, else retuns value
int gpioHandle::getPinVal(const int pin)
{
	return(0);
}

// returns true if pin is usable
bool gpioHandle::pinOpen(const int pin)
{
	return(true);
}

// unexports pin
void gpioHandle::pinClose(const int pin)
{
	return;
}



