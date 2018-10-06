#include <iostream>

#include "commsInterface.h"

#include "BME280.h"

using namespace std;


BME280::BME280()
{
	commsInt = nullptr;
	lastMeasurementControl = 0;
}

BME280::~BME280()
{
	commsInt = nullptr;
	calibration = false;
}

int BME280::initialize(commsInterface *commsI)
{
	commsInt = commsI;

	const int readStart = 0xD0;
	unsigned char transBuf[1];
	unsigned char readBuf[1];

	transBuf[0] = 0b10000000 | readStart;

	int res = readWrite(transBuf, 1, readBuf, sizeof(readBuf));
	if(res != 0)
		return(res);

	if(readBuf[0] != 0x60)
		return(-1);

	res = getCalibrationData();
	if(res != 0)
		return(res);
}

int BME280::setHumidityControl(const unsigned char config)
{
	unsigned char transBuf[] = {(0b01111111 & 0xF2), config};

	int res = readWrite(transBuf, 2, nullptr, 0);

	if(res != 0)
		return(res);

	unsigned char readBuf[1];
	readBuf[0] = 0;

	transBuf[0] = 0b10000000 | 0xF2;
	res = readWrite(transBuf, 1, readBuf, sizeof(readBuf));
	if(res != 0)
		return(res);

	if(readBuf[0] != config)
	      return(-1);

	return(0);
}

int BME280::setMeasurementControl(const unsigned char config)
{
	unsigned char transBuf[] = {(0b01111111 & 0xF4), config};

	int res = readWrite(transBuf, 2, nullptr, 0);

	if(res != 0)
		return(res);

	unsigned char readBuf[1];
	readBuf[0] = 0;

	transBuf[0] = 0b10000000 | 0xF4;
	res = readWrite(transBuf, 1, readBuf, sizeof(readBuf));
	if(res != 0)
		return(res);

	lastMeasurementControl = config;
	if(readBuf[0] != config)
		return(-1);

	return(0);
}

int BME280::setConfig(const unsigned char config)
{
	unsigned char transBuf[] = {(0b01111111 & 0xF5), config};

	int res = readWrite(transBuf, 2, nullptr, 0);

	if(res != 0)
		return(res);

	unsigned char readBuf[1];
	readBuf[0] = 0;

	transBuf[0] = 0b10000000 | 0xF5;
	res = readWrite(transBuf, 1, readBuf, sizeof(readBuf));
	if(res != 0)
		return(res);

	if(readBuf[0] != config)
		return(-1);

	return(0);
}

int BME280::reset()
{
	unsigned char transBuf[] = {(0b01111111 & 0xE0), 0xB6};

	int res = readWrite(transBuf, 2, nullptr, 0);

	if(res != 0)
		return(res);
	return(0);
}

int BME280::takeMeasurement()
{
	int res;
	if(calibration == false)
	{
		res = getCalibrationData();
		if(res != 0)
			return(res);
	}

	unsigned char tempControl = lastMeasurementControl;
	res = setMeasurementControl((lastMeasurementControl & 0b1111100) | FORCED_MODE);
	lastMeasurementControl = tempControl;
	if(res != 0)
	{
		return(res);
	}

	return(0);
}

int BME280::getMeasurementResults()
{
	int res;
	if(calibration == false)
	{
		res = getCalibrationData();
		if(res != 0)
			return(res);
	}

	const int readStart = 0xF3;
	unsigned char transBuf[1];
	unsigned char readBuf[120];

	transBuf[0] = 0b10000000 | readStart;// 0xF3;

	res = readWrite(transBuf, 1, readBuf, sizeof(readBuf));
	if(res != 0)
		return(res);

	// the following compensation code was adapted from the BME280 datasheet.
	BME280_S32_t adc_T = (readBuf[0xFA - readStart] << 12)
				+ (readBuf[0xFB - readStart] << 4)
				+ (readBuf[0xFC - readStart] >> 4);

	BME280_S32_t adc_P = (readBuf[ 0xF7 - readStart] << 12)
				+ (readBuf[0xF8 - readStart] << 4)
				+ (readBuf[0xF9 - readStart] >> 4);

	BME280_S32_t adc_H = (readBuf[0xFD - readStart] << 8) + readBuf[0xFE - readStart];

	BME280_S32_t t_fine;

	//Temperature
	BME280_S32_t varT1, varT2;
	varT1 = ((((adc_T>>3) - ((BME280_S32_t)dig_T1<<1))) * ((BME280_S32_t)dig_T2)) >> 11;
	varT2 = (((((adc_T>>4) - ((BME280_S32_t)dig_T1)) * ((adc_T>>4) - ((BME280_S32_t)dig_T1))) >> 12) *
	((BME280_S32_t)dig_T3)) >> 14;
	t_fine = varT1 + varT2;
	lastTemperature = (t_fine * 5 + 128) >> 8;

	//Pressure
	BME280_S64_t varP1, varP2, pressRes;
	varP1 = ((BME280_S64_t)t_fine) - 128000;
	varP2 = varP1 * varP1 * (BME280_S64_t)dig_P6;
	varP2 = varP2 + ((varP1*(BME280_S64_t)dig_P5)<<17);
	varP2 = varP2 + (((BME280_S64_t)dig_P4)<<35);
	varP1 = ((varP1 * varP1 * (BME280_S64_t)dig_P3)>>8) + ((varP1 * (BME280_S64_t)dig_P2)<<12);
	varP1 = (((((BME280_S64_t)1) << 47)+varP1))*((BME280_S64_t)dig_P1)>>33;
	if (varP1 == 0)// avoid exception caused by division by zero
	{
		lastPressure = 0;
	}
	else
	{
		pressRes = 1048576 - adc_P;
		pressRes = (((pressRes<<31)-varP2)*3125)/varP1;
		varP1 = (((BME280_S64_t)dig_P9) * (pressRes>>13) * (pressRes>>13)) >> 25;
		varP2 = (((BME280_S64_t)dig_P8) * pressRes) >> 19;
		pressRes = ((pressRes + varP1 + varP2) >> 8) + (((BME280_S64_t)dig_P7)<<4);
		lastPressure = (BME280_U32_t)pressRes;
	}

	//Humidity
	BME280_S32_t v_x1_u32r;
	v_x1_u32r = (t_fine - ((BME280_S32_t)76800));
	v_x1_u32r = (((((adc_H << 14) - (((BME280_S32_t)dig_H4) << 20) - (((BME280_S32_t)dig_H5) * v_x1_u32r)) +
		((BME280_S32_t)16384)) >> 15) * (((((((v_x1_u32r * ((BME280_S32_t)dig_H6)) >> 10) * (((v_x1_u32r *
		((BME280_S32_t)dig_H3)) >> 11) + ((BME280_S32_t)32768))) >> 10) + ((BME280_S32_t)2097152)) *
		((BME280_S32_t)dig_H2) + 8192) >> 14));
	v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((BME280_S32_t)dig_H1)) >> 4));
	v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
	v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
	lastHumidity = (BME280_U32_t)(v_x1_u32r>>12);

	//cout << (lastTemperature * 0.01f) << "C\t"
	//	<< (lastPressure/256.0f) << "Pa\t"
	//	<< (lastHumidity/1024.0f) << "%RH" << endl;
	//cout << "---------------------------------------" << endl;

	if((readBuf[0xF3 - readStart] & 0x09) != 0) // measurement had not completed
		return(8);

	return(0);
}

int BME280::getCalibrationData()
{

	int res;

	const int readStart = 0x88;
	unsigned char transBuf[1];
	unsigned char readBuf[120];

	transBuf[0] = 0b10000000 | readStart;

	res = readWrite(transBuf, 1, readBuf, sizeof(readBuf));
	if(res != 0)
		return(res);


	dig_T1 = (readBuf[0x89 - readStart] << 8) + readBuf[0x88 - readStart];
	dig_T2 = (readBuf[0x8B - readStart] << 8) + readBuf[0x8A - readStart];
	dig_T3 = (readBuf[0x89 - readStart] << 8) + readBuf[0x88 - readStart];

	dig_P1 = (readBuf[0x8F - readStart] << 8) + readBuf[0x8E - readStart];
	dig_P2 = (readBuf[0x91 - readStart] << 8) + readBuf[0x90 - readStart];
	dig_P3 = (readBuf[0x93 - readStart] << 8) + readBuf[0x92 - readStart];
	dig_P4 = (readBuf[0x95 - readStart] << 8) + readBuf[0x94 - readStart];
	dig_P5 = (readBuf[0x97 - readStart] << 8) + readBuf[0x96 - readStart];
	dig_P6 = (readBuf[0x99 - readStart] << 8) + readBuf[0x98 - readStart];
	dig_P7 = (readBuf[0x9B - readStart] << 8) + readBuf[0x9A - readStart];
	dig_P8 = (readBuf[0x9D - readStart] << 8) + readBuf[0x9C - readStart];
	dig_P9 = (readBuf[0x9F - readStart] << 8) + readBuf[0x9E - readStart];

	dig_H1 = readBuf[0xA1 - readStart];
	dig_H2 = (readBuf[0xE2 - readStart] << 8) + readBuf[0xE1 - readStart];
	dig_H3 = readBuf[0xE3 - readStart];
	dig_H4 = (readBuf[0xE4 - readStart] << 4) + (readBuf[0xE5 - readStart] & 0x0f);
	dig_H5 = ((readBuf[0xE5 - readStart] & 0xf0) >> 4) + (readBuf[0xE6 - readStart] << 4);
	dig_H6 = readBuf[0xE7 - readStart];

	calibration = true;

	return(0);
}

float BME280::getLastHumidity()
{
	return(lastHumidity/1024.0f);
}

float BME280::getLastTemperature()
{
	return(lastTemperature*0.01f);
}

int BME280::getLastPressure()
{
	return(lastPressure/256.0f);
}

int BME280::readWrite(const unsigned char *transBuf, const int transLen,
			unsigned char *recBuf, const int recLen)
{
	return(commsInt->readWriteAddr(transBuf, transLen, recBuf, recLen, 0)); // address of 0 = BME
}
