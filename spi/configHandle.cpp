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

int configHandle::initFromJson(json configJson)
{
#define DEBUG_GYRO
#define DEBUG_ACCEL
#define DEBUG_ATMO
#define DEBUG_MAGN

	// check if everything is present
	if(configJson["Atmospheric"].is_null())
		return(-1);
	if(configJson["Gyroscope"].is_null())
		return(-1);
	if(configJson["Accelerometer"].is_null())
		return(-1);
	if(configJson["Magnetometer"].is_null())
		return(-1);

	if(configJson["Atmospheric"]["ID"].is_number())
	{
		return(-1);
	}
	// compleated witout errors
	return(0);
}
