#ifndef SPI_HANDLE
#define SPI_HANDLE

#include <string>

class spiHandle
{
public:
	spiHandle();
	~spiHandle();

	int initialize(const std::string devAddr,
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
