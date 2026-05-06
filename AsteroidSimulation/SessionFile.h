#pragma once

class SessionFile
{
//Variables
private:
	string m_fileName;

//Methods
public:
	SessionFile (const string& fileName);
	void WriteLine(const string& content);
};

