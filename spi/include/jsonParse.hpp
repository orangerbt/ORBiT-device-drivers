#include <fstream>
#include <string>

#include <iostream>

#include "json.hpp"

//using namespace std;
//using json = nlohmann::json;

class jsonParse
{
public:
	static nlohmann::json parseFile(std::string fileName)
	{
		std::ifstream file;
		file.open(fileName);
		if(!file.is_open())
		{
			throw(0); // throw exeptionm since json does not support this
		}

		nlohmann::json parser;

		std::string fileString;
		while(file.good())
		{
			std::string tempLine;
			getline(file, tempLine);
			for(int i = 0; i < tempLine.length()-1 && tempLine.length() != 0 ; i++)
			{
				if(tempLine[i] == '/' && tempLine[i+1] == '/')
				{
					tempLine.erase(i, tempLine.length() - i);
					break;
				}
			}
			fileString += tempLine;
		}
		file.close();

		parser = nlohmann::json::parse(fileString);
		return(parser);
	}
private:

};
