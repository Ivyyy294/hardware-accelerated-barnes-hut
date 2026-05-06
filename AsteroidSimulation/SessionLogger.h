#pragma once

class SessionLogger
{
//Variables
private:
	string m_dir;
	bool m_live = false;

	map<string, ofstream> m_fileMap;

//Methods
public:
	static SessionLogger& Me();

	bool StartSession (const string& rootDir);
	void EndSession();

	void RegisterFile(const string& name);
	void WriteLine (const string& fileName, const string& content);
private:
	SessionLogger(){};
	~SessionLogger();

	string GetSessionName();

	bool CreateSessionFiles(const string& targetDir);
};

