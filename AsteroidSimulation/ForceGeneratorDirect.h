#pragma once
#include "ForceGeneratorBase.h"
class ForceGeneratorDirect :
    public ForceGeneratorBase
{
//Methods
private:
	void PreCalculateForcesInternal() override;
	void PreCalculateForcesChunked(const size_t startIndex, const size_t stride);
};

