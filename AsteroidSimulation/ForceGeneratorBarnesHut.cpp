#include "pch.h"
#include "ForceGeneratorBarnesHut.h"

void ForceGeneratorBarnesHut::OnDrawGizmo()
{
	m_octTree.DrawGizmo();
}

void ForceGeneratorBarnesHut::PreCalculateForcesInternal()
{
	BuildTree();

	const size_t size = 4;
	std::future<void>* tList = new std::future<void>[size];

	for (size_t i = 0; i < size; ++i)
		tList[i] = std::async(std::launch::async, &ForceGeneratorBarnesHut::PreCalculateForcesChunked, this, i, size);

	for (int i = 0; i < (int)size; ++i)
		tList[i].wait();

	delete[] tList;
}

Vector3 ForceGeneratorBarnesHut::CalculateForces(PhysicObject* physicObj)
{
	if (physicObj->IsStatic())
		return Vector3::Zero;

	std::queue<const SpatialOctree*> queue;

	Vector3 pos = physicObj->GetTransform()->GetPosition();
	Vector3 forceSum = Vector3::Zero;

	queue.push(&m_octTree);

	while (!queue.empty())
	{
		const SpatialOctree* current = queue.front();
		queue.pop();

		Vector3 direction = current->CenterOfMass() - pos;
		float sqrDistance = direction.SquareMagnitude();

		float result = (current->Length() * current->Length()) / sqrDistance;

		if (result < m_theta)
		{
			float invDistance = 1.f / sqrtf(sqrDistance + 0.0025f);
			float invDistance3 = invDistance * invDistance * invDistance;
			
			forceSum += current->Mass() * direction * invDistance3;
		}
		else
		{
			for (int i = 0; i < current->ChildCount(); ++i)
				queue.push(current->Children()[i]);
		}
	}

	return (Ivyyy::IvyyyPhysic::GravitationalConstant() * physicObj->GetMass()) * forceSum;
}

void ForceGeneratorBarnesHut::PreCalculateForcesChunked(const size_t startIndex, const size_t stride)
{
	for (size_t i = startIndex; i < m_objects.size(); i +=stride)
	{
		Vector3 force = CalculateForces(m_objects[i]);
		m_forceBuffer[i] = XMFLOAT3(force.x, force.y, force.z);
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
