#include "pch.h"
#include "SessionFile.h"
#include "SessionLogger.h"

SessionFile::SessionFile(const string& fileName)
	: m_fileName (fileName)
{
	SessionLogger::Me().RegisterFile(m_fileName);
}

void SessionFile::WriteLine(const string& content)
{
	SessionLogger::Me().WriteLine (m_fileName, content);
}
