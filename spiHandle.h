#ifndef SPI_HANDLE
#define SPI_HANDLE

#include <iostream>
#include <string>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/spi/spidev.h>
#include <errno.h>
#include <cstring> // for memset (do not use cstrings!)

using namespace std;

class spiHandle
{
public:
	spiHandle();
	~spiHandle();

	int initialize(const string devAddr,
			const unsigned char bitsPerWord = 8,
			unsigned int speed = 1000000);

	int readWrite(const unsigned char *transBuf, const int transLen, unsigned char *recBuf, const int recLen);

private:

	int dev;
	unsigned char mode;
	unsigned char wordBits;
	unsigned int dataRate;

	//gpioHandle addressPins;

};

#endif
