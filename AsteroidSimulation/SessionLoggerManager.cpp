#include "pch.h"
#include "SessionLoggerManager.h"
#include "SessionLogger.h"
#include <IvyyyEngine.h>

SessionLoggerManager* SessionLoggerManager::m_me = nullptr;

void SessionLoggerManager::OnEnable()
{
	m_me = this;
	SessionLogger::Me().StartSession ("..//Out");
}

void SessionLoggerManager::Update()
{
	if (Input::KeyDown(Key::KEY_ESC))
		IvyyyEngine::Me()->Exit();
}

void SessionLoggerManager::OnDisable()
{
	m_me = nullptr;
	SessionLogger::Me().EndSession();
}

SessionLoggerManager::SessionLoggerManager()
	: m_variables ("variables.csv")
	, m_position ("positions.csv")
	, m_force ("force.csv")
	, m_gpuMemDump ("gpuMemDump.csv")
	, m_shaderTime ("shader.csv")
	, m_frameTime ("frameTime.csv")
{
}

SessionLoggerManager* SessionLoggerManager::Me()
{
	return m_me;
}