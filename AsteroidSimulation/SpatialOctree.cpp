#include "pch.h"
#include "SpatialOctree.h"

SpatialOctree::SpatialOctree(Vector3 center, Vector3 halfExtents)
	: m_center (center)
	, m_halfExtents (halfExtents)
	, m_children ()
{
	m_length = m_halfExtents.Magnitude() * 2.f;
}

bool SpatialOctree::CheckBounds(PhysicObject* physicObject)
{
	Vector3 objCenter = physicObject->GetTransform()->GetPosition();

	Vector3 min = m_center - m_halfExtents;
	Vector3 max = m_center + m_halfExtents;

	if (objCenter.x < min.x || objCenter.y < min.y || objCenter.z < min.z
		|| objCenter.x > max.x || objCenter.y > max.y || objCenter.z > max.z)
		return false;

	return true;
}

void SpatialOctree::Insert(PhysicObject* physicObject)
{
	if (m_physicObject == nullptr && m_childCount == 0)
	{
		m_physicObject = physicObject;
		return;
	}

	if (m_childCount == 0)
	{
		DivideCells();

		for (int i = 0; i < 8; ++i)
		{
			if (m_children[i]->CheckBounds(m_physicObject))
			{
				m_children[i]->Insert(m_physicObject);
				m_physicObject = nullptr;
				break;
			}
		}
	}

	for (int i = 0; i < 8; ++i)
	{
		if (m_children[i]->CheckBounds (physicObject))
		{
			m_children[i]->Insert (physicObject);
			return;
		}
	}
}

void SpatialOctree::CalculateMass()
{
	if (m_physicObject != nullptr)
	{
		m_mass = m_physicObject->GetMass();
		m_centerOfMass = m_physicObject->GetTransform()->GetPosition();
		return;
	}
	
	Vector3 sum;

	for (int i = 0; i < m_childCount; ++i)
	{
		SpatialOctree* child = m_children[i];
		child->CalculateMass();
		m_mass += child->m_mass;
		sum.x += child->m_centerOfMass.x * child->m_mass;
		sum.y += child->m_centerOfMass.y * child->m_mass;
		sum.z += child->m_centerOfMass.z * child->m_mass;
	}

	if (m_mass > 0.f)
		m_centerOfMass = sum / m_mass;
}

void SpatialOctree::DivideCells()
{
	Vector3 childHalfExtents = m_halfExtents * 0.5f;

	int i = 0;

	for (int xi = 0; xi < 2; ++xi)
	{
		for (int yi = 0; yi < 2; ++yi)
		{
			for (int zi = 0; zi < 2; ++zi)
			{
				float dx = m_center.x + (xi ? childHalfExtents.x : -childHalfExtents.x);
				float dy = m_center.y + (yi ? childHalfExtents.y : -childHalfExtents.y);
				float dz = m_center.z + (zi ? childHalfExtents.x : -childHalfExtents.z);

				m_children[i++] = new SpatialOctree ({dx, dy, dz},childHalfExtents);
			}
		}
	}

	m_childCount = 8;
}

void SpatialOctree::Clear()
{
	m_physicObject = nullptr;
	m_mass = 0.f;
	m_centerOfMass = Vector3::Zero;

	for (int i = 0; i < 8; ++i)
	{
		if (m_children[i] != nullptr)
		{
			m_children[i]->Clear();
			delete m_children[i];
			m_children[i] = nullptr;
		}
	}

	m_childCount = 0;
}

void SpatialOctree::DrawGizmo()
{
	std::queue<SpatialOctree*> queue;

	queue.push (this);

	while (!queue.empty())
	{
		SpatialOctree* current = queue.front();
		queue.pop();

		if (current->m_physicObject != nullptr)
			Gizmo::AddCube (current->m_center, current->m_halfExtents, Color::Green, Transform::Space::WORLD);
		else
		{
			for (int i = 0; i < current->m_childCount; ++i)
				queue.push(current->m_children[i]);
		}
	}
}