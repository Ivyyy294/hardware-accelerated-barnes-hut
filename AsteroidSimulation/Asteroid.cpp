#include "pch.h"

#include <IvyyyMeshRenderer.h>
#include <IvyyyPhysicObject.h>
#include <IvyyyCircleCollider.h>
#include <IvyyyPhysicObjectGravityGenerator.h>

#include "Asteroid.h"
#include "ColorMaterial.h"

Asteroid::Asteroid()
{
	//Material
	auto colorMaterial = std::make_shared<ColorMaterial> ();
	colorMaterial->SetColor (Color (147, 146, 140));

	//MeshRenderer
	auto meshRenderer = AddComponent<MeshRenderer>();
	meshRenderer->SetMesh (Mesh::Sphere(16, 16));
	meshRenderer->SetMaterial (colorMaterial);

	//Physics
	auto physic = AddComponent<PhysicObject>();

	//Collider
	auto collider = AddComponent<CircleCollider>();
	collider->SetRadius (0.5f);

	//Add Force Generator
	AddComponent<PhysicObjectGravityGenerator>();

	transform.SetLocalScale ({0.2f, 0.2f, 0.2f});
}
