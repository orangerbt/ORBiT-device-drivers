#include <iostream>
#include <bitset>

#include "commsInterface.h"

#include "BMX055_GYRO.h"

using namespace std;

BMX055_G::BMX055_G()
{
	commsInt = nullptr;
	FIFOOutputFormat = GYR_FIFODATA_XYZ;
	lastPowerMode = GYR_NORMAL_POWER;
	DegPerLSB = 0.0038f; // set initial range to +- 125Deg/s
}

BMX055_G::~BMX055_G()
{
	commsInt = nullptr;
}

int BMX055_G::initialize(commsInterface *commsI)
{
	commsInt = commsI;

	const int readStart = 0x00;
	unsigned char transBuf[1];
	unsigned char readBuf[1];

	transBuf[0] = 0b10000000 | readStart;

	int res = readWrite(transBuf, 1, readBuf, sizeof(readBuf));
	if(res != 0)
		return(res);

	//identification is incorrect
	if(readBuf[0] != 0x0f)
		return(-1);

	//res = reset();
	//if(res != 0)
	//	return(res);

	return(0);
}

int BMX055_G::setGyroRange(const unsigned char range)
{
	unsigned char transBuf[] = {(0b01111111 & 0x0F), (unsigned char)(range & 0x07)};

	int res = readWrite(transBuf, 2, nullptr, 0);

	if(res != 0)
		return(res);

	unsigned char readBuf[1];
	readBuf[0] = 0;

	transBuf[0] = (0b10000000 | 0x0F);
	res = readWrite(transBuf, 1, readBuf, sizeof(readBuf));
	if(res != 0)
		return(res);

	if((readBuf[0] & 0x07) != (range & 0x07))
		return(-1);

	switch(range & 0x07) // set deg/sec/least siginificant bit according to range (definitions from datasheet)
	{
	case(GYR_RANGE_2000D_S):
		DegPerLSB = 0.061f;
		break;
	case(GYR_RANGE_1000D_S):
		DegPerLSB = 0.0305f;
		break;
	case(GYR_RANGE_500D_S):
		DegPerLSB = 0.0153f;
		break;
	case(GYR_RANGE_250D_S):
		DegPerLSB = 0.0076f;
		break;
	case(GYR_RANGE_125D_S):
		DegPerLSB = 0.0038f;
		break;
	default:
		return(-2);
	}

	return(0);
}

int BMX055_G::setGyroFilterBandwidth(const unsigned char bandwidth)
{
	unsigned char transBuf[] = {(0b01111111 & 0x10),(unsigned char)(bandwidth & 0x0f)};

	int res = readWrite(transBuf, 2, nullptr, 0);

	if(res != 0)
		return(res);

	unsigned char readBuf[1];
	readBuf[0] = 0;

	transBuf[0] = (0b10000000 | 0x10);
	res = readWrite(transBuf, 1, readBuf, sizeof(readBuf));
	if(res != 0)
		return(res);

	if((readBuf[0] & 0x0F) != (bandwidth & 0x0F))
		return(-1);

	return(0);
}

int BMX055_G::setDataOutputFormat(const unsigned char format)
{
	unsigned char transBuf[] = {(0b01111111 & 0x13), (unsigned char)(format & 0xc0)};

	int res = readWrite(transBuf, 2, nullptr, 0);

	if(res != 0)
		return(res);

	unsigned char readBuf[1];
	readBuf[0] = 0;

	transBuf[0] = (0b10000000 | 0x13);
	res = readWrite(transBuf, 1, readBuf, sizeof(readBuf));
	if(res != 0)
		return(res);

	if((readBuf[0] & 0xc0) != (format & 0xc0))
		return(-1);

	return(0);
}

int BMX055_G::getFIFOStatus()
{
	int res;

	const int readStart = 0x0E;
	unsigned char transBuf[1];
	unsigned char readBuf[1];
	transBuf[0] = 0b10000000 | readStart;

	res = readWrite(transBuf, 1, readBuf, sizeof(readBuf));
	if(res != 0)
		return(-1);

	return(readBuf[0]);
}

int BMX055_G::setFIFOConfig(const unsigned char config)
{
	unsigned char transBuf[] = {(0b01111111 & 0x3E), (unsigned char)(config & 0xc3)};

	int res = readWrite(transBuf, 2, nullptr, 0);

	if(res != 0)
		return(res);

	unsigned char readBuf[1];
	readBuf[0] = 0;

	transBuf[0] = (0b10000000 | 0x3E);
	res = readWrite(transBuf, 1, readBuf, sizeof(readBuf));
	if(res != 0)
		return(res);

	if((readBuf[0] & 0xc3) != (config & 0xc3))
		return(-1);

	FIFOOutputFormat = config & 0x03; //remember output setting for fifo readout

	return(0);
}

int BMX055_G::getFIFOFillStatus()
{
	int res;

	const int readStart = 0x0E;
	unsigned char transBuf[1];
	unsigned char readBuf[1];
	transBuf[0] = 0b10000000 | readStart;

	res = readWrite(transBuf, 1, readBuf, sizeof(readBuf));
	if(res != 0)
		return(-1);

	return(readBuf[0] & 0xef);
}

int BMX055_G::getFIFOOverrunStatus()
{
	int res;

	const int readStart = 0x0E;
	unsigned char transBuf[1];
	unsigned char readBuf[1];
	transBuf[0] = 0b10000000 | readStart;

	res = readWrite(transBuf, 1, readBuf, sizeof(readBuf));
	if(res != 0)
		return(-1);

	if((readBuf[0] & 0x80) > 0)
		return(1);
	else
		return(0);
}

int BMX055_G::getFIFOData(gyrData *structPointer,const int length)
{
	if(length < 1)
		return(-2);// return error if struct length is less than 1

	int arrLength;
	if(FIFOOutputFormat == GYR_FIFODATA_XYZ)
		arrLength = length * 6; // 3 axies and 2 registers per axis
	else
		arrLength = length * 2; // just one 2 register axis

	int res;
	const int readStart = 0x3F;
	unsigned char transBuf[1];
	unsigned char *readBuf = new unsigned char[arrLength];
	transBuf[0] = 0b10000000 | readStart;

	res = readWrite(transBuf, 1, readBuf, arrLength);
	if(res != 0)
	{
		delete readBuf;
		return(-1);
	}

	switch(FIFOOutputFormat)
	{
	case(GYR_FIFODATA_XYZ):
		for(int i = 0; i < length; i++)
		{
			short int tempRes = readBuf[i*6] + (readBuf[(i*6)+1] << 8); // X
			structPointer[i].XAxis = tempRes * DegPerLSB;

			tempRes = readBuf[(i*6)+1] + (readBuf[(i*6)+3] << 8); // Y
			structPointer[i].YAxis = tempRes * DegPerLSB;

			tempRes = readBuf[(i*6)+4] + (readBuf[(i*6)+5] << 8); // Z
			structPointer[i].ZAxis = tempRes * DegPerLSB;

		}
		break;

	case(GYR_FIFODATA_X):
		for(int i = 0; i < length; i++)
		{
			short int tempRes = readBuf[i*2] + (readBuf[(i*2)+1] << 8);

			structPointer[i].XAxis = tempRes *DegPerLSB;
			structPointer[i].YAxis = 0;
			structPointer[i].ZAxis = 0;
		}
		break;

	case(GYR_FIFODATA_Y):
		for(int i = 0; i < length; i++)
		{
			short int tempRes = readBuf[i*2] + (readBuf[(i*2)+1] << 8);

			structPointer[i].XAxis = 0;
			structPointer[i].YAxis = tempRes * DegPerLSB;
			structPointer[i].ZAxis = 0;
		}
		break;

	case(GYR_FIFODATA_Z):
		for(int i = 0; i < length; i++)
		{
			short int tempRes = readBuf[i*2] + (readBuf[(i*2)+1] << 8);

			structPointer[i].XAxis = 0;
			structPointer[i].YAxis = 0;
			structPointer[i].ZAxis = tempRes * DegPerLSB;
		}
		break;

	default:
		delete readBuf;
		return(-3);
	}
	delete readBuf;
	return(0);
}

int BMX055_G::setPowermodeAndSleepDur(const unsigned char config)
{

	int nextPwrMode = config & 0xa0;
	switch(lastPowerMode)
	{
	case(GYR_NORMAL_POWER):
		break;
	case(GYR_DEEP_SUS_POWER):
		if(nextPwrMode != GYR_NORMAL_POWER)
			return(-2);
	case(GYR_SUSPEND_POWER):
		break;
	}

	unsigned char transBuf[] = {(0b01111111 & 0x11),(unsigned char)(config & 0xae)};

	int res = readWrite(transBuf, 2, nullptr, 0);

	if(res != 0)
		return(res);

	unsigned char readBuf[1];
	readBuf[0] = 0;

	transBuf[0] = (0b10000000 | 0x11);
	res = readWrite(transBuf, 1, readBuf, sizeof(readBuf));
	if(res != 0)
		return(res);

	if((readBuf[0] & 0xae) != (config & 0xae))
		return(-1);

	lastPowerMode = config & 0xa0;

	return(0);
}

int BMX055_G::setFastPwrConfig(const unsigned char config)
{

	//int nextPwrMode = config & 0xE0;
	switch(lastPowerMode)
	{
	case(GYR_DEEP_SUS_POWER):
		if(config & 0x80 > 0)
			return(-2);
	}

	unsigned char transBuf[] = {(0b01111111 & 0x12),(unsigned char)(config & 0xfe)};

	int res = readWrite(transBuf, 2, nullptr, 0);

	if(res != 0)
		return(res);

	unsigned char readBuf[1];
	readBuf[0] = 0;

	transBuf[0] = (0b10000000 | 0x12);
	res = readWrite(transBuf, 1, readBuf, sizeof(readBuf));
	if(res != 0)
		return(res);

	if((readBuf[0] & 0xfe) != (config & 0xfe))
		return(-1);

	if(config & 0x80 > 0)
		lastPowerMode = GYR_NORMAL_POWER;

	return(0);
}


int BMX055_G::reset()
{
	unsigned char transBuf[] = {(0b01111111 & 0x14), 0xB6};

	int res = readWrite(transBuf, 2, nullptr, 0);

	if(res != 0)
		return(res);

	return(0);
}

int BMX055_G::readWrite(const unsigned char *transBuf, const int transLen,
			unsigned char *recBuf, const int recLen)
{
	return(commsInt->readWriteAddr(transBuf, transLen, recBuf, recLen, 2)); // adress 2 = gyroscope
}
