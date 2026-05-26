#include "pch.h"
#include "SessionVariables.h"
#include "SessionLoggerManager.h"

bool SessionVariablesLoadFromFile(SessionVariables& variables, const std::string filePath)
{
	std::ifstream file(filePath);

	if (file.is_open())
	{
		std::string buffer;
		std::string valBuffer1;
		std::string valBuffer2;

		auto GetValueFromString = [](const std::string& str, std::string& strVal)
		{
			int splitIndex = str.find_first_of(";") + 1;
			int endIndex = str.find(";", splitIndex);
			strVal = str.substr(splitIndex, endIndex);
		};

		auto GetRangeFromString = [](const std::string& str, std::string& strVal1, std::string& strVal2)
			{
				int splitIndex = str.find_first_of(";") + 1;
				int endIndex = str.find (";", splitIndex);
				strVal1 = str.substr(splitIndex, endIndex - splitIndex);

				splitIndex = str.find(";", endIndex) + 1;
				endIndex = str.find(";", splitIndex);
				strVal2 = str.substr(splitIndex, endIndex - splitIndex);
			};

		if (std::getline(file, buffer))
		{
			GetValueFromString(buffer, valBuffer1);
			variables.method = (SessionVariables::ForceGeneratorMethod) stoi (valBuffer1);
		}
		else
			return false;

		if (std::getline(file, buffer))
		{
			GetValueFromString(buffer, valBuffer1);
			variables.numAsteroids = stoi(valBuffer1);
		}
		else
			return false;

		if (std::getline(file, buffer))
		{
			GetValueFromString(buffer, valBuffer1);
			variables.ugc = stof(valBuffer1);
		}
		else
			return false;

		if (std::getline(file, buffer))
		{
			GetValueFromString(buffer, valBuffer1);
			variables.fixedDeltaTime = stof(valBuffer1);
		}
		else
			return false;

		if (std::getline(file, buffer))
		{
			GetRangeFromString(buffer, valBuffer1, valBuffer2);
			variables.distRange.x = stof(valBuffer1);
			variables.distRange.y = stof(valBuffer2);
		}
		else
			return false;

		if (std::getline(file, buffer))
		{
			GetRangeFromString(buffer, valBuffer1, valBuffer2);
			variables.eRange.x = stof(valBuffer1);
			variables.eRange.y = stof(valBuffer2);
		}
		else
			return false;

		if (std::getline(file, buffer))
		{
			GetRangeFromString(buffer, valBuffer1, valBuffer2);
			variables.iRange.x = stof(valBuffer1);
			variables.iRange.y = stof(valBuffer2);
		}
		else
			return false;

		if (std::getline(file, buffer))
		{
			GetValueFromString(buffer, valBuffer1);
			variables.maxFixedFrames = stoi(valBuffer1);
		}
		else
			return false;

		if (std::getline(file, buffer))
		{
			GetValueFromString(buffer, valBuffer1);
			variables.maxDurationSec = stoi(valBuffer1);
		}
		else
			return false;

		return true;
	}

	return false;
}

void SessionVariables::Log()
{
	SessionLoggerManager* slm = SessionLoggerManager::Me();
	slm->m_variables.WriteLine("method;" + std::to_string(method));
	slm->m_variables.WriteLine("numAsteroids;" + std::to_string(numAsteroids));
	slm->m_variables.WriteLine("ugc;" + std::to_string(ugc));
	slm->m_variables.WriteLine("fixedDeltaTime;" + std::to_string(fixedDeltaTime));
	slm->m_variables.WriteLine("distRange;" + std::to_string(distRange.x) + ";" + std::to_string(distRange.y));
	slm->m_variables.WriteLine("eRange;" + std::to_string(eRange.x) + ";" + std::to_string(eRange.y));
	slm->m_variables.WriteLine("iRange;" + std::to_string(iRange.x) + ";" + std::to_string(iRange.y));
	slm->m_variables.WriteLine("maxFixedFrames;" + std::to_string(maxFixedFrames));
	slm->m_variables.WriteLine("maxDurationSec;" + std::to_string(maxDurationSec));
}
