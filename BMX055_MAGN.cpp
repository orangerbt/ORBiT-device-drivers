#include <iostream>
#include <bitset>
#include <unistd.h>

#include "commsInterface.h"

#include "BMX055_MAGN.h"

using namespace std;

BMX055_M::BMX055_M()
{
	commsInt = nullptr;
	disabledAxes = 0; //axes are active low - All enabled by default
	lastPowerMode = MAG_SUSPEND_MODE;
}

BMX055_M::~BMX055_M()
{
	commsInt = nullptr;
}

int BMX055_M::initialize(commsInterface *commsI)
{
	commsInt = commsI;

	//TODO make sure setControlSettings can run while in 3pin spi mode
	//(maybe make another function instead?)

	setControlSettings(MAG_SLEEP); // boot up chip to access ID


	const int readStart = 0x40;
	unsigned char transBuf[1];
	unsigned char readBuf[1];

	transBuf[0] = 0b10000000 | readStart;

	usleep(10000);

	int res = readWrite(transBuf, 1, readBuf, sizeof(readBuf));
	if(res != 0)
		return(res);


	//cout << (int)readBuf[0] << endl;
	//identification is incorrect
	if(readBuf[0] != 0x32)
		return(-1);

	return(0);
}

int BMX055_M::setControlSettings(const unsigned char config)
{
	unsigned char transBuf[] = {(0b01111111 & 0x4B), (unsigned char)(config & 0x87)};

	int res = readWrite(transBuf, 2, nullptr, 0);

	if(res != 0)
		return(res);

	unsigned char readBuf[1];
	readBuf[0] = 0;

	transBuf[0] = (0b10000000 | 0x4B);
	res = readWrite(transBuf, 1, readBuf, sizeof(readBuf));
	if(res != 0)
		return(res);

	if((readBuf[0] & 0x87) != (config & 0x87))
		return(-1);


	return(0);
}

int BMX055_M::setOpperationSettings(const unsigned char config)
{
	unsigned char transBuf[] = {(0b01111111 & 0x4C), config};

	int res = readWrite(transBuf, 2, nullptr, 0);

	if(res != 0)
		return(res);

	unsigned char readBuf[1];
	readBuf[0] = 0;

	transBuf[0] = (0b10000000 | 0x4C);
	res = readWrite(transBuf, 1, readBuf, sizeof(readBuf));
	if(res != 0)
		return(res);

	if(readBuf[0] != config)
		return(-1);

	return(0);
}

int BMX055_M::setRepititions(const unsigned char repetitionsXY, const unsigned char repetitionsZ)
{
	unsigned char transBuf[2] = {(0b01111111 & 0x51), repetitionsXY};

	int res = readWrite(transBuf, 2, nullptr, 0);

	if(res != 0)
		return(res);

	transBuf[0] = (0b01111111 & 0x52);
	transBuf[1] = repetitionsZ;

	res = readWrite(transBuf, 2, nullptr, 0);

	if(res != 0)
		return(res);


	unsigned char readBuf[2];
	readBuf[0] = 0;
	readBuf[1] = 0;

	transBuf[0] = (0b10000000 | 0x51);
	res = readWrite(transBuf, 1, readBuf, sizeof(readBuf));
	if(res != 0)
		return(res);

	if(readBuf[0] != repetitionsXY || readBuf[1] != repetitionsZ)
		return(-1);

	return(0);
}

int BMX055_M::setAxisAndPins(const unsigned char config)
{
	unsigned char transBuf[] = {(0b01111111 & 0x4E), config};

	int res = readWrite(transBuf, 2, nullptr, 0);

	if(res != 0)
		return(res);

	unsigned char readBuf[1];
	readBuf[0] = 0;

	transBuf[0] = (0b10000000 | 0x4E);
	res = readWrite(transBuf, 1, readBuf, sizeof(readBuf));
	if(res != 0)
		return(res);

	if(readBuf[0] != config)
		return(-1);

	disabledAxes = (config & 0x38) >> 3;



	return(0);
}

int BMX055_M::getData(magData *structPointer)
{
	int res;
	const int readStart = 0x42;
	unsigned char transBuf[1];
	unsigned char readBuf[9];
	transBuf[0] = 0b10000000 | readStart;

	res = readWrite(transBuf, 1, readBuf, 9);
	if(res != 0)
		return(-1);

	structPointer->overflow = (readBuf[8] & 0x40) > 0;

	int rHall = (readBuf[7] * 0x40) + ((readBuf[6] & 0xfc) >> 2);

	int tempRes;

	structPointer->XAxis = 0;
	structPointer->YAxis = 0;
	structPointer->ZAxis = 0;


	if(!(lastPowerMode & MAG_CHN_X_DIS))
	{
		tempRes = (readBuf[1] * 0x20) + ((readBuf[0] & 0xf8) >> 3);
		if((readBuf[1] & 0x80) > 0) // number is negative
			tempRes |= ~(0xfff);

		structPointer->XAxis = (tempRes / (float)(1<<16))*1000;
	}

	if(!(lastPowerMode & MAG_CHN_Y_DIS))
	{
		tempRes = (readBuf[3] * 0x20) + ((readBuf[2] & 0xf8) >> 3);
		if((readBuf[3] & 0x80) > 0) // number is negative
			tempRes |= ~(0xfff);

		structPointer->YAxis = (tempRes / (float)(1<<16))*1000;
	}

	if(!(lastPowerMode & MAG_CHN_Z_DIS))
	{
		tempRes = (readBuf[5] * 0x80) + ((readBuf[4] & 0xfe) >> 1);
		if((readBuf[5] & 0x80) > 0) // number is negative
			tempRes |= ~(0x3fff);

		structPointer->ZAxis = (tempRes / (float)(1<<16))*1000;
	}

	return(0);
}

int BMX055_M::reset()
{
	unsigned char transBuf[] = {(0b01111111 & 0x4B), 0x82};

	int res = readWrite(transBuf, 2, nullptr, 0);

	if(res != 0)
		return(res);

	return(0);
}

int BMX055_M::readWrite(const unsigned char *transBuf, const int transLen,
			unsigned char *recBuf, const int recLen)
{
	return(commsInt->readWriteAddr(transBuf, transLen, recBuf, recLen, 3)); // adress 3 = gyroscope
}
