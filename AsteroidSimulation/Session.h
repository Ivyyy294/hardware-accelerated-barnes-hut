#pragma once

#include "SessionVariables.h"
#include "SessionFile.h"

class Session
{
//Variables
public:
	SessionVariables m_variables;
	SessionFile m_variablesFile;
	SessionFile m_positionFile;
	SessionFile m_forceFile;
	SessionFile m_gpuMemDumpFile;
	SessionFile m_shaderTimeFile;
	SessionFile m_forceCalcTimeFile;

	bool m_logDebugInfo = false;

//Methods
private:
	Session();

public:
	static Session& Me();
};

