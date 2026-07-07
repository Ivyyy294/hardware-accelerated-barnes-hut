#pragma once

class PlanetRing :
    public Component
{
	//Variables
	private:
		PhysicObject* m_rb;
		Color m_color;
		float m_radius;

	//Methods
	public:
		void Start() override;
		void Update() override;

		void SetRingData (const Color& color, const float radius);
};

