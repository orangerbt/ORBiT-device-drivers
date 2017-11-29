#ifndef BMX055_A_HANDLE
#define BMX055_A_HANDLE

#include <iostream>
#include <bitset>

#include "commsInterface.h"

using namespace std;

//register constants
//accelerometer range in +-g
#define ACC_RANGE_2G 0x03
#define ACC_RANGE_4G 0x05
#define ACC_RANGE_8G 0x08
#define ACC_RANGE_16G 0x0b

//accelerometer data filter bandwith
#define ACC_F_BANDWIDTH_7_81HZ 0x08
#define ACC_F_BANDWIDTH_15_63HZ 0x09
#define ACC_F_BANDWIDTH_31_25HZ 0x0a
#define ACC_F_BANDWIDTH_62_5HZ 0x0b
#define ACC_F_BANDWIDTH_125HZ 0x0c
#define ACC_F_BANDWIDTH_250HZ 0x0d
#define ACC_F_BANDWIDTH_500HZ 0x0e
#define ACC_F_BANDWIDTH_1000HZ 0x0f

//data output format
#define ACC_UNFILTERED_OUTPUT 0x80
#define ACC_FILTERED_OUTPUT 0x00
#define ACC_SHADOW_DISABLED 0x40
#define ACC_SHADOW_ENABLED 0x00

//fifo mode
#define ACC_FIFO_BYPASS 0x00
#define ACC_FIFO_FIFO 0x40
#define ACC_FIFO_STREAM 0x80

#define ACC_FIFODATA_XYZ 0x00
#define ACC_FIFODATA_X 0x01
#define ACC_FIFODATA_Y 0x02
#define ACC_FIFODATA_Z 0x03

//power mode
#define ACC_SLEEP_DUR_0_5MS 0x0a
#define ACC_SLEEP_DUR_1MS 0x0c
#define ACC_SLEEP_DUR_2MS 0x0e
#define ACC_SLEEP_DUR_4MS 0x10
#define ACC_SLEEP_DUR_6MS 0x12
#define ACC_SLEEP_DUR_10MS 0x14
#define ACC_SLEEP_DUR_25MS 0x16
#define ACC_SLEEP_DUR_50MS 0x18
#define ACC_SLEEP_DUR_100MS 0x1a
#define ACC_SLEEP_DUR_500MS 0x1c
#define ACC_SLEEP_DUR_1S 0x1e

#define ACC_NORMAL_POWER 0x00
#define ACC_DEEP_SUS_POWER 0x20
#define ACC_LOW_POWER 0x40
#define ACC_SUSPEND_POWER 0x80

//low power mode
#define ACC_LOWPOWER_MODE1 0x00
#define ACC_LOWPOWER_MODE2 0x40
#define ACC_SLEEPTIMER_MODE 0x20
#define ACC_NO_SLEEPTIMER_MODE 0x00

class BMX055_A
{
public:

	struct accData
	{
		float XAxis;
		float YAxis;
		float ZAxis;
	};

	BMX055_A();
	~BMX055_A();

	int initialize(commsInterface *commsI);

	// CAUTION if setAccelRange fails, data readout may become unpredictable!
	int setAccelRange(const unsigned char range);
	int setAccelFilterBandwidth(const unsigned char bandwidth);

	int setDataOutputFormat(const unsigned char format);

	// returns contents of register 0x0E or -1 on failure
	int getFIFOStatus();
	// CAUTION if setFIFOConfig fails, datareadout may become unpredictable!
	int setFIFOConfig(const unsigned char config);

	// the fill level of the FIFO registers. returns -1 on failure
	int getFIFOFillStatus();
	int getFIFOOverrunStatus();

	int getFIFOData(accData *structPointer,const int length);

	int getChipTemp(); // returns -111 on error


	//TODO refine check for invalid combinations
	int setPowermodeAndSleepDur(const unsigned char config);
	int setLowPowerConfig(const unsigned char config);


	// soft reset chip
	int reset();


	//TODO Refine power mode management
	//TODO Add interupt engines
	//TODO Add self test
	//TODO Add control for the few-time non-volatile memory
	//TODO Add digital interface control (I2C bus/ 3wire spi)
	//TODO Add offsets
	//TODO Add intterupts
	//TODO Add getting offset values
	//TODO Add direct data getting

private:
        //seperate definition for a read/write cylce for customizablity
	int readWrite(const unsigned char *transBuf, const int transLen,
			unsigned char *recBuf, const int recLen);

	commsInterface *commsInt;

	int FIFOOutputFormat;
	int lastPowerMode;
	float GPerLSB;
};
#endif

