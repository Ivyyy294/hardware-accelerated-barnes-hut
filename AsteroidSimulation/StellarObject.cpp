#include "pch.h"
#include "StellarObject.h"
#include "KeplerOrbit.h"

StellarObject::StellarObject()
{
	m_physicObject = AddComponent<PhysicObject>().get();
}

void StellarObject::SetPhysicalProperties(float mass, float size)
{
	m_physicObject->SetMass(mass);
	transform.SetLocalScale(Vector3::One * size);
}

void StellarObject::SetOrbit(OrbitalElement oe, const PhysicObject* soi)
{
	OrbitalData orbitalData = KeplerOrbit::CallculateOrbitalData(oe, soi->GetMass());
	Vector3 pos = soi->GetTransform()->GetPosition() + KeplerOrbit::CalculateOrbitalPositionAtTime(oe, orbitalData, 0.f);
	Vector3 vel = KeplerOrbit::CalculateOrbitalVelocityAtPosition(oe, orbitalData, pos) * -1.f;
	transform.SetPosition(pos);
	m_physicObject->SetVelocity(vel);
}