#include "pch.h"
#include "ForceGeneratorDirect.h"

void ForceGeneratorDirect::PreCalculateForcesInternal()
{
	const size_t size = 4;
	std::future<void>* tList = new std::future<void>[size];

	for (size_t i = 0; i < size; ++i)
		tList[i] = std::async(std::launch::async, &ForceGeneratorDirect::PreCalculateForcesChunked, this, i, size);

	for (int i = 0; i < (int)size; ++i)
		tList[i].wait();

	delete[] tList;
}

void ForceGeneratorDirect::PreCalculateForcesChunked(const size_t startIndex, const size_t stride)
{
	for (size_t i = startIndex; i < m_objects.size(); i += stride)
	{
		PhysicObject* m_rb = m_objects[i];
		Vector3 pPos = m_rb->GetTransform()->GetPosition();
		float pMass = m_rb->GetMass();

		if (m_rb->IsStatic())
			continue;

		Vector3 pForce = Vector3::Zero;

		for (size_t j = 0; j < m_objects.size(); ++j)
		{
			if (i == j)
				continue;

			PhysicObject* physicObj = m_objects[j];

			Vector3 diff = physicObj->GetTransform()->GetPosition() - pPos;
			Vector3 direction = diff.Normalized();
			float distance = diff.Magnitude();

			if (distance < 0.01f)
				continue;

			float force = Ivyyy::IvyyyPhysic::GravitationalConstant() * ((pMass * physicObj->GetMass()) / (distance * distance));

			Vector3 forceVec = direction * force;
			pForce += forceVec;
		}

		m_forceBuffer[i] = { pForce.x, pForce.y, pForce.z };
	}
}
