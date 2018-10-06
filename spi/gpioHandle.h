#ifndef GPIO_HANDLE
#define GPIO_HANDLE

#include <iostream>
#include <fstream>

using namespace std;

// pinmode definitions
#define PINMODE_OUT	0
#define PINMODE_IN	1

class gpioHandle
{
public:

	gpioHandle();
	~gpioHandle(); // unexport all pins opened by this class only

	int initializePin(const int pin, const int mode, const bool initialValue = 0); //exports pin and sets mode

	int setPinMode(const int pin, const int mode, const bool initialValue = 0); // changes pin mode

	int setPinVal(const int pin, const bool val); // retuns -1 when pin is unusable, else returns 0
	int getPinVal(const int pin); // returns -1 when pin is unusable, else retuns value

	bool pinOpen(const int pin); // returns true if pin is usable

	int pinClose(const int pin); //unexports pin

private:
	char *buffer;	// for sprintfs
};

#endif
