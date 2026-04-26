#include "pch.h"
#include "KeplerOrbit.h"

OrbitalData::OrbitalData(const OrbitalElement& data)
{
	//Apply Longitude of ascending node
	Vector3 ascendingNodeDir = Vector3::Down;
	Vector3 orbitalPlaneNormal = Vector3::Back;

	Quaternion o = Quaternion::AroundAxis(Vector3::Forward, data.o * MathF::Deg2Rad);

	ascendingNodeDir = (o * ascendingNodeDir).Normalized();

	orbitalPlaneNormal = o * orbitalPlaneNormal;

	//Apply inclination
	Quaternion i = Quaternion::AroundAxis(ascendingNodeDir, -data.i * MathF::Deg2Rad);

	orbitalPlaneNormal = (i * orbitalPlaneNormal).Normalized();

	Vector3 periapsisDir = orbitalPlaneNormal.Cross(ascendingNodeDir);

	//Apply Argument of periapsis
	Quaternion w = Quaternion::AroundAxis(orbitalPlaneNormal, -data.w * MathF::Deg2Rad);

	periapsisDir = (w * periapsisDir).Normalized();

	c = data.e * data.a;

	if (c > 0.f)
		b = sqrt(data.a * data.a - c * c);
	else
		b = data.a;

	center = periapsisDir * c;

	Quaternion q_o = Quaternion::AroundAxis(Vector3::Forward, data.o * MathF::Deg2Rad);
	Quaternion q_i = Quaternion::AroundAxis(Vector3::Up, data.i * MathF::Deg2Rad);
	Quaternion q_w = Quaternion::AroundAxis(Vector3::Forward, data.w * MathF::Deg2Rad);

	orientation = (q_o * q_i * q_w);
}

OrbitalData KeplerOrbit::CallculateOrbitalData(const OrbitalElement& orbitalElement, float parentMass)
{
	OrbitalData data = OrbitalData(orbitalElement);

	if (parentMass > 0.0f)
	{
		data.gm = IvyyyPhysic::GravitationalConstant() * parentMass;
		data.n = std::sqrtf(data.gm / (orbitalElement.a * orbitalElement.a * orbitalElement.a));
		data.t = 2.f * float(M_PI) / data.n;
	}

	return data;
}

Vector3 KeplerOrbit::CalculateOrbitalPositionAtTime(const OrbitalElement& orbitalElement, const OrbitalData& orbitalData, float time)
{
	float eccentricAnomaly = CalculateEccentricAnomaly(orbitalElement, orbitalData, time);

	// --- 3. Position in ORBITAL PLANE (XY, Z = 0) ---
	float a = orbitalElement.a;
	float e = orbitalElement.e;

	Vector3 orbitalPosition(
		a * (std::cos(eccentricAnomaly) - e),
		a * std::sqrt(1.0f - e * e) * std::sin(eccentricAnomaly),
		0.0f
	);

	// --- 5. Rotate into world space ---
	Vector3 pos = orbitalData.orientation * orbitalPosition;

	return pos;
}

Vector3 KeplerOrbit::CalculateOrbitalVelocityAtPosition(const OrbitalElement& orbitalElement, const OrbitalData& orbitalData, Vector3 pos)
{
	float mu = orbitalData.gm;
	float a = orbitalElement.a;        // you should store this explicitly
	float e = orbitalData.c / a;

	// 1. Transform position into orbital space (focus-centered)
	Vector3 rLocal =
		orbitalData.orientation.Inverse() * pos;

	rLocal -= orbitalData.center; // shift to focus frame

	float r2 = rLocal.SquareMagnitude();
	float r = sqrt(r2);

	if (r < 1e-6f)
		return Vector3::Zero;

	// 2. Radial unit vector
	Vector3 rHat = rLocal / r;

	// 3. Orbital plane normal (Z axis in local space)
	Vector3 hHat = Vector3::Back; // since orbit is XY plane

	// 4. Tangent direction (prograde direction)
	Vector3 tHat = hHat.Cross(rHat).Normalized();

	// 5. Speed (vis-viva)
	float v = sqrt(mu * (2.0f / r - 1.0f / a));

	// 6. Direction: ensure correct sign (prograde consistency)
	// If needed, enforce direction using dot with velocity history or periapsis direction.
	Vector3 vLocal = tHat * v;

	// 7. Back to world space
	return orbitalData.orientation * vLocal;
}

float KeplerOrbit::CalculateEccentricAnomaly(const OrbitalElement& orbitalElement, const OrbitalData& orbitalData, float time)
{
	float M = (orbitalElement.m0 * MathF::Deg2Rad) + orbitalData.n * time;

	// Wrap into [0, 2?]
	M = std::fmod(M, 2.0f * float(M_PI));
	if (M < 0.0f)
		M += 2.0f * float(M_PI);

	// --- 2. Solve Kepler's equation E - e*sin(E) = M ---
	float E = M; // initial guess
	const float e = orbitalElement.e;

	for (int iter = 0; iter < 10; ++iter)
	{
		float f = E - e * std::sin(E) - M;
		float fp = 1.0f - e * std::cos(E);
		E -= f / fp;
	}

	// Wrap E into [0, 2?]
	E = std::fmod(E, 2.0f * float(M_PI));
	if (E < 0.0f)
		E += 2.0f * float(M_PI);

	return E;
}
