#include "pch.h"

#include "BarnesHutForceGenerator.h"


void BarnesHutForceGenerator::FixedUpdate()
{
	m_octTree.Clear();

	for (size_t i = 0; i < m_objects.size(); ++i)
		m_octTree.Insert (m_objects[i]);

	m_octTree.CalculateMass();
}

void BarnesHutForceGenerator::OnDrawGizmo()
{
	m_octTree.DrawGizmo();
}

void BarnesHutForceGenerator::ApplyForceForObject(PhysicObject* physicObj) const
{
	std::queue<const SpatialOctree*> queue;

	Vector3 pos = physicObj->GetTransform()->GetPosition();

	queue.push(&m_octTree);

	while (!queue.empty())
	{
		const SpatialOctree* current = queue.front();
		queue.pop();

		float sqrDistance = (current->CenterOfMass() - pos).SquareMagnitude();
		float result = (current->Length() * current->Length()) / sqrDistance;

		if (result < m_theta)
		{
			if (physicObj->IsStatic())
				return;

			Vector3 diff = current->CenterOfMass() - physicObj->GetTransform()->GetPosition();
			Vector3 direction = diff.Normalized();
			float distance = diff.Magnitude();

			if (distance > 0.f)
			{
				float force = Ivyyy::IvyyyPhysic::GravitationalConstant() * ((current->Mass() * physicObj->GetMass()) / (distance * distance));

				Vector3 forceVec = direction * force;

				physicObj->ApplyForce(forceVec);
			}
		}
		else
		{
			for (int i = 0; i < current->ChildCount(); ++i)
				queue.push(current->Children()[i]);
		}
	}
}
