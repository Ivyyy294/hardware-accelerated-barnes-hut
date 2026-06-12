#include "pch.h"
#include "SessionVariables.h"
#include "Session.h"

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
			variables.theta = stof(valBuffer1);
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
			variables.seed = valBuffer1;
		}
		else
			return false;

		if (std::getline(file, buffer))
		{
			GetValueFromString(buffer, valBuffer1);
			variables.centralMass = stof(valBuffer1);
		}
		else
			return false;

		if (std::getline(file, buffer))
		{
			GetRangeFromString(buffer, valBuffer1, valBuffer2);
			variables.massRangeAsteroids.x = stof(valBuffer1);
			variables.massRangeAsteroids.y = stof(valBuffer2);
		}
		else
			return false;

		if (std::getline(file, buffer))
		{
			GetValueFromString(buffer, valBuffer1);
			variables.minMassAsteroids = stof(valBuffer1);
		}
		else
			return false;

		return true;
	}

	return false;
}

void SessionVariables::Log()
{
	Session::Me().m_variablesFile.WriteLine("method;" + std::to_string(method));
	Session::Me().m_variablesFile.WriteLine("theta;" + std::to_string(theta));
	Session::Me().m_variablesFile.WriteLine("numAsteroids;" + std::to_string(numAsteroids));
	Session::Me().m_variablesFile.WriteLine("ugc;" + std::to_string(ugc));
	Session::Me().m_variablesFile.WriteLine("fixedDeltaTime;" + std::to_string(fixedDeltaTime));
	Session::Me().m_variablesFile.WriteLine("distRange;" + std::to_string(distRange.x) + ";" + std::to_string(distRange.y));
	Session::Me().m_variablesFile.WriteLine("eRange;" + std::to_string(eRange.x) + ";" + std::to_string(eRange.y));
	Session::Me().m_variablesFile.WriteLine("iRange;" + std::to_string(iRange.x) + ";" + std::to_string(iRange.y));
	Session::Me().m_variablesFile.WriteLine("maxFixedFrames;" + std::to_string(maxFixedFrames));
	Session::Me().m_variablesFile.WriteLine("seed;" + seed);
	Session::Me().m_variablesFile.WriteLine("centralMass;" + std::to_string(centralMass));
	Session::Me().m_variablesFile.WriteLine("massRangeAsteroids;" + std::to_string(massRangeAsteroids.x) + ";" + std::to_string(massRangeAsteroids.y));
	Session::Me().m_variablesFile.WriteLine("minMassAsteroids;" + std::to_string(minMassAsteroids));
}
