#include <iostream>

#include "configHandle.h"
#include "include/jsonParse.hpp"

using namespace std;
using json = nlohmann::json;

int configHandle::loadFromFile(string filePath)
{
	try
	{
		json configJson = jsonParse::parseFile(filePath);
		return(initFromJson(configJson));
	}
	catch (...)
	{
		cerr << "Error while parsing configuration file!" << endl;
		return(-1);
	}

}

int configHandle::loadFromString(string jsonString)
{
	try
	{
		return(initFromJson(json::parse(jsonString)));
	}
	catch (...)
	{
		cerr << "Error while parsing configuration string!" << endl;
		return(-1);
	}
}

void configHandle::printSettings()
{
	for(int i = 0; i < 4; i++)
	{
		switch(i)
		{
		case(DEV_ATMO):
			cout << "--- Atmospheric settings ---" << endl;
			break;
		case(DEV_GYRO):
			cout << "---  Gyrosocpe settings  ---" << endl;
			break;
		case(DEV_MAGN):
			cout << "---Magnetometer settings ---" << endl;
			break;
		case(DEV_ACCE):
			cout << "---Accelerometer settings---" << endl;
			break;
		}

		cout << "ID: " << devID[i] << endl;
		cout << "Description: " << devDesc[i] << endl;
		cout << "Destination: " << devDestAddress[i] << endl;
		cout << endl;
	}
}

int configHandle::getDevID(int dev)
{
	if(dev < 0 || dev > 3)
		return(-1); // invalid device

	return(devID[dev]); // valid, return ID
}

string configHandle::getDevAddress(int dev)
{
	if(dev < 0 || dev > 3)
		return(""); // invalid device

	return(devDestAddress[dev]); // valid, return ID
}

string configHandle::getDevDesc(int dev)
{
	if(dev < 0 || dev > 3)
		return(""); // invalid device

	return(devDesc[dev]); // valid, return ID

}

int configHandle::initFromJson(json configJson)
{
	devDesc[0] = "Atmospheric";
	devDesc[1] = "Gyroscope";
	devDesc[2] = "Magnetometer";
	devDesc[3] = "Accelerometer";

	for(int i = 0; i < 4; i++)
	{
		string devString;

		switch(i)
		{
		case(DEV_ATMO):
			devString = "Atmospheric";
			break;
		case(DEV_GYRO):
			devString = "Gyroscope";
			break;
		case(DEV_MAGN):
			devString = "Magnetometer";
			break;
		case(DEV_ACCE):
			devString = "Accelerometer";
			break;
		}

		// populate with defaults
		devID[i] = i;
		devDesc[i] = devString;
		devDestAddress[i] = "localHost:2222";

		//check if config is present
		if(configJson[devString].is_null())
			continue;

		// populate values
		if(configJson[devString]["ID"].is_number())
			devID[i] = configJson[devString]["ID"];

		if(configJson[devString]["Desc"].is_string())
			devDesc[i] = configJson[devString]["Desc"];

		if(configJson[devString]["Dest"].is_string())
			devDestAddress[i] = configJson[devString]["Dest"];
	}

	// compleated without errors
	return(0);
}
