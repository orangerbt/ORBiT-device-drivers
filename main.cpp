#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/time.h>

//#include "spiHandle.h"
//#include "gpioHandle.h"
#include "commsInterface.h"
#include "BME280.h"
#include "BMX055_ACCEL.h"
#include "BMX055_GYRO.h"
#include "BMX055_MAGN.h"

using namespace std;

#define deviceAddr "/dev/spidev1.0"

//#define DEBUG_GYRO
//#define DEBUG_ACCEL
//#define DEBUG_ATMO
#define DEBUG_MAGN


int main(int argc, char* argv[])
{
	commsInterface commsI;

	int addressPins[] = {61, 86, 88};
	int res = commsI.initialize(deviceAddr, 3, addressPins);
	if(res != 0)
	{
		cout << "Error: (" << res << ')' << endl;
		return(1);
	}

	int tempCount = 0;

#ifdef DEBUG_GYRO
	BMX055_G bmx055_Gyro;

	res = bmx055_Gyro.initialize(&commsI);
	if(res != 0)
	{
		cout << "BMX055_G initialize error: (" << res << ')' << endl;
		return(-1);
	}

	if(argc > 1)
	{
		if(strcmp(argv[1], "-r") == 0)
		{
			cout << "reset chip!" << endl;
			bmx055_Gyro.reset();
			return(0);
		}
	}

	res = bmx055_Gyro.setGyroRange(GYR_RANGE_250D_S);
	if(res != 0)
		cout << "BMX055_G range set error: (" << res << ')' << endl;

	res = bmx055_Gyro.setGyroFilterBandwidth(GYR_F_BANDWIDTH_32HZ);
	if(res != 0)
		cout << "BMX055_G bandwidth set error: (" << res << ')' << endl;

	res = bmx055_Gyro.setFIFOConfig(GYR_FIFO_FIFO | GYR_FIFODATA_XYZ);
	if(res != 0)
		cout << "BMX055_G FIFO config set 2 error: (" << res << ')' << endl;

	res = bmx055_Gyro.setPowermodeAndSleepDur(GYR_SLEEP_DUR_20MS | GYR_NORMAL_POWER);
	if(res != 0)
		cout << "BMX055_G powermode set error: (" << res << ')' << endl;

	res = bmx055_Gyro.setFastPwrConfig(GYR_AUTOSLEEP_DUR_20MS | GYR_PWR_SAVE_MODE);
	if(res != 0)
		cout << "BMX055_G fast power set error: (" << res << ')' << endl;

	float deltaX = 0.0f;
	float deltaY = 0.0f;
	float deltaZ = 0.0f;

	timespec ts;
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);
	double lastTime = ts.tv_nsec;

	int lastFill = 0;
	while(tempCount < 50)
	{
		int fill = bmx055_Gyro.getFIFOFillStatus();
		if(fill == -1)
			cout << "BMX055_G FIFO fill get error: (" << fill << ')' << endl;
		int overrun = bmx055_Gyro.getFIFOOverrunStatus();
		if(overrun == -1)
			cout << "BMX055_G FIFO overrun get error: (" << overrun << ')' << endl;

		//if(lastFill != fill | overrun > 0)
		//{
		//	cout << fill << ' ' << flush;

		//	if(overrun > 0)
		//		cout << "Overrun! ";
		//}
		//lastFill = fill;

		if(fill >= 1)
		{
			//cout << endl;

			if(fill > 10)
				fill = 10;

			BMX055_G::gyrData tempData[10];

			res = bmx055_Gyro.getFIFOData(tempData, fill);
			if(res == -1)
				cout << "BMX055_G FIFO data get error: (" << res << ')' << endl;

			clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);
			double curTime = ts.tv_nsec;

			double deltaTime = (curTime - lastTime)*0.00000001f;
			deltaTime /= fill;
			lastTime = curTime;
			//cout << deltaTime << ' ' << curTime << ' ' << lastTime << endl;

			for(int i = 0; i < fill; i++)
			{
				//deltaX += tempData[i].XAxis*deltaTime;
				//deltaY += tempData[i].YAxis*deltaTime;
				//deltaZ += tempData[i].ZAxis*deltaTime;

				//cout << i << "\tX:" << deltaX << "deg"
				//	<< "\tY:" << deltaY << "deg"
				//	<< "\tZ:" << deltaZ << "deg" << endl;

				tempCount++;
				cout << i << "\tX:" << tempData[i].XAxis << "deg/s"
					<< "\tY:" << tempData[i].YAxis << "deg/s"
					<< "\tZ:" << tempData[i].ZAxis << "deg/s" << endl;
			}

		}

		//int temp = bmx055_Gyro.getChipTemp();
		//if(temp == -111)
		//	cout << "BMX055 chip temperature error: (" << res << ')' << endl;

		//cout << temp << 'C' << endl;

		usleep(10000);
	}
#endif
#ifdef DEBUG_ACCEL

	BMX055_A bmx055_Accel;

	res = bmx055_Accel.initialize(&commsI);
	if(res != 0)
	{
		cout << "BMX055_A initialize error: (" << res << ')' << endl;
		return(-1);
	}
	//res = bmx055_Gyro.initialize(&commsI);
	//if(res != 0)
	//	cout << "BMX055_G initialize error: (" << res << ')' << endl;

	if(argc > 1)
	{
		if(strcmp(argv[1], "-r") == 0)
		{
			cout << "reset chip!" << endl;
			bmx055_Accel.reset();
			return(0);
		}
	}

	res = bmx055_Accel.setAccelRange(ACC_RANGE_4G);
	if(res != 0)
		cout << "BMX055_A range set error: (" << res << ')' << endl;

	res = bmx055_Accel.setAccelFilterBandwidth(ACC_F_BANDWIDTH_125HZ);
	if(res != 0)
		cout << "BMX055_A bandwidth set error: (" << res << ')' << endl;

	res = bmx055_Accel.setLowPowerConfig(ACC_SLEEPTIMER_MODE | ACC_LOWPOWER_MODE2);
	if(res != 0)
		cout << "BMX055_A lowpower set error: (" << res << ')' << endl;

	res = bmx055_Accel.setFIFOConfig(ACC_FIFO_FIFO | ACC_FIFODATA_XYZ);
	if(res != 0)
		cout << "BMX055_A FIFO config set 2 error: (" << res << ')' << endl;

	res = bmx055_Accel.setPowermodeAndSleepDur(ACC_SLEEP_DUR_50MS | ACC_LOW_POWER);
	if(res != 0)
		cout << "BMX055_A powermode set error: (" << res << ')' << endl;


	tempCount = 0;
	lastFill = 0;
	while(tempCount < 50)
	{
		int fill = bmx055_Accel.getFIFOFillStatus();
		if(fill == -1)
			cout << "BMX055_A FIFO fill get error: (" << res << ')' << endl;
		int overrun = bmx055_Accel.getFIFOOverrunStatus();
		if(overrun == -1)
			cout << "BMX055_A FIFO overrun get error: (" << res << ')' << endl;

		//if(lastFill != fill)
		//{
		//	cout << fill << ' ' << flush;

		//	if(overrun > 0)
		//		cout << "Overrun! ";
		//}
		//lastFill = fill;

		if(fill >= 1)
		{
			//cout << endl;

			if(fill > 10)
				fill = 10;

			BMX055_A::accData tempData[10];

			res = bmx055_Accel.getFIFOData(tempData, fill);
			if(res == -1)
				cout << "BMX055 FIFO data get error: (" << res << ')' << endl;

			for(int i = 0; i < 1; i++)
			{
				tempCount++;
				cout << i << "\tX:" << tempData[i].XAxis << 'G'
				<< "\tY:" << tempData[i].YAxis << 'G'
				<< "\tZ:" << tempData[i].ZAxis << 'G' << endl;
			}

		}

		//int temp = bmx055_Accel.getChipTemp();
		//if(temp == -111)
		//	cout << "BMX055_A chip temperature error: (" << res << ')' << endl;

		//cout << temp << 'C' << endl;

		usleep(10000);
	}




	//res = bmx055_Accel.setDataOutputFormat(ACC_F_BANDWIDTH_31_25HZ);
	//if(res != 0)
	//	cout << "BMX055 output format set error: (" << res << ')' << endl;

#endif
#ifdef DEBUG_ATMO

	BME280 bme280Handle;

	res = bme280Handle.initialize(&commsI);
	if(res != 0)
	{
		cout << "BME280 initialize error: (" << res << ')' << endl;
		return(-1);
	}

	if(argc > 1)
	{
		if(strcmp(argv[1], "-r") == 0)
		{
			cout << "reset chip!" << endl;
			bme280Handle.reset();
			return(0);
		}
	}

	res =  bme280Handle.setConfig(T_SB_10);
	if(res != 0)
		cout << "BME280 config error: (" << res << ')' << endl;

	// set up BME 280 parameters
	res = bme280Handle.setHumidityControl(PRESSURE_X1_OVERSAMPLE);
	if(res != 0)
		cout << "BME280 ctrl_hum error: (" << res << ')' << endl;

	res =  bme280Handle.setMeasurementControl(
		FORCED_MODE | TEMPERATURE_X1_OVERSAMPLE | PRESSURE_X1_OVERSAMPLE);
	if(res != 0)
		cout << "BME280 ctrl_meas error: (" << res << ')' << endl;

	cout << "started measurements:" << endl;

	tempCount = 0;
	while(tempCount < 50)
	{
		usleep(50000);

		res = bme280Handle.getMeasurementResults();
		if(res != 0)
			cout << "BME280 measurement get error: (" << res << ')' << endl;

		cout << bme280Handle.getLastTemperature() << "C\t"
			<< bme280Handle.getLastPressure() << "Pa\t"
			<< bme280Handle.getLastHumidity() << "%RH" << endl;

		tempCount++;

		res = bme280Handle.takeMeasurement();
		if(res != 0)
			cout << "BME280 measurement error: (" << res << ')' << endl;
	}

	cout << "Success!" << endl;
#endif
#ifdef DEBUG_MAGN

	BMX055_M bmx055_Magn;

	res = bmx055_Magn.initialize(&commsI);
	if(res != 0)
	{
		cout << "BMX055_M initialize error: (" << res << ')' << endl;
		return(-1);
	}
	usleep(10000);

	if(argc > 1)
	{
		if(strcmp(argv[1], "-r") == 0)
		{
			cout << "reset chip!" << endl;
			bmx055_Magn.reset();
			return(0);
		}
	}

	res = bmx055_Magn.setOpperationSettings(MAG_OUT_DATARATE_2HZ | MAG_NORMAL_MODE);
	if(res != 0)
		cout << "BMX055_M opperation settingserror: (" << res << ')' << endl;
	res = bmx055_Magn.setRepititions(0x10,0x10);
	if(res != 0)
		cout << "BMX055_M repitions set error: (" << res << ')' << endl;
	res = bmx055_Magn.setAxisAndPins(MAG_CHN_X_EN | MAG_CHN_Y_EN | MAG_CHN_Z_EN);
	if(res != 0)
		cout << "BMX055_M axes set error: (" << res << ')' << endl;


	tempCount = 0;
	while(tempCount < 50)
	{
		BMX055_M::magData tempData;

		res = bmx055_Magn.getData(&tempData);
		if(res != 0)
			cout << "BMX055_M data get error: (" << res << ')' << endl;

		cout << "X: " << tempData.XAxis << "mT\tY: "
			<< tempData.YAxis << "mT\tZ: "
			<< tempData.ZAxis << "mT\t";
		if(tempData.overflow)
			cout << "Overflow!";
		cout << endl;


		//tempCount++;
		usleep(500000);
	}




	//res = bmx055_Accel.setDataOutputFormat(ACC_F_BANDWIDTH_31_25HZ);
	//if(res != 0)
	//	cout << "BMX055 output format set error: (" << res << ')' << endl;

#endif

	return(0);
}
