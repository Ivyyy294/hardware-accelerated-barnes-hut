#include "pch.h"
#include "ForceGeneratorBarnesHutBase.h"


float ForceGeneratorBarnesHutBase::GetMaxCoordinate()
{
	float coord;

	for (size_t i = 0; i < m_objects.size(); ++i)
	{
		Vector3 pos = m_objects[i]->GetTransform()->GetPosition();
		float val = std::max <float>(std::max <float>(std::abs(pos.x), std::abs(pos.y)), pos.z);
		coord = std::max<float>(coord, val);
	}

	return coord;
}