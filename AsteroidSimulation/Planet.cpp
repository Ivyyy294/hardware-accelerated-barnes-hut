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
	colorMaterial->SetColor(Color(0, 0, 255));

	//MeshRenderer
	auto meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetMesh(Mesh::Sphere(64, 64));
	meshRenderer->SetMaterial(colorMaterial);

	//Physics
	auto physic = AddComponent<PhysicObject>();
	physic->SetMass (100);
	physic->SetStatic(true);

	//Collider
	auto collider = AddComponent<CircleCollider>();
	collider->SetRadius(0.5f);

	//Add Force Generator
	AddComponent<PhysicObjectGravityGenerator>();

	transform.SetLocalScale({ 2.0f, 2.0f, 2.0f });
}
