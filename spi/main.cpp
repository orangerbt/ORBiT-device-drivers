#include <iostream>
#include <string>
#include <cstring>
#include <cstdio>
#include <unistd.h>
#include <sys/time.h>

#include "socket.h"
#include "dataProtocol.h"
#include "commsInterface.h"
#include "BME280.h"
#include "BMX055_ACCEL.h"
#include "BMX055_GYRO.h"
#include "BMX055_MAGN.h"

#include "configHandle.h"
//#include "include/json.hpp"

using namespace std;
//using namespace nlohmann::json;

#define deviceAddr "/dev/spidev1.0"

#define DEBUG_GYRO
#define DEBUG_ACCEL
#define DEBUG_ATMO
#define DEBUG_MAGN

#define ADDR_GYRO 6
#define DEST_GYRO "localhost:2222" // will use any ip version
//#define DEST_GYRO "127.0.0.1:2222" // will use ipv4

#define ADDR_ACCEL 7
#define DEST_ACCEL "localhost:2222"

#define ADDR_ATMO 8
#define DEST_ATMO "localhost:2222"

#define ADDR_MAGN 9
#define DEST_MAGN "localhost:2222"

#define CONF_PATH "Config.json"

int timeDiff(timespec* initial, timespec* final, timespec* result)
{
	if(final->tv_sec > initial->tv_sec || (final->tv_sec == initial->tv_sec && final->tv_nsec >= initial->tv_nsec))
	{
		// final >= initial;
		result->tv_sec = final->tv_sec - initial->tv_sec;

		if(final->tv_nsec >= initial->tv_nsec)
		{
			result->tv_nsec = final->tv_nsec - initial->tv_nsec;
		}
		else
		{
			result->tv_nsec = 1000000000 - (initial->tv_nsec - final->tv_nsec);
			result->tv_sec -= 1;
		}
		return(0);
	}
	timeDiff(final ,initial, result);
	return(-1);
}

timespec divTime(timespec time, int denominator)
{
	timespec result;
	if(denominator <= 0)
	{
		//cout << "Error!!!!!!" << endl;
		result.tv_sec = -1;
		result.tv_nsec = -1;
		return(result);
	}
	result.tv_sec = time.tv_sec / denominator;
	result.tv_nsec = (time.tv_nsec / denominator) + ((time.tv_nsec % denominator) / denominator) * 10000000000;

	if(result.tv_nsec < 0)
	{
		cout << "1st div: " << (time.tv_nsec / denominator) << endl;
		cout << "remainder: " << time.tv_nsec % denominator << endl;
		cout << "2nd div: " << (((time.tv_nsec % denominator) * 10000000000 ) / denominator) << endl;
	}
	return(result);
}

timespec addTime(timespec t1, timespec t2)
{
	timespec result;
	result.tv_sec = t1.tv_sec + t2.tv_sec;
	result.tv_nsec = t1.tv_nsec + t2.tv_nsec;
	if(result.tv_nsec > 1000000000)
	{
		result.tv_nsec -= 1000000000;
		result.tv_sec += 1;
	}
	return(result);
}

string printTime(timespec time)
{
	char buffer[100];
	sprintf(buffer, "%lld.%.9ld", (long long)time.tv_sec, time.tv_nsec);
	return(buffer);
}

int main(int argc, char* argv[])
{
	configHandle config;
	commsInterface commsI;

	if(config.loadFromFile(CONF_PATH) != 0)
	{
		cout << "Error opening config!" << endl;
		return(-1);
	}

	int addressPins[] = {61, 86, 88};
	int res = commsI.initialize(deviceAddr, 3, addressPins);
	if(res != 0)
	{
		cout << "Error on initializing comms interface: (" << res << ')' << endl;
		return(-1);
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
        ident.units = "X deg/s, Y deg/s, Z deg/s";

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
        ident.units = "X G, Y G, Z G";

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
        ident.units = "X mT, Y mT, Z mT";

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
	int fill = 0;
	int overrun = 0;
	int tempCount = 0;
	int backLines = 0;

	dataPacket data;
	string sendString = "";

	timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
// define last time variables for data with multiple measurements
#ifdef DEBUG_GYRO
	timespec lastGyroTime;
	lastGyroTime.tv_sec = ts.tv_sec;
	lastGyroTime.tv_nsec = ts.tv_nsec;
#endif
#ifdef DEBUG_ACCEL
	timespec lastAccelTime;
	lastAccelTime.tv_sec = ts.tv_sec;
	lastAccelTime.tv_nsec = ts.tv_nsec;
#endif

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

			// set up time differences
			clock_gettime(CLOCK_REALTIME, &ts);

			timespec tempTime;
			tempTime.tv_sec = ts.tv_sec;
			tempTime.tv_nsec = ts.tv_nsec;

			//cout << printTime(tempTime) << endl;

			timespec timeDelta; // compute time intervall between readings
			timeDiff(&lastGyroTime, &ts, &timeDelta);
			timeDelta = divTime(timeDelta, fill);

			for(int i = 0; i < fill; i++)
			{
				sendString = to_string(tempData[i].XAxis) + ", " + to_string(tempData[i].YAxis) + ", " + to_string(tempData[i].ZAxis);

				tempTime = addTime(timeDelta, tempTime);
				//cout << printTime(tempTime) << endl;

				data.id = ADDR_GYRO;
				data.data = sendString;
				data.time = printTime(tempTime);

				prot.package(data, &output);
				if(conn.sendDataTo(output, DEST_GYRO))
				{
					return(-1);
				}
			}

			lastGyroTime.tv_sec = ts.tv_sec;
			lastGyroTime.tv_nsec = ts.tv_nsec;

			clock_gettime(CLOCK_REALTIME, &ts);
			timespec lastRecorded;

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

			// set up time differences
			clock_gettime(CLOCK_REALTIME, &ts);
			timespec tempTime;
			tempTime.tv_sec = ts.tv_sec;
			tempTime.tv_nsec = ts.tv_nsec;

			timespec timeDelta; // compute time intervall between readings
			timeDiff(&lastAccelTime, &ts, &timeDelta);
			timeDelta = divTime(timeDelta, fill);

			for(int i = 0; i < fill; i++)
			{
				sendString = to_string(tempData[i].XAxis) + ", " + to_string(tempData[i].YAxis) + ", " + to_string(tempData[i].ZAxis);

				tempTime = addTime(timeDelta, tempTime);

				data.id = ADDR_ACCEL;
				data.data = sendString;
				data.time = printTime(tempTime);

				prot.package(data, &output);
				if(conn.sendDataTo(output, DEST_ACCEL))
				{
					return(-1);
				}
			}

			lastAccelTime.tv_sec = ts.tv_sec;
			lastAccelTime.tv_nsec = ts.tv_nsec;

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
		clock_gettime(CLOCK_REALTIME, &ts);

		data.id = ADDR_ATMO;
		data.data = sendString;
		data.time = printTime(ts);

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

		clock_gettime(CLOCK_REALTIME, &ts);
		sendString = to_string(tempData.XAxis) + ", " + to_string(tempData.YAxis) + ", " + to_string(tempData.ZAxis);
		data.id = ADDR_MAGN;
		data.data = sendString;
		data.time = printTime(ts);

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
