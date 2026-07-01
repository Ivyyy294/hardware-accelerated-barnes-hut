#pragma once

#include <IvyyyColor.h>
#include "OrbitalElement.h"

class PlanetData
{
	//Variables
	public:
		Color color;
		float mass {1.f};
		float radius {0.5f};
		float ringRadius {0.f};
		OrbitalElement orbitalElement;

	//Methods
	public:
		static vector<PlanetData> GetPlanetData();

	private:
		static bool ParsePlanetFromString (const std::string& str, PlanetData& data);
};

