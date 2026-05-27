#include "pch.h"

#include <IvyyyMeshRenderer.h>
#include <IvyyyPhysicObject.h>
#include <IvyyyCircleCollider.h>
#include <IvyyyPhysicObjectGravityGenerator.h>

#include "Asteroid.h"
#include "KeplerOrbit.h"

std::shared_ptr<ColorMaterial> Asteroid::m_material = nullptr;
Mesh Asteroid::m_mesh = Mesh::Sphere(16, 16);

Asteroid::Asteroid()
{
	//Material
	if (m_material == nullptr)
	{
		m_material = std::make_shared<ColorMaterial> ();
		m_material->SetColor (Color (147, 146, 140));
	}

	//MeshRenderer
	auto meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetMesh (m_mesh);
	meshRenderer->SetMaterial (m_material);

	//Physics
	m_physicObject = AddComponent<PhysicObject>().get();
	//physic->SetMass(0.1f);

	//Collider
	//auto collider = AddComponent<CircleCollider>();
	//collider->SetRadius (0.5f);

	transform.SetLocalScale ({0.05f, 0.05f, 0.05f});
}

void Asteroid::SetPhysicalProperties(float mass, float size)
{
	m_physicObject->SetMass(mass);
	transform.SetLocalScale(Vector3{ 0.05f, 0.05f, 0.05f } * size);
}

void Asteroid::SetOrbit(OrbitalElement oe, const PhysicObject* soi)
{
	OrbitalData orbitalData = KeplerOrbit::CallculateOrbitalData (oe, soi->GetMass());
	Vector3 pos = soi->GetTransform()->GetPosition() + KeplerOrbit::CalculateOrbitalPositionAtTime (oe, orbitalData, 0.f);
	Vector3 vel = KeplerOrbit::CalculateOrbitalVelocityAtPosition (oe, orbitalData, pos) * -1.f;
	transform.SetPosition (pos);
	m_physicObject->SetVelocity(vel);
}
