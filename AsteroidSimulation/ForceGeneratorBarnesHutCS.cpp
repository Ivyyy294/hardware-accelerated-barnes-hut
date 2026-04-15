#include "pch.h"
#include "ForceGeneratorBarnesHutCS.h"

void ForceGeneratorBarnesHutCS::Start()
{
	m_computeShader.SetMaxSize (1024);
	m_computeShader.Initialize ();
}

void ForceGeneratorBarnesHutCS::Exit()
{
	m_computeShader.ShutdownShader();
}

void ForceGeneratorBarnesHutCS::FixedUpdate()
{
}

void ForceGeneratorBarnesHutCS::ApplyForceForObject(PhysicObject* physicObj) const
{
}

void ForceGeneratorBarnesHutCS::Dispatch()
{
	m_computeShader.Dispatch(1, 1, 1);
}

void ForceGeneratorBarnesHutCS::ReadBack()
{
	Debug::Log("Wait for compute shader...");
	
	while (!m_computeShader.IsDone())
		continue;

	Debug::Log("´Compute shader done!");
	m_computeShader.CopyData();
}
