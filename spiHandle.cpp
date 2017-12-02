#include <iostream>
//#include <string> //included in header
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/spi/spidev.h>
#include <errno.h>
#include <cstring> // for memset (do not use cstrings!)

#include "spiHandle.h"

using namespace std;

spiHandle::spiHandle()
{
	dev = -1;
}


spiHandle::~spiHandle()
{
	if(dev != -1)
		close(dev);
}

int spiHandle::initialize(const string devAddr,const unsigned char bitsPerWord, unsigned int speed)
{

	// set up parameters
	dev = open(devAddr.c_str(), O_RDWR);
	mode = SPI_MODE_0;
	wordBits = bitsPerWord;
	dataRate = speed;
	int statusVal;


	if(dev < 0)
	{
		cerr << "Error opening SPI port: (" << errno << ')' << endl;
		return(1);
	}

	statusVal = ioctl (dev, SPI_IOC_WR_MODE, &(mode));
	if(statusVal < 0){
		cerr << "Could not set SPIMode (WR)...ioctl fail: (" << errno << ')' << endl;
		return(2);
	}

	statusVal = ioctl (dev, SPI_IOC_RD_MODE, &(mode));
	if(statusVal < 0) {
		cerr << "Could not set SPIMode (RD)...ioctl fail: (" << errno << ')' << endl;
		return(3);
	}

	statusVal = ioctl (dev, SPI_IOC_WR_BITS_PER_WORD, &(wordBits));
	if(statusVal < 0) {
		cerr << "Could not set SPI bitsPerWord (WR)...ioctl fail: (" << errno << ')' << endl;
		return(4);
	}

	statusVal = ioctl (dev, SPI_IOC_WR_MAX_SPEED_HZ, &(dataRate));
	if(statusVal < 0) {
		cerr << "Could not set SPI speed (WR)...ioctl fail: (" << errno << ')' << endl;
		return(5);
	}

	statusVal = ioctl (dev, SPI_IOC_RD_MAX_SPEED_HZ, &(dataRate));
	if(statusVal < 0) {
		cerr << "Could not set SPI speed (RD)...ioctl fail: (" << errno << ')' << endl;
		return(6);
	}

	return(0);
}

int spiHandle::readWrite(const unsigned char *transBuf, const int transLen, unsigned char *recBuf, const int recLen)
{
	const int length = 2;
	struct spi_ioc_transfer spi[2];

	memset(&spi[0], 0, sizeof (spi[0]));
	memset(&spi[1], 0, sizeof (spi[1]));

	int res;

	if(transLen > 0 && recLen > 0)
	{
		spi[0].tx_buf        = (unsigned long)transBuf; // transmit from "transBuf"
		spi[0].rx_buf        = (unsigned int)NULL; // do not recive in this step
		spi[0].len           = transLen;
		spi[0].delay_usecs   = 0;
		spi[0].speed_hz      = dataRate;
		spi[0].bits_per_word = wordBits;
		spi[0].cs_change = 0;
		//spi[0].pad = 0;
		//spi[0].tx_nbits = 0;
		//spi[0].rx_nbits = 0;

		spi[1].tx_buf        = (unsigned long)NULL; // do not transmit in this step
		spi[1].rx_buf        = (unsigned int)recBuf; // receive into "recBuf"
		spi[1].len           = recLen;
		spi[1].delay_usecs   = 0;
		spi[1].speed_hz      = dataRate;
		spi[1].bits_per_word = wordBits;
		spi[1].cs_change = 0;
		//spi[1].pad = 0;
		//spi[1].tx_nbits = 0;
		//spi[1].rx_nbits = 0;
		res = ioctl(dev, SPI_IOC_MESSAGE(2), spi);
	}
	else
	{
		if(transLen > 0)
		{
			spi[0].tx_buf        = (unsigned long)transBuf; // transmit from "transBuf"
			spi[0].rx_buf        = (unsigned int)NULL; // do not recive in this step
			spi[0].len           = transLen;
		}
		else if(recLen > 0)
		{
			spi[0].tx_buf        = (unsigned long)NULL; // do not transmit in this step
			spi[0].rx_buf        = (unsigned int)recBuf; // receive into "recBuf"
			spi[0].len           = transLen;
		}
		else
		{
			cerr << "Problem transmitting spi data..no data defined!" << endl;
			return(1);
		}

		spi[0].delay_usecs   = 0;
		spi[0].speed_hz      = dataRate;
		spi[0].bits_per_word = wordBits;
		spi[0].cs_change = 0;
		//spi[0].pad = 0;
		//spi[0].tx_nbits = 0;
		//spi[0].rx_nbits = 0;
		res = ioctl(dev, SPI_IOC_MESSAGE(1), spi);
	}

	if(res < 0)
	{
		cerr << "Problem transmitting spi data..ioctl: " << errno << endl;
		return(-1);
	}

	return(0);
}

