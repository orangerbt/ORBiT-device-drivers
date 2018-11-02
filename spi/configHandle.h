#include <string>

#include "include/json.hpp"

#define DEV_ATMO 0
#define DEV_GYRO 1
#define DEV_MAGN 2
#define DEV_ACCE 3

class configHandle
{
public:
	int loadFromFile(std::string filePath);
	int loadFromString(std::string jsonString);

//	int printSettings();

//	int getDevID(int dev);
//	int getDevAddress(int dev);
//	int getDevDesc(int dev);

private:

	int initFromJson(nlohmann::json configJson);

	int atmoID;
	int gyroID;
	int magnID;
	int acceID;

	std::string atmoDestAddress;
	std::string gyroDestAddress;
	std::string magnDestAddress;
	std::string acceDestAddress;

	std::string atmoDesc;
	std::string gyroDesc;
	std::string magnDesc;
	std::string acceDesc;
};
