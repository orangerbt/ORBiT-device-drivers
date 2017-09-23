#include <iostream>
#include <string>

//#include "spiHandle.h"
//#include "gpioHandle.h"
#include "commsInterface.h"
#include "BME280.h"

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

	BME280 bme280Handle;

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
	}

	cout << "Success!" << endl;

	return(0);
}
