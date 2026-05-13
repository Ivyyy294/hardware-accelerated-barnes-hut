#include "pch.h"
#include "ForceGeneratorDirect.h"

void ForceGeneratorDirect::PreCalculateForcesInternal()
{
	for (size_t i = 0; i < m_objects.size(); ++i)
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

		m_forceBuffer[i] = {pForce.x, pForce.y, pForce.z};
	}
}
