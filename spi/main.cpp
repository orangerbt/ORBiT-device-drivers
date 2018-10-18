#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/time.h>

#include "socket.h"
#include "dataProtocol.h"
#include "commsInterface.h"
#include "BME280.h"
#include "BMX055_ACCEL.h"
#include "BMX055_GYRO.h"
#include "BMX055_MAGN.h"

using namespace std;

#define deviceAddr "/dev/spidev1.0"

#define DEBUG_GYRO
#define DEBUG_ACCEL
#define DEBUG_ATMO
#define DEBUG_MAGN

#define ADDR_GYRO 6
//#define DEST_GYRO "localhost:2222"
#define DEST_GYRO "127.0.0.1:2222"

#define ADDR_ACCEL 7
#define DEST_ACCEL "localhost:2222"

#define ADDR_ATMO 8
#define DEST_ATMO "localhost:2222"

#define ADDR_MAGN 9
#define DEST_MAGN "localhost:2222"

int main(int argc, char* argv[])
{
	commsInterface commsI;

	int addressPins[] = {61, 86, 88};
	int res = commsI.initialize(deviceAddr, 3, addressPins);
	if(res != 0)
	{
		cout << "Error on initializing comms interface: (" << res << ')' << endl;
		return(1);
	}

	udpSocketHandle conn;

	string output;
	dataProtocol prot;
	dataIdentifier ident;

#ifdef DEBUG_GYRO
	BMX055_G bmx055_Gyro;

	cout << "Initializing BMX055 GYRO..." << endl;

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

	res = bmx055_Gyro.setGyroFilterBandwidth(GYR_F_BANDWIDTH_12HZ);//32
	if(res != 0)
		cout << "BMX055_G bandwidth set error: (" << res << ')' << endl;

	res = bmx055_Gyro.setFIFOConfig(GYR_FIFO_FIFO | GYR_FIFODATA_XYZ);
	if(res != 0)
		cout << "BMX055_G FIFO config set 2 error: (" << res << ')' << endl;

	res = bmx055_Gyro.setPowermodeAndSleepDur(GYR_SLEEP_DUR_20MS | GYR_NORMAL_POWER);
	if(res != 0)
		cout << "BMX055_G powermode set error: (" << res << ')' << endl;

	res = bmx055_Gyro.setFastPwrConfig(GYR_AUTOSLEEP_DUR_40MS | GYR_PWR_SAVE_MODE);
	if(res != 0)
		cout << "BMX055_G fast power set error: (" << res << ')' << endl;


	ident.id = ADDR_GYRO;
        ident.description = "Gyroscope 1";
        ident.units = "deg/s";

	prot.package(ident, &output);
	if(conn.sendDataTo(output, DEST_GYRO))
	{
		return(-1);
	}


#endif
#ifdef DEBUG_ACCEL

	BMX055_A bmx055_Accel;

	cout << "Initializing BMX055 ACCEL..." << endl;

	res = bmx055_Accel.initialize(&commsI);
	if(res != 0)
	{
		cout << "BMX055_A initialize error: (" << res << ')' << endl;
		return(-1);
	}

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

	ident.id = ADDR_ACCEL;
        ident.description = "Accelerometer 1";
        ident.units = "G";

	prot.package(ident, &output);
	if(conn.sendDataTo(output, DEST_ACCEL))
	{
		return(-1);
	}

#endif
#ifdef DEBUG_ATMO

	BME280 bme280Handle;

	cout << "Initializing BME280..." << endl;

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

	ident.id = ADDR_ATMO;
        ident.description = "Atmospheric probe 1";
        ident.units = "C, PA, %RH";

	prot.package(ident, &output);
	if(conn.sendDataTo(output, DEST_ATMO))
	{
		return(-1);
	}

#endif
#ifdef DEBUG_MAGN

	BMX055_M bmx055_Magn;

	cout << "Initializing BMX055 MAGN..." << endl;

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
			usleep(5000000);
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

	ident.id = ADDR_MAGN;
        ident.description = "Magnetometer 1";
        ident.units = "mT";

	prot.package(ident, &output);
	if(conn.sendDataTo(output, DEST_MAGN))
	{
		return(-1);
	}

#endif

	cout << "Initialization Done!" << endl;
	cout << "Starting measurements..." << endl;


// ********************************* Measurements start *********************************

	// stuff about time, that might get used later
	//timespec ts;
	//clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);
	//double lastTime = ts.tv_nsec;

	int fill = 0;
	int overrun = 0;
	int tempCount = 0;
	int backLines = 0;

	dataPacket data;
	string sendString = "";


	while(tempCount < 50)
	{
		backLines = 0;
		cout << "\nMeasurement number: " << tempCount << '\n' << endl;

		backLines += 2;
#ifdef DEBUG_GYRO

		fill = bmx055_Gyro.getFIFOFillStatus();
		if(fill == -1)
			cout << "BMX055_G FIFO fill get error: (" << fill << ')' << endl;

		overrun = bmx055_Gyro.getFIFOOverrunStatus();
		if(overrun == -1)
			cout << "BMX055_G FIFO overrun get error: (" << overrun << ')' << endl;

		cout << "Gyro Fill: " << fill << endl;
		if(fill >= 1)
		{
			if(fill > 100)
				fill = 100;

			BMX055_G::gyrData tempData[100];

			res = bmx055_Gyro.getFIFOData(tempData, fill);
			if(res == -1)
				cout << "BMX055_G FIFO data get error: (" << res << ')' << endl;

			//clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts);

			for(int i = 0; i < fill; i++)
			{
				sendString = to_string(tempData[i].XAxis) + ", " + to_string(tempData[i].YAxis) + ", " + to_string(tempData[i].ZAxis);

				data.id = ADDR_MAGN;
				data.data = sendString;
				data.time = "0";

				prot.package(data, &output);
				if(conn.sendDataTo(output, DEST_MAGN))
				{
					return(-1);
				}
			}

			cout << "\x1b[2K";
			cout << "X:" << tempData[fill-1].XAxis << "deg/s"
			     << "\tY:" << tempData[fill-1].YAxis << "deg/s"
			     << "\tZ:" << tempData[fill-1].ZAxis << "deg/s" << endl;
		}
		backLines += 2;

#endif
#ifdef DEBUG_ACCEL

		fill = bmx055_Accel.getFIFOFillStatus();
		if(fill == -1)
			cout << "BMX055_A FIFO fill get error: (" << res << ')' << endl;
		overrun = bmx055_Accel.getFIFOOverrunStatus();
		if(overrun == -1)
			cout << "BMX055_A FIFO overrun get error: (" << res << ')' << endl;

		cout << "Accel Fill: " << fill << endl;
		if(fill >= 1)
		{
			if(fill > 100)
				fill = 100;

			BMX055_A::accData tempData[100];

			res = bmx055_Accel.getFIFOData(tempData, fill);
			if(res == -1)
				cout << "BMX055 FIFO data get error: (" << res << ')' << endl;

			for(int i = 0; i < fill; i++)
			{
				sendString = to_string(tempData[i].XAxis) + ", " + to_string(tempData[i].YAxis) + ", " + to_string(tempData[i].ZAxis);

				data.id = ADDR_ACCEL;
				data.data = sendString;
				data.time = "0";

				prot.package(data, &output);
				if(conn.sendDataTo(output, DEST_ACCEL))
				{
					return(-1);
				}
			}
			cout << "\x1b[2K";
			cout << "X:" << tempData[fill-1].XAxis << 'G'
			     << "\tY:" << tempData[fill-1].YAxis << 'G'
			     << "\tZ:" << tempData[fill-1].ZAxis << 'G' << endl;
		}
		backLines += 2;

#endif
#ifdef DEBUG_ATMO

		res = bme280Handle.getMeasurementResults();
		if(res != 0)
			cout << "BME280 measurement get error: (" << res << ')' << endl;

		cout << "\x1b[2K";
		cout << bme280Handle.getLastTemperature() << "C\t"
			<< bme280Handle.getLastPressure() << "Pa\t"
			<< bme280Handle.getLastHumidity() << "%RH" << endl;

		sendString = to_string(bme280Handle.getLastTemperature()) + ", " + to_string(bme280Handle.getLastPressure()) + ", " + to_string(bme280Handle.getLastHumidity());
		data.id = ADDR_ATMO;
		data.data = sendString;
		data.time = "0";

		prot.package(data, &output);
		if(conn.sendDataTo(output, DEST_ATMO))
		{
			return(-1);
		}

		res = bme280Handle.takeMeasurement();
		if(res != 0)
			cout << "BME280 measurement error: (" << res << ')' << endl;

		backLines += 1;

#endif
#ifdef DEBUG_MAGN

		BMX055_M::magData tempData;

		res = bmx055_Magn.getData(&tempData);
		if(res != 0)
			cout << "BMX055_M data get error: (" << res << ')' << endl;

		cout << "\x1b[2K";
		cout << "X: " << tempData.XAxis << "mT\tY: "
			<< tempData.YAxis << "mT\tZ: "
			<< tempData.ZAxis << "mT\t";
		if(tempData.overflow)
			cout << "Overflow!";
		cout << endl;

		sendString = to_string(tempData.XAxis) + ", " + to_string(tempData.YAxis) + ", " + to_string(tempData.ZAxis);
		data.id = ADDR_MAGN;
		data.data = sendString;
		data.time = "0";

		prot.package(data, &output);
		if(conn.sendDataTo(output, DEST_MAGN))
		{
			return(-1);
		}


		backLines += 1;


#endif
		cout << "\x1b[" << backLines + 1  << "A" << flush;
		tempCount++;
	}

	for(int i = 0; i < backLines; i++)
	{
		cout << "\n";
	}
	cout << endl;

	return(0);
}
