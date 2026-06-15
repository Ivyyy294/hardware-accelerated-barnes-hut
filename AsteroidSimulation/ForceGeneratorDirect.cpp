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
			PhysicObject* physicObj = m_objects[j];

			Vector3 direction = physicObj->GetTransform()->GetPosition() - pPos;
			float sqrDistance = direction.SquareMagnitude();
			float invDistance = 1.f / sqrtf(sqrDistance + 0.0025f);
			float invDistance3 = invDistance * invDistance * invDistance;

			pForce += physicObj->GetMass() * direction * invDistance3;
		}

		pForce = Ivyyy::IvyyyPhysic::GravitationalConstant() * pMass * pForce;
		m_forceBuffer[i] = { pForce.x, pForce.y, pForce.z };
	}
}
