#pragma once
#include "ForceGeneratorBase.h"
class ForceGeneratorDirect :
    public ForceGeneratorBase
{
//Methods
private:
	void PreCalculateForcesInternal() override;
};

