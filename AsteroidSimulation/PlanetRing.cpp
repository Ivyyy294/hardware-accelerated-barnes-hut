#include "pch.h"
#include "PlanetRing.h"
#include <IvyyyDraw.h>

void PlanetRing::Start()
{
	m_rb = gameObject->GetComponent<PhysicObject>().get();
}

void PlanetRing::Update()
{
	if (m_radius > 0.f)
		Draw::AddCircle (transform->GetPosition(), Vector3::Forward, m_radius, 32, m_color);
}

void PlanetRing::SetRingData(const Color& color, const float radius)
{
	m_color = color;
	m_radius = radius;
}
