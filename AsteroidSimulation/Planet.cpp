#include "pch.h"
#include "Planet.h"
#include "ColorMaterial.h"
#include <IvyyyMeshRenderer.h>
#include <IvyyyPhysicObject.h>
#include <IvyyyCircleCollider.h>
#include <IvyyyPhysicObjectGravityGenerator.h>

Planet::Planet()
{
	//Material
	auto colorMaterial = std::make_shared<ColorMaterial>();
	colorMaterial->SetColor(Color(252, 120, 1));

	//MeshRenderer
	auto meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetMesh(Mesh::Sphere(64, 64));
	meshRenderer->SetMaterial(colorMaterial);

	//Physics
	auto physic = AddComponent<PhysicObject>();
	physic->SetMass (25000);

	transform.SetLocalScale({ 2.0f, 2.0f, 2.0f });
}
