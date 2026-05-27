#pragma once

#include <IvyyyVector2.h>
#include <string>

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
	float theta = 0.8f;
	int nCrit = 64;
	int numAsteroids;
	float ugc;
	float fixedDeltaTime;
	Ivyyy::Vector2 distRange;
	Ivyyy::Vector2 eRange;
	Ivyyy::Vector2 iRange;
	int maxFixedFrames = 0;
	int maxDurationSec = 0;
	std::string seed = "";
	float centralMass = 0.f;
	Ivyyy::Vector2 massRangeAsteroids = {10.f, 0.5f};
	float minMassAsteroids = 0.1f;

//Methods
public:
	void Log ();
};

bool SessionVariablesLoadFromFile (SessionVariables& variables, const std::string filePath);