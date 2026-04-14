#include "pch.h"
#include "BarnesHutCSForceGenerator.h"

void BarnesHutCSForceGenerator::Start()
{
	m_computeShader.Initialize();
}

void BarnesHutCSForceGenerator::Exit()
{
	m_computeShader.ShutdownShader();
}

void BarnesHutCSForceGenerator::FixedUpdate()
{
}

void BarnesHutCSForceGenerator::ApplyForceForObject(PhysicObject* physicObj) const
{
}

void BarnesHutCSForceGenerator::Dispatch()
{
	m_computeShader.Dispatch(1, 1, 1);
}

void BarnesHutCSForceGenerator::ReadBack()
{
	Debug::Log("Wait for compute shader...");
	
	while (!m_computeShader.IsDone())
		continue;

	Debug::Log("´Compute shader done!");
	m_computeShader.CopyData();
}
