#include "pch.h"
#include "PlanetData.h"

vector<PlanetData> PlanetData::GetPlanetData()
{
	vector<PlanetData> pVec;
	std::ifstream file("planets.csv");

	if (file.is_open())
	{
		std::string buffer;

		//Discard header
		std::getline(file, buffer);

		while (std::getline(file, buffer))
		{
			PlanetData pData;

			if (ParsePlanetFromString (buffer, pData))
				pVec.push_back(pData);
		}
	}

	return pVec;
}

bool PlanetData::ParsePlanetFromString(const std::string& str, PlanetData& data)
{
	if (str.size() <= 0)
		return false;

	//Color
	int splitIndex = 0;
	int endIndex = str.find_first_of(";", splitIndex);

	if(splitIndex == std::string::npos)
		return false;

	std::string strVal = str.substr(splitIndex, endIndex - splitIndex);
	data.color.r = stof (strVal) / 255.f;

	splitIndex = str.find(";", endIndex) + 1;
	endIndex = str.find(";", splitIndex);

	if (splitIndex == std::string::npos)
		return false;

	strVal = str.substr(splitIndex, endIndex - splitIndex);
	data.color.g = stof(strVal) / 255.f;

	splitIndex = str.find(";", endIndex) + 1;
	endIndex = str.find(";", splitIndex);

	if (splitIndex == std::string::npos)
		return false;

	strVal = str.substr(splitIndex, endIndex - splitIndex);
	data.color.b = stof(strVal) / 255.f;

	//Mass
	splitIndex = str.find(";", endIndex) + 1;
	endIndex = str.find(";", splitIndex);

	if (splitIndex == std::string::npos)
		return false;

	strVal = str.substr(splitIndex, endIndex - splitIndex);
	data.mass = stof(strVal);

	//Radius
	splitIndex = str.find(";", endIndex) + 1;
	endIndex = str.find(";", splitIndex);

	if (splitIndex == std::string::npos)
		return false;

	strVal = str.substr(splitIndex, endIndex - splitIndex);
	data.radius = stof(strVal);

	//a
	splitIndex = str.find(";", endIndex) + 1;
	endIndex = str.find(";", splitIndex);

	if (splitIndex == std::string::npos)
		return false;

	strVal = str.substr(splitIndex, endIndex - splitIndex);
	data.orbitalElement.a = stof(strVal);

	//e
	splitIndex = str.find(";", endIndex) + 1;
	endIndex = str.find(";", splitIndex);

	if (splitIndex == std::string::npos)
		return false;

	strVal = str.substr(splitIndex, endIndex - splitIndex);
	data.orbitalElement.e = stof(strVal);

	//i
	splitIndex = str.find(";", endIndex) + 1;
	endIndex = str.find(";", splitIndex);

	if (splitIndex == std::string::npos)
		return false;

	strVal = str.substr(splitIndex, endIndex - splitIndex);
	data.orbitalElement.i = stof(strVal);

	//0
	splitIndex = str.find(";", endIndex) + 1;
	endIndex = str.find(";", splitIndex);

	if (splitIndex == std::string::npos)
		return false;

	strVal = str.substr(splitIndex, endIndex - splitIndex);
	data.orbitalElement.o = stof(strVal);

	//w
	splitIndex = str.find(";", endIndex) + 1;
	endIndex = str.find(";", splitIndex);

	if (splitIndex == std::string::npos)
		return false;

	strVal = str.substr(splitIndex, endIndex - splitIndex);
	data.orbitalElement.w = stof(strVal);

	//m0
	splitIndex = str.find(";", endIndex) + 1;
	endIndex = str.find(";", splitIndex);

	if (splitIndex == std::string::npos)
		return false;

	strVal = str.substr(splitIndex, endIndex - splitIndex);
	data.orbitalElement.m0 = stof(strVal);
}
