#pragma once

#include <IvyyyComponent.h>
#include "SessionFile.h"

class SessionLoggerManager :
    public Component
{
//variables
public:
	SessionFile m_variables;
	SessionFile m_position;
	SessionFile m_force;
	SessionFile m_gpuMemDump;
	SessionFile m_shaderTime;
	SessionFile m_frameTime;

private:
	static SessionLoggerManager* m_me;

//Methods
public:
	SessionLoggerManager();
	static SessionLoggerManager* Me();

	void OnEnable() override;
	void OnDisable() override;
};

