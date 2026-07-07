#include "pch.h"

#include <IvyyyMeshRenderer.h>
#include <IvyyyPhysicObject.h>
#include <IvyyyCircleCollider.h>
#include "Asteroid.h"


std::shared_ptr<ColorMaterial> Asteroid::m_material = nullptr;
Mesh Asteroid::m_mesh = Mesh::Sphere(16, 16);

Asteroid::Asteroid()
	: StellarObject()
{
	//Material
	if (m_material == nullptr)
	{
		m_material = std::make_shared<ColorMaterial> ();
		m_material->SetColor (Color (200, 200, 195));
	}

	//MeshRenderer
	auto meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetMesh (m_mesh);
	meshRenderer->SetMaterial (m_material);
}
