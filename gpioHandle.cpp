#include "gpioHandle.h"
#include <cstdio>
#include <unistd.h>

//#define DEBUG

gpioHandle::gpioHandle()
{
	buffer = new char[256];
}

gpioHandle::~gpioHandle()
{
	delete[] buffer;
}

//exports pin and sets mode
int gpioHandle::initializePin(const int pin, const int mode, const bool initialValue)
{
	ofstream exportPin;
	exportPin.open("/sys/class/gpio/export");
	if(!exportPin.is_open())
	{
		cout << "Error exporting pin " << pin << endl;
		return (-1);
	}

	exportPin << pin;
	exportPin.close();

	ofstream pinMode;
	sprintf(buffer,"/sys/class/gpio/gpio%d/direction", pin);
	int loop = 0;
	while(loop < 100)
	{
		pinMode.open(buffer);
		if(pinMode.is_open())
			break;
		else
		{
			loop++;
			usleep(1000); // wait 1ms for fs to catch up
		}
	}
	if(!pinMode.is_open())
	{
		cout << "Error setting mode for pin " << pin << endl;
		return (-1);
	}
	
	if (PINMODE_OUT)
	{
		pinMode <<  "out";
	}
	else if (PINMODE_IN)
	{
       		pinMode <<  "in";
	}
	else
	{
		cout << "ERROR!!! \n";
		return (-1);
	}
	pinMode.close();

#ifdef DEBUG
	cout << "Pin " << pin << " initialized to mode ";
	switch(mode)
	{
	case(PINMODE_OUT):
		cout << "OUTPUT";
		break;
	case(PINMODE_IN):
		cout << "INPUT";
		break;
	default:
		cout << "INVALID PINMODE";
		break;
	}
	cout << " with an intial value of " << initialValue << endl;
#endif

	return(0);
}

// changes pin mode
int gpioHandle::setPinMode(const int pin, const int mode, const bool initialValue)
{
	ofstream pinMode;
	sprintf(buffer,"/sys/class/gpio/gpio%d/direction",pin);
	pinMode.open(buffer);
	if(!pinMode.is_open())
	{
		cout << "Error! \n";
		return (-1);
	}

	if (PINMODE_OUT) {
	pinMode <<  "out";
	}
	else {
	if (PINMODE_IN) {
       			pinMode <<  "in";
		}
		else {
			cout << "ERROR!!! \n";
			return (-1);
		}
	}
	pinMode.close();

#ifdef DEBUG
	cout << "Pin " << pin << " set to mode ";
	switch(mode)
	{
	case(PINMODE_OUT):
		cout << "OUTPUT";
		break;
	case(PINMODE_IN):
		cout << "INPUT";
		break;
	default:
		cout << "INVALID PINMODE";
		break;
	}
	cout << " with an intial value of " << initialValue << endl;
#endif

	return(0);
}

// retuns -1 when pin is unusable, else returns 0
int gpioHandle::setPinVal(const int pin, const bool val)
{
	ofstream setVal;
	sprintf(buffer,"/sys/class/gpio/gpio%d/value",pin);
	setVal.open(buffer);
	if(!setVal.is_open())
	{
		cout << "Error! \n";
		return (-1);
	}
	else {
		return(0);
	}
	setVal.close();

#ifdef DEBUG
	cout << "Pin " << pin << " set to " << val << endl;
#endif
}

// returns -1 when pin is unusable, else retuns value
int gpioHandle::getPinVal(const int pin)
{
	ifstream getVal;
	sprintf(buffer,"/sys/class/gpio/gpio%d/value",pin);
	getVal.open(buffer);
	if(!getVal.is_open())
	{
		cout << "Error!" << endl;
		return (-1);
	}
	int val;
	getVal >> val;
	getVal.close();

	return(val);
}

 // returns true if pin is usable
bool gpioHandle::pinOpen(const int pin)
{
	ofstream usePin;
	sprintf(buffer,"/sys/class/gpio/gpio%d",pin);
	usePin.open(buffer);
	if(usePin.is_open())
	{
		cout << "The pin " << pin << " is usable!";
		return (true);
	}
	else {
		cout << "The pin " << pin <<" is unusable!";
		return(false);
	}
	usePin.close();

#ifdef DEBUG
	cout << "Pin " << pin << " opened!" << endl;
#endif
}

// unexports pin
int gpioHandle::pinClose(const int pin)
{
	ofstream exportPin;
	exportPin.open("/sys/class/gpio/unexport");
	if(!exportPin.is_open())
	{
		cout << "Error! \n";
		return (-1);
	}
	exportPin << pin;
	exportPin.close();
#ifdef DEBUG
	cout << "Pin " << pin << " closed!" << endl;
#endif
}
