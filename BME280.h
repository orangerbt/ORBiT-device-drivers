#include <iostream>

#include "commsInterface.h"

using namespace std;

//*************** cntrl_meas constants ***************
//mode constants
#define SLEEP_MODE 0x00
#define FORCED_MODE 0x01
#define NORMAL_MODE 0x03

//oversample constants
// If you want to sample temperature or pressure, an oversample option MUST be used
#define PRESSURE_DISABLED 0x00
#define PRESSURE_X1_OVERSAMPLE 0x04
#define PRESSURE_X2_OVERSAMPLE 0x08
#define PRESSURE_X4_OVERSAMPLE 0x0C
#define PRESSURE_X8_OVERSAMPLE 0x10
#define PRESSURE_X16_OVERSAMPLE 0x14

#define TEMPERATURE_DISABLED 0x00
#define TEMPERATURE_X1_OVERSAMPLE 0x20
#define TEMPERATURE_X2_OVERSAMPLE 0x40
#define TEMPERATURE_X4_OVERSAMPLE 0x60
#define TEMPERATURE_X8_OVERSAMPLE 0x80
#define TEMPERATURE_X16_OVERSAMPLE 0xA0

//***************** config constants *****************
// spimode constant
#define DISABLE_3WIRE_SPI 0x00
#define ENAMBLE_3WIRE_SPI 0x01

// filter constants
#define FILTER_OFF 0x00
#define FILTER_X2 0x04
#define FILTER_X4 0x08
#define FILTER_X8 0x0C
#define FILTER_X16 0x14

//t_sb in milliseconds
#define T_SB_0_5 0x00
#define T_SB_62_5 0x20
#define T_SB_125 0x40
#define T_SB_250 0x60
#define T_SB_500 0x80
#define T_SB_1000 0xA0
#define T_SB_10 0xC0
#define T_SB_20 0xE0

class BME280
{
private:
	bool calibration;

public:

	// custom datatypes as defined by the BME280 datasheet
	typedef long signed int 	BME280_S32_t;
	typedef long unsigned int 	BME280_U32_t;
	typedef long long signed int 	BME280_S64_t;


	BME280();
	~BME280();

	int initialize(commsInterface *commsI);

	int setHumidityControl(const unsigned char config);
	// the chip doesn't submit setHumidityControl until setMeasurementConrol is called
	int setMeasurementControl(const unsigned char config);

	int setConfig(const unsigned char config);

	int reset();

	int takeMeasurement();

	int getMeasurementResults();

	float getLastHumidity(); //last measured humidity in %RH
	float getLastTemperature(); //last measured temperature in degC
	int getLastPressure(); // last measured pressure in Pa

private:

	//seperate definition for a read/write cylce for costomizablity
	int readWrite(const unsigned char *transBuf, const int transLen,
			unsigned char *recBuf, const int recLen);

	int getCalibrationData();

	// requred since this register will be used by takeMeasurement
	unsigned char lastMeasurementControl;

	BME280_S32_t lastTemperature;
	BME280_S32_t lastPressure;
	BME280_S32_t lastHumidity;

	unsigned short 	dig_T1;
	signed short 	dig_T2;
	signed short 	dig_T3;

	unsigned short 	dig_P1;
	signed short 	dig_P2;
	signed short 	dig_P3;
	signed short 	dig_P4;
	signed short 	dig_P5;
	signed short 	dig_P6;
	signed short 	dig_P7;
	signed short 	dig_P8;
	signed short 	dig_P9;

	unsigned char 	dig_H1;
	signed short 	dig_H2;
	unsigned char 	dig_H3;
	signed short 	dig_H4;
	signed short 	dig_H5;
	signed char 	dig_H6;

	commsInterface *commsInt;
};

