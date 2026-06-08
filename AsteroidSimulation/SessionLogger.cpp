#include "pch.h"
#include "SessionLogger.h"

#include <filesystem>
#include <ctime>


bool SessionLogger::StartSession (const string& rootDir)
{
	string sessionName = GetSessionName();
	string targetDir = rootDir + "//" + sessionName;
	std::filesystem::path path = std::filesystem::path(targetDir);

	if (!filesystem::create_directory(path))
		return false;

	if (!CreateSessionFiles(targetDir))
		return false;

	m_dir = targetDir;
	m_live = true;

	return true;
}

void SessionLogger::EndSession()
{
	for (auto iter = m_fileMap.begin(); iter != m_fileMap.end(); ++iter)
		iter->second.close();

	m_live = false;
}

void SessionLogger::RegisterFile(const string& name)
{
	m_fileMap[name] = ofstream();
}

void SessionLogger::WriteLine(const string& fileName, const string& content)
{
	if (!m_live)
		return;

	m_fileMap.at(fileName) << content << endl;
}

SessionLogger::~SessionLogger()
{
	if (m_live)
		EndSession();
}

string SessionLogger::GetSessionName()
{
	char buffer[32];

	time_t timestamp = time(&timestamp);
	tm datetime;
	localtime_s(&datetime ,&timestamp);

	strftime (buffer,32, "%Y-%b-%d-%H-%M-%S", &datetime);

	return std::string(buffer);
}

bool SessionLogger::CreateSessionFiles(const string& targetDir)
{
	for (auto iter = m_fileMap.begin(); iter != m_fileMap.end(); ++iter)
	{
		iter->second.open(targetDir + "//" + iter->first);
		
		if (!iter->second.is_open())
			return false;
	}

	return true;
}

SessionLogger& SessionLogger::Me()
{
	static SessionLogger me;
	return me;
}
