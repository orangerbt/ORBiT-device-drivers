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

	void printSettings();

	int getDevID(int dev);
	std::string getDevAddress(int dev);
	std::string getDevDesc(int dev);

private:
	int initFromJson(nlohmann::json configJson);

	int devID[4];
	std::string devDestAddress[4];
	std::string devDesc[4];

};
