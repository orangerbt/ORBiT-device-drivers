#ifndef BMX055_M_HANDLE
#define BMX055_M_HANDLE


//register constants
//axis and pin settings
#define MAG_DATA_RDY_EN 0x80
#define MAG_INTERRUPT_EN 0x40
#define MAG_CHN_Z_DIS 0x20
#define MAG_CHN_Y_DIS 0x10
#define MAG_CHN_X_DIS 0x08
#define MAG_CHN_Z_EN 0x00
#define MAG_CHN_Y_EN 0x00
#define MAG_CHN_X_EN 0x00
#define MAG_DR_POL_ACTIVE_HIGH 0x04
#define MAG_DR_POL_ACTIVE_LOW 0x00
#define MAG_INT_LATCHED 0x02
#define MAG_INT_NON_LATCHED 0x00
#define MAG_INT_POL_ACTIVE_HIGH 0x01
#define MAG_INT_POL_ACTIVE_LOW 0x00

//repetitions settigs
//see datasheet for values

//power control settings
#define MAG_SLEEP 0x01
#define MAG_SUSPEND 0x00
#define MAG_3WIRE_SPI 0x04

//opperation settings
#define MAG_SELF_TEST 0x01
#define MAG_SELF_ADV_TEST_POS 0x40
#define MAG_SELF_ADV_TEST_NEG 0xc0
#define MAG_OUT_DATARATE_2HZ 0x08
#define MAG_OUT_DATARATE_6HZ 0x10
#define MAG_OUT_DATARATE_8HZ 0x18
#define MAG_OUT_DATARATE_15HZ 0x00
#define MAG_OUT_DATARATE_20HZ 0x28
#define MAG_OUT_DATARATE_25HZ 0x30
#define MAG_OUT_DATARATE_30HZ 0x38
#define MAG_NORMAL_MODE 0x00
#define MAG_FORCED_MODE 0x02
#define MAG_SLEEP_MODE 0x06


// INTERNAL DEFINITIONS DO NOT USE:
#define MAG_SUSPEND_MODE 0x01

class BMX055_M
{
public:

	struct magData // output format in mT
	{
		float XAxis;
		float YAxis;
		float ZAxis;
		bool overflow; // only with overflow interrupt set
	};

	BMX055_M();
	~BMX055_M();

	int initialize(commsInterface *commsI);


	int getData(magData *structPointer);

	int setControlSettings(const unsigned char config);
	int setOpperationSettings(const unsigned char config);

	int setRepititions(const unsigned char repetitionsXY, const unsigned char repetitionsZ);

	int setAxisAndPins(const unsigned char config);

	// soft reset chip (can also be done with setControlSettings)
	int reset();



	//TODO Add interupt control
	//TODO Check for power states

private:
        //seperate definition for a read/write cylce for customizablity
	int readWrite(const unsigned char *transBuf, const int transLen,
			unsigned char *recBuf, const int recLen);

	commsInterface *commsInt;

	int disabledAxes;
	int lastPowerMode;
};
#endif
