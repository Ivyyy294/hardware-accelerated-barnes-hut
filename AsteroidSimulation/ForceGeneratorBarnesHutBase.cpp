#include "pch.h"
#include "ForceGeneratorBarnesHutBase.h"
#include "SessionLoggerManager.h"

void ForceGeneratorBarnesHutBase::FixedUpdate()
{
	string posData;
	string forceData;

	for (size_t i = 0; i < m_objects.size(); ++i)
	{
		Vector3 pos = m_objects[i]->GetTransform()->GetPosition();
		Vector3 force = m_objects[i]->GetForce();
		posData += std::to_string(pos.x) + ";" + std::to_string(pos.y) + ";" + std::to_string(pos.z) + ";";
		forceData += std::to_string(force.x) + ";" + std::to_string(force.y) + ";" + std::to_string(force.z) + ";";
	}

	SessionLoggerManager::Me()->m_position.WriteLine(posData);
	SessionLoggerManager::Me()->m_force.WriteLine(forceData);
}

float ForceGeneratorBarnesHutBase::GetMaxCoordinate()
{
	float coord;

	for (size_t i = 0; i < m_objects.size(); ++i)
	{
		Vector3 pos = m_objects[i]->GetTransform()->GetPosition();
		float val = std::max <float>(std::max <float>(std::abs(pos.x), std::abs(pos.y)), pos.z);
		coord = std::max<float>(coord, val);
	}

	return coord;
}