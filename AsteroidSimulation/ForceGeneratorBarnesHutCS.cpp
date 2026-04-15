#include "pch.h"
#include "ForceGeneratorBarnesHutCS.h"

void ForceGeneratorBarnesHutCS::Start()
{
	m_computeShader.SetMaxSize (m_objects.size());
	m_computeShader.Initialize ();

	uint64_t x = GetMortonCode64(15);
}

void ForceGeneratorBarnesHutCS::FixedUpdate()
{
	float maxValue = std::ceil (GetMaxCoordinate());

	const size_t max = (1u << 10);

	std::array <unsigned long, 2625> test;

	for (size_t i = 0; i < m_objects.size(); ++i)
	{
		Vector3 pos = m_objects[i]->GetTransform()->GetPosition();
		float x = Normalize(pos.x / maxValue, maxValue);
		float y = Normalize(pos.y / maxValue, maxValue);
		float z = Normalize(pos.z / maxValue, maxValue);

		unsigned long ix = (unsigned long)(x * 512.f);
		unsigned long iy = (unsigned long)(y * 512.f);
		unsigned long iz = (unsigned long)(z * 512.f);

		ix = GetMortonCode32(ix);
		iy = GetMortonCode32(iy);
		iz = GetMortonCode32(iz);

		unsigned long final = ix | iy << 1 | iz << 2;
		test[i] = final;
	}

	std::sort (test.begin(), test.end());

	return;
}

void ForceGeneratorBarnesHutCS::Exit()
{
	m_computeShader.ShutdownShader();
}

void ForceGeneratorBarnesHutCS::ApplyForceForObject(PhysicObject* physicObj) const
{
}

void ForceGeneratorBarnesHutCS::Dispatch()
{
	m_computeShader.SetShaderParameters(m_objects);
	m_computeShader.Dispatch(m_objects.size(), 1, 1);
}

void ForceGeneratorBarnesHutCS::ReadBack()
{
	Debug::Log("Wait for compute shader...");
	
	while (!m_computeShader.IsDone())
		continue;

	Debug::Log("´Compute shader done!");
	m_computeShader.CopyData();
}

float ForceGeneratorBarnesHutCS::Normalize(float val, float max)
{
	return (val + max) / (max + max);
}

int ForceGeneratorBarnesHutCS::Quantize(float val, const int depth)
{
	int x = val * (1 << depth);
	return x;
}

uint64_t ForceGeneratorBarnesHutCS::GetMortonCode64(uint64_t val)
{
	//Limit val to 21bits
	uint64_t x = val & 0x1fffff;

	x = (x | x << 32) & 0x1f00000000ffff; // shift left 32 bits, OR with self, and 00011111000000000000000000000000000000001111111111111111
	x = (x | x << 16) & 0x1f0000ff0000ff; // shift left 32 bits, OR with self, and 00011111000000000000000011111111000000000000000011111111
	x = (x | x << 8) & 0x100f00f00f00f00f; // shift left 32 bits, OR with self, and 0001000000001111000000001111000000001111000000001111000000000000
	x = (x | x << 4) & 0x10c30c30c30c30c3; // shift left 32 bits, OR with self, and 0001000011000011000011000011000011000011000011000011000100000000
	x = (x | x << 2) & 0x1249249249249249;

	return x;
}

unsigned long ForceGeneratorBarnesHutCS::GetMortonCode32(unsigned long val)
{
	val &= 0x000003ff;
	val = (val ^ (val << 16)) & 0xff0000ff;
	val = (val ^ (val << 8)) & 0x0300f00f;
	val = (val ^ (val << 4)) & 0x030c30c3;
	val = (val ^ (val << 2)) & 0x09249249;
	return val;
}
