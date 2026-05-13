#include "pch.h"
#include "ForceGeneratorBase.h"
#include "SessionLoggerManager.h"
#include "FrameTimeRecorder.h"

void ForceGeneratorBase::FixedUpdate()
{
	string posData;
	string forceData;

	posData.reserve (m_objects.size());
	forceData.reserve (m_objects.size());

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

void ForceGeneratorBase::ApplyForceForObject(PhysicObject* physicObj) const
{
	if (physicObj->IsStatic())
		return;

	auto it = std::find(m_objects.begin(), m_objects.end(), physicObj);

	if (it != m_objects.end())
	{
		size_t index = std::distance(m_objects.begin(), it);
		//XMFLOAT3 force = m_forceBuffer[index];
		Vector3 force = Vector3(m_forceBuffer[index].x, m_forceBuffer[index].y, m_forceBuffer[index].z);
		physicObj->ApplyForce(force);

		if (force.SquareMagnitude() == 0.f)
			return;
	}
	else
		return;
}

float ForceGeneratorBase::GetMaxCoordinate()
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

void ForceGeneratorBase::PreCalculateForces()
{
	clock_t startTime = clock();
	PreCalculateForcesInternal();

	clock_t endTime = clock();
	long diff = (long)(endTime - startTime);
	FrameTimeRecorder::Me()->m_forceCalcTime = diff;
}