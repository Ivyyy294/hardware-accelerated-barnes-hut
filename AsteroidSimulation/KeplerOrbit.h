#pragma once
#include "OrbitalElement.h"

struct OrbitalData
{
	Vector3 center;
	Quaternion orientation;
	Vector3 periapsis;
	float gm;

	float c = 0.f;
	float b = 0.f;
	float t = 0.f;

	//mean motion
	float n = 0.f;

	OrbitalData() {};
	OrbitalData(const OrbitalElement& data);
};

class KeplerOrbit
{
//Methods
public:
	static OrbitalData CallculateOrbitalData (const OrbitalElement& orbitalElement, float parentMass);
	static Vector3 CalculateOrbitalPositionAtTime (const OrbitalElement& orbitalElement, const OrbitalData& orbitalData, float time);
	static Vector3 CalculateOrbitalVelocityAtPosition (const OrbitalElement& orbitalElement, const OrbitalData& orbitalData, Vector3 pos);

private:
	static float CalculateEccentricAnomaly(const OrbitalElement& orbitalElement, const OrbitalData& orbitalData, float time);
};

