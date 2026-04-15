#include "pch.h"
#include "ForceGeneratorBarnesHut.h"

void ForceGeneratorBarnesHut::OnDrawGizmo()
{
	m_octTree.DrawGizmo();
}

void ForceGeneratorBarnesHut::ApplyForceForObject(PhysicObject* physicObj) const
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

void ForceGeneratorBarnesHut::BuildTree()
{
	m_octTree.Clear();

	float bounds = std::ceilf (GetMaxCoordinate());

	m_octTree.SetHalfExtends({bounds, bounds, bounds});

	for (size_t i = 0; i < m_objects.size(); ++i)
		m_octTree.Insert(m_objects[i]);

	m_octTree.CalculateMass();
}

float ForceGeneratorBarnesHut::GetMaxCoordinate()
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
