#include "pch.h"
#include "Planet.h"
#include <IvyyyMeshRenderer.h>
#include <IvyyyPhysicObject.h>
#include <IvyyyCircleCollider.h>
#include <IvyyyPhysicObjectGravityGenerator.h>


Planet::Planet()
	: StellarObject()
{
	//Material
	m_material = std::make_shared<ColorMaterial>();
	m_material->SetColor(Color(252, 120, 1));

	//MeshRenderer
	auto meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetMesh(Mesh::Sphere(64, 64));
	meshRenderer->SetMaterial(m_material);

	//Physics
	m_physicObject->SetMass (25000);

	transform.SetLocalScale({ 2.0f, 2.0f, 2.0f });

	//Ring
	m_ring = AddComponent<PlanetRing>();	
}

void Planet::SetData(const PlanetData& data)
{
	m_material->SetColor(data.color);

	SetPhysicalProperties (data.mass, data.radius);

	m_ring->SetRingData(data.color, data.ringRadius);
}
