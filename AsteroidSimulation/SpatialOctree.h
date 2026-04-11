#pragma once
#include <IvyyyAABB.h>
#include <IvyyyPhysicObject.h>

class SpatialOctree
{
//Variables
private:

	//Bounds
	Vector3 m_center{ Vector3::Zero };
	Vector3 m_halfExtents{ Vector3::Zero };
	float m_length {0.f};

	//Resident
	PhysicObject* m_physicObject = nullptr;
	
	//Physical quantities
	Vector3 m_centerOfMass {Vector3::Zero};
	float m_mass {0.f};
	
	//Child cells
	int m_childCount {0};
	std::array <SpatialOctree*, 8> m_children {nullptr};

//Methods
public:
	SpatialOctree (Vector3 center, Vector3 halfExtents);

	bool CheckBounds (PhysicObject* physicObject);

	void Insert (PhysicObject* physicObject);

	void CalculateMass();

private:
	void DivideCells ();
};

