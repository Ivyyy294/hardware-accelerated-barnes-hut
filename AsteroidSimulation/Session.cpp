#include "pch.h"
#include "Session.h"

Session::Session()
	: m_variablesFile("variables.csv")
	, m_positionFile("positions.csv")
	, m_forceFile("force.csv")
	, m_gpuMemDumpFile("gpuMemDump.csv")
	, m_shaderTimeFile("shader.csv")
	, m_frameTimeFile("frameTime.csv")
{
}

Session& Session::Me()
{
	static Session me;
	return me;
}
