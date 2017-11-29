
#include "BMX055_ACCEL.h"

BMX055_A::BMX055_A()
{
	commsInt = nullptr;
	FIFOOutputFormat = ACC_FIFODATA_XYZ;
	lastPowerMode = ACC_NORMAL_POWER;
	GPerLSB = 0.00098f; // set initial range to +- 2G
}

BMX055_A::~BMX055_A()
{
	commsInt = nullptr;
}

int BMX055_A::initialize(commsInterface *commsI)
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
	if(readBuf[0] != 0xfa)
		return(-1);

	//res = reset();
	//if(res != 0)
	//	return(res);

	return(0);
}

int BMX055_A::setAccelRange(const unsigned char range)
{
	unsigned char transBuf[] = {(0b01111111 & 0x0F), (unsigned char)(range & 0x0F)};

	int res = readWrite(transBuf, 2, nullptr, 0);

	if(res != 0)
		return(res);

	unsigned char readBuf[1];
	readBuf[0] = 0;

	transBuf[0] = (0b10000000 | 0x0F);
	res = readWrite(transBuf, 1, readBuf, sizeof(readBuf));
	if(res != 0)
		return(res);

	if((readBuf[0] & 0x0F) != (range & 0x0F))
		return(-1);

	switch(range & 0x0F) // set G/least siginificant bit according to range (definitions from datasheet)
	{
	case(ACC_RANGE_2G):
		GPerLSB = 0.00098f;
		break;
	case(ACC_RANGE_4G):
		GPerLSB = 0.00195;
		break;
	case(ACC_RANGE_8G):
		GPerLSB = 0.00391;
		break;
	case(ACC_RANGE_16G):
		GPerLSB = 0.00781;
		break;
	default:
		return(-2);
	}

	return(0);
}

int BMX055_A::setAccelFilterBandwidth(const unsigned char bandwidth)
{
	unsigned char transBuf[] = {(0b01111111 & 0x10),(unsigned char)(bandwidth & 0x1F)};

	int res = readWrite(transBuf, 2, nullptr, 0);

	if(res != 0)
		return(res);

	unsigned char readBuf[1];
	readBuf[0] = 0;

	transBuf[0] = (0b10000000 | 0x10);
	res = readWrite(transBuf, 1, readBuf, sizeof(readBuf));
	if(res != 0)
		return(res);

	if((readBuf[0] & 0x1F) != (bandwidth & 0x1F))
		return(-1);

	return(0);
}

int BMX055_A::setDataOutputFormat(const unsigned char format)
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

int BMX055_A::getFIFOStatus()
{
	int res;

	const int readStart = 0x0D;
	unsigned char transBuf[1];
	unsigned char readBuf[1];
	transBuf[0] = 0b10000000 | readStart;

	res = readWrite(transBuf, 1, readBuf, sizeof(readBuf));
	if(res != 0)
		return(-1);

	return(readBuf[0]);
}

int BMX055_A::setFIFOConfig(const unsigned char config)
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

int BMX055_A::getFIFOFillStatus()
{
	int res;

	const int readStart = 0x0E;
	unsigned char transBuf[1];
	unsigned char readBuf[1];
	transBuf[0] = 0b10000000 | readStart;

	res = readWrite(transBuf, 1, readBuf, sizeof(readBuf));
	if(res != 0)
		return(-1);

	return(readBuf[0] & 0x7f);
}

int BMX055_A::getFIFOOverrunStatus()
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

int BMX055_A::getFIFOData(accData *structPointer,const int length)
{
	if(length < 1)
		return(-2);// return error if struct length is less than 1

	int arrLength;
	if(FIFOOutputFormat == ACC_FIFODATA_XYZ)
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
	case(ACC_FIFODATA_XYZ):
		for(int i = 0; i < length; i++)
		{
			int tempRes = ((readBuf[i*6] >> 4) & 0x0f) + (readBuf[(i*6)+1] << 4); // X

			if((readBuf[(i*6)+1] & 0x80) > 0) // number is negative
				tempRes |= ~(0xfff);

			structPointer[i].XAxis = tempRes * GPerLSB;


			tempRes = ((readBuf[(i*6)+2] >> 4) & 0x0f) + (readBuf[(i*6)+3] << 4); // Y

			if((readBuf[(i*6)+3] & 0x80) > 0) // number is negative
				tempRes |= ~(0xfff);

			structPointer[i].YAxis = tempRes * GPerLSB;


			tempRes = ((readBuf[(i*6)+4] >> 4) & 0x0f) + (readBuf[(i*6)+5] << 4); // Z

			if((readBuf[(i*6)+5] & 0x80) > 0) // number is negative
				tempRes |= ~(0xfff);

			structPointer[i].ZAxis = tempRes * GPerLSB;

		}
		break;

	case(ACC_FIFODATA_X):
		for(int i = 0; i < length; i++)
		{
			int tempRes = ((readBuf[i*2] >> 4) & 0x0f) + (readBuf[(i*2)+1] << 4);

			if((readBuf[(i*2)+1] & 0x80) > 0) // number is negative
				tempRes |= ~(0xfff);

			structPointer[i].XAxis = tempRes * GPerLSB;
			structPointer[i].YAxis = 0;
			structPointer[i].ZAxis = 0;
		}
		break;

	case(ACC_FIFODATA_Y):
		for(int i = 0; i < length; i++)
		{
			int tempRes = ((readBuf[i*2] >> 4) & 0x0f) + (readBuf[(i*2)+1] << 4);

			if((readBuf[(i*2)+1] & 0x80) > 0) // number is negative
				tempRes |= ~(0xfff);

			structPointer[i].XAxis = 0;
			structPointer[i].YAxis = tempRes * GPerLSB;
			structPointer[i].ZAxis = 0;
		}
		break;

	case(ACC_FIFODATA_Z):
		for(int i = 0; i < length; i++)
		{
			int tempRes = ((readBuf[i*2] >> 4) & 0x0f) + (readBuf[(i*2)+1] << 4);

			if((readBuf[(i*2)+1] & 0x80) > 0) // number is negative
				tempRes |= ~(0xfff);

			structPointer[i].XAxis = 0;
			structPointer[i].YAxis = 0;
			structPointer[i].ZAxis = tempRes * GPerLSB;
		}
		break;

	default:
		delete readBuf;
		return(-3);
	}
	delete readBuf;
	return(0);
}

int BMX055_A::getChipTemp()
{
	int res;

	const int readStart = 0x08;
	unsigned char transBuf[1];
	unsigned char readBuf[1];
	transBuf[0] = 0b10000000 | readStart;

	res = readWrite(transBuf, 1, readBuf, sizeof(readBuf));
	if(res != 0)
		return(-111);//error result much lower than lowest possible temperature

	return((signed char)readBuf[0] + 23); // return result + offset
}

int BMX055_A::setPowermodeAndSleepDur(const unsigned char config)
{

	int nextPwrMode = config & 0xE0;
	switch(lastPowerMode)
	{
	case(ACC_NORMAL_POWER):
		break;
	case(ACC_DEEP_SUS_POWER):
		if(nextPwrMode != ACC_NORMAL_POWER)
			return(-2);
	case(ACC_LOW_POWER):
		break; //TODO check which low power mode
	case(ACC_SUSPEND_POWER):
		break;
	}

	unsigned char transBuf[] = {(0b01111111 & 0x11),(unsigned char)(config & 0xFE)};

	int res = readWrite(transBuf, 2, nullptr, 0);

	if(res != 0)
		return(res);

	unsigned char readBuf[1];
	readBuf[0] = 0;

	transBuf[0] = (0b10000000 | 0x11);
	res = readWrite(transBuf, 1, readBuf, sizeof(readBuf));
	if(res != 0)
		return(res);

	if((readBuf[0] & 0xFE) != (config & 0xFE))
		return(-1);

	lastPowerMode = config & 0xE0;

	return(0);
}

int BMX055_A::setLowPowerConfig(const unsigned char config)
{

	//int nextPwrMode = config & 0xE0;
	switch(lastPowerMode)
	{
	case(ACC_NORMAL_POWER):
		break;
	case(ACC_DEEP_SUS_POWER):
		if(config & 0x20 > 0)
			return(-2);
	case(ACC_LOW_POWER):
		break; //TODO check which low power mode
	case(ACC_SUSPEND_POWER):
		if(config & 0x20 > 0)
			return(-2);
	}

	unsigned char transBuf[] = {(0b01111111 & 0x12),(unsigned char)(config & 0x60)};

	int res = readWrite(transBuf, 2, nullptr, 0);

	if(res != 0)
		return(res);

	unsigned char readBuf[1];
	readBuf[0] = 0;

	transBuf[0] = (0b10000000 | 0x12);
	res = readWrite(transBuf, 1, readBuf, sizeof(readBuf));
	if(res != 0)
		return(res);

	if((readBuf[0] & 0x60) != (config & 0x60))
		return(-1);

	if(config & 0x20 > 0)
		lastPowerMode = ACC_NORMAL_POWER;

	return(0);
}


int BMX055_A::reset()
{
	unsigned char transBuf[] = {(0b01111111 & 0x14), 0xB6};

	int res = readWrite(transBuf, 2, nullptr, 0);

	if(res != 0)
		return(res);

	return(0);
}

int BMX055_A::readWrite(const unsigned char *transBuf, const int transLen,
			unsigned char *recBuf, const int recLen)
{
	return(commsInt->readWriteAddr(transBuf, transLen, recBuf, recLen, 1)); // adress 1 = accelerometer
}
