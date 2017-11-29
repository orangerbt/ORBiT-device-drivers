#include <iostream>
#include <string>

//#include "spiHandle.h"
//#include "gpioHandle.h"
#include "commsInterface.h"
#include "BME280.h"
#include "BMX055_ACCEL.h"

using namespace std;

#define deviceAddr "/dev/spidev1.0"

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

	BMX055_A bmx055_Accel;

	res = bmx055_Accel.initialize(&commsI);
	if(res != 0)
		cout << "BMX055 initialize error: (" << res << ')' << endl;

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
		cout << "BMX055 range set error: (" << res << ')' << endl;

	res = bmx055_Accel.setAccelFilterBandwidth(ACC_F_BANDWIDTH_125HZ);
	if(res != 0)
		cout << "BMX055 bandwidth set error: (" << res << ')' << endl;

	res = bmx055_Accel.setLowPowerConfig(ACC_SLEEPTIMER_MODE | ACC_LOWPOWER_MODE2);
	if(res != 0)
		cout << "BMX055 lowpower set error: (" << res << ')' << endl;

	res = bmx055_Accel.setFIFOConfig(ACC_FIFO_FIFO | ACC_FIFODATA_XYZ);
	if(res != 0)
		cout << "BMX055 FIFO config set 2 error: (" << res << ')' << endl;

	res = bmx055_Accel.setPowermodeAndSleepDur(ACC_SLEEP_DUR_50MS | ACC_LOW_POWER);
	if(res != 0)
		cout << "BMX055 powermode set error: (" << res << ')' << endl;


	int tempCount = 0;
	int lastFill = 0;
	while(tempCount < 50)
	{
		int fill = bmx055_Accel.getFIFOFillStatus();
		if(fill == -1)
			cout << "BMX055 FIFO fill get error: (" << res << ')' << endl;
		int overrun = bmx055_Accel.getFIFOOverrunStatus();
		if(overrun == -1)
			cout << "BMX055 FIFO overrun get error: (" << res << ')' << endl;

		/*if(lastFill != fill)
		{
			cout << fill << ' ' << flush;

			if(overrun > 0)
				cout << "Overrun! ";
		}
		lastFill = fill;*/

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
				//tempCount++;
				cout << i << "\tX:" << tempData[i].XAxis << 'G'
				<< "\tY:" << tempData[i].YAxis << 'G'
				<< "\tZ:" << tempData[i].ZAxis << 'G' << endl;
			}

		}

		//int temp = bmx055_Accel.getChipTemp();
		//if(temp == -111)
		//	cout << "BMX055 chip temperature error: (" << res << ')' << endl;

		//cout << temp << 'C' << endl;

		usleep(10000);
	}




	//res = bmx055_Accel.setDataOutputFormat(ACC_F_BANDWIDTH_31_25HZ);
	//if(res != 0)
	//	cout << "BMX055 output format set error: (" << res << ')' << endl;



	/*BME280 bme280Handle;

	res = bme280Handle.initialize(&commsI);
	if(res != 0)
		cout << "BME280 initialize error: (" << res << ')' << endl;

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

	while(true)
	{
		usleep(50000);

		res = bme280Handle.getMeasurementResults();
		if(res != 0)
			cout << "BME280 measurement get error: (" << res << ')' << endl;

		cout << bme280Handle.getLastTemperature() << "C\t"
			<< bme280Handle.getLastPressure() << "Pa\t"
			<< bme280Handle.getLastHumidity() << "%RH" << endl;

		res = bme280Handle.takeMeasurement();
		if(res != 0)
			cout << "BME280 measurement error: (" << res << ')' << endl;
	}*/

	cout << "Success!" << endl;

	return(0);
}
