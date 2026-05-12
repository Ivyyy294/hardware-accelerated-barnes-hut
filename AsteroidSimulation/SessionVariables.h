#pragma once

#include <IvyyyVector2.h>

struct SessionVariables
{
//Defines
public:
	enum ForceGeneratorMethod
	{
		DIRECT,
		BARNES_HUT_CPU,
		BARNES_HUT_GPU
	};

//Variables
public:
	ForceGeneratorMethod method;
	int numAsteroids;
	float ugc;
	float fixedDeltaTime;
	Ivyyy::Vector2 distRange;
	Ivyyy::Vector2 eRange;
	Ivyyy::Vector2 iRange;
	int maxFixedFrames = 0;
	int maxDurationSec = 0;
};