#ifndef BMX055_G_HANDLE
#define BMX055_G_HANDLE


//register constants
//gyroscope range in +-degrees per second
#define GYR_RANGE_2000D_S 0x00
#define GYR_RANGE_1000D_S 0x01
#define GYR_RANGE_500D_S 0x02
#define GYR_RANGE_250D_S 0x03
#define GYR_RANGE_125D_S 0x04

//gyroscope data filter bandwith
#define GYR_F_BANDWIDTH_32HZ 0x07
#define GYR_F_BANDWIDTH_64HZ 0x06
#define GYR_F_BANDWIDTH_12HZ 0x05
#define GYR_F_BANDWIDTH_23HZ 0x04
#define GYR_F_BANDWIDTH_47HZ 0x03
#define GYR_F_BANDWIDTH_116HZ 0x02
#define GYR_F_BANDWIDTH_230HZ 0x01
#define GYR_F_BANDWIDTH_523HZ 0x00
#define GYR_F_BANDWIDTH_UN 0x00

//fifo mode
#define GYR_FIFO_BYPASS 0x00
#define GYR_FIFO_FIFO 0x40
#define GYR_FIFO_STREAM 0x80

#define GYR_FIFODATA_XYZ 0x00
#define GYR_FIFODATA_X 0x01
#define GYR_FIFODATA_Y 0x02
#define GYR_FIFODATA_Z 0x03

//power mode
#define GYR_SLEEP_DUR_2MS 0x00
#define GYR_SLEEP_DUR_4MS 0x02
#define GYR_SLEEP_DUR_5MS 0x04
#define GYR_SLEEP_DUR_8MS 0x06
#define GYR_SLEEP_DUR_10MS 0x08
#define GYR_SLEEP_DUR_15MS 0x0a
#define GYR_SLEEP_DUR_18MS 0x0c
#define GYR_SLEEP_DUR_20MS 0x0e

#define GYR_NORMAL_POWER 0x00
#define GYR_DEEP_SUS_POWER 0x20
#define GYR_SUSPEND_POWER 0x80

//advanced power mode
#define GYR_FAST_PWRUP 0x80
#define GYR_PWR_SAVE_MODE 0x40

#define GYR_EXT_TRIG_NO 0x00
#define GYR_EXT_TRIG_INT1 0x10
#define GYR_EXT_TRIG_INT2 0x20
#define GYR_EXT_TRIG_SDO 0x30

#define GYR_AUTOSLEEP_DUR_4MS 0x01
#define GYR_AUTOSLEEP_DUR_5MS 0x02
#define GYR_AUTOSLEEP_DUR_8MS 0x03
#define GYR_AUTOSLEEP_DUR_10MS 0x04
#define GYR_AUTOSLEEP_DUR_15MS 0x05
#define GYR_AUTOSLEEP_DUR_20MS 0x06
#define GYR_AUTOSLEEP_DUR_40MS 0x07


class BMX055_G
{
public:

	struct gyrData
	{
		float XAxis;
		float YAxis;
		float ZAxis;
	};

	BMX055_G();
	~BMX055_G();

	int initialize(commsInterface *commsI);

	// CAUTION if setGyroRange fails, data readout may become unpredictable!
	int setGyroRange(const unsigned char range);
	int setGyroFilterBandwidth(const unsigned char bandwidth);

	int setDataOutputFormat(const unsigned char format);

	// returns contents of register 0x0E or -1 on failure
	int getFIFOStatus();
	// CAUTION if setFIFOConfig fails, datareadout may become unpredictable!
	int setFIFOConfig(const unsigned char config);

	// the fill level of the FIFO registers. returns -1 on failure
	int getFIFOFillStatus();
	int getFIFOOverrunStatus();

	int getFIFOData(gyrData *structPointer,const int length);

	//TODO refine check for invalid combinations
	int setPowermodeAndSleepDur(const unsigned char config);
	int setFastPwrConfig(const unsigned char config);


	// soft reset chip
	int reset();



private:
        //seperate definition for a read/write cylce for customizablity
	int readWrite(const unsigned char *transBuf, const int transLen,
			unsigned char *recBuf, const int recLen);

	commsInterface *commsInt;

	int FIFOOutputFormat;
	int lastPowerMode;
	float DegPerLSB;
};
#endif
