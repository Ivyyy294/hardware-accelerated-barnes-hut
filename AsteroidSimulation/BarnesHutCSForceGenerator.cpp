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
	if (m_computeShader.IsDone())
	{
		Debug::Log("BarnesHutCSForceGenerator done!");
		m_computeShader.CopyData();
	}

	m_computeShader.Dispatch(1,1,1);
}

void BarnesHutCSForceGenerator::ApplyForceForObject(PhysicObject* physicObj) const
{
}
